#include "CApp.h"
#include "CRand.h"
#include "CScope.h"
#include "Exception.h"
#include "LoggerManager.h"

namespace Util
{
	CApp* g_App = 0;

	//////////////////////////////////////////////////////////////////////////

	class CApp_ProtoTask : public CTask
	{
	public:
		CApp_ProtoTask(const XID& sXid = XID(0,0), SID iSid = 0, Protocol* pProto = 0) : m_sXid(sXid), m_iSid(iSid), m_pProto(pProto)
		{
		};

		virtual ~CApp_ProtoTask()
		{
			g_ProtocolManager->ReleaseProto(m_pProto);
			m_pProto = 0;
		}

	public:
		virtual PVoid  Run(void* pArgs = 0)
		{
			if(m_pProto && g_App)
			{
				Bool bDispatch = g_App->DispatchProto(m_sXid, m_iSid, m_pProto);
				if (!bDispatch)
					FmtError("Dispatch Protocol Failed, Xid: (%u, %u), ProtocolId: %u, Sid: %u", m_sXid.Type, m_sXid.Id, m_pProto->GetType(), m_iSid);
			}
			return 0;
		}

	protected:
		XID	      m_sXid;
		SID		  m_iSid;
		Protocol* m_pProto;
	};

	class CApp_MsgTask : public CTask
	{
	public:
		CApp_MsgTask(const XID& sXid = XID(0,0), CMsg* pMsg = 0) : m_sXid(sXid), m_pMsg(pMsg)
		{
		};		

		CApp_MsgTask(const XIDVector& vXid, CMsg* pMsg) : m_pMsg(pMsg)
		{
			for (Size_t i=0;i<vXid.size();i++)
			{
				m_vXid.push_back(vXid[i]);
			}
		};

		virtual ~CApp_MsgTask()
		{
			m_vXid.clear();
			g_MsgManager->FreeMsg(m_pMsg);
			m_pMsg = 0;
		}

	public:
		virtual PVoid  Run(void* pArgs = 0)
		{
			if(m_pMsg && g_App)
			{
				if (m_vXid.size())
				{
					for (Size_t i=0;i<m_vXid.size();i++)
					{
						m_pMsg->Target = m_vXid[i];
						Bool bDispatch = g_App->DispatchMsg(m_vXid[i], m_pMsg);
						if (!bDispatch)
							FmtError("Dispatch Message Failed, Xid: (%u, %u), MsgId: %u", m_vXid[i].Id, m_pMsg->Msg);
					}
				}
				else
				{
					Bool bDispatch = g_App->DispatchMsg(m_sXid, m_pMsg);
					if (!bDispatch)
						FmtError("Dispatch Message Failed, Xid: (%u, %u), MsgId: %u", m_sXid.Id, m_pMsg->Msg);
				}				
			}
			return 0;
		}

	protected:
		XID	      m_sXid;
		XIDVector m_vXid;
		CMsg*  m_pMsg;
	};

	class CApp_TickTask : public CTask
	{
	public:
		CApp_TickTask(const XID& sXid = XID(0,0)) : m_sXid(sXid)
		{			
		};

		CApp_TickTask(const XIDVector& vXid)
		{
			for (Size_t i=0;i<vXid.size();i++)
			{
				m_vXid.push_back(vXid[i]);
			}
		};

		virtual ~CApp_TickTask()
		{
			m_vXid.clear();
		}

	public:
		virtual void ResetXids(const XIDVector& vXid)
		{
			m_vXid.clear();
			for (Size_t i=0;i<vXid.size();i++)
			{
				m_vXid.push_back(vXid[i]);
			}
		}

		virtual PVoid  Run(void* pArgs = 0)
		{
			if (m_vXid.size())
			{
				for (Size_t i=0;i<m_vXid.size();i++)
				{
					g_App->DispatchTick(m_vXid[i]);
				}
			}
			else
			{
				g_App->DispatchTick(m_sXid);
			}
			return 0;
		}

	protected:
		XID			m_sXid;
		XIDVector	m_vXid;
	};

	//////////////////////////////////////////////////////////////////////////

	CApp::SafeObj::SafeObj(XID sXid) : ObjMan::SafeObj(g_App->GetObjMan(sXid.Type), sXid)
	{
	}

	CApp::SafeObj::~SafeObj()
	{
	}

	//////////////////////////////////////////////////////////////////////////

	CApp::CApp(const XID& sXid) : CAppObj(sXid)
	{
		Assert(g_App == 0);
		g_App     = this;
		m_pThreadPool = 0;
		m_bRunning	  = false;		
		m_pSidXidLock = new CSpinLock;
		m_iMainTid    = COSOperator::GetThreadId();		
	}

	CApp::~CApp()
	{
		Stop();		

		if (m_pThreadPool)
			m_pThreadPool->Close();

		ObjManMap::iterator it = m_mObjMan.begin();
		for (;it!=m_mObjMan.end();it++)
		{
			ObjMan* pMan = it->second;
			DEF_RELEASE(pMan);
		}
		m_mObjMan.clear();
		m_mSidXid.clear();

		DEF_RELEASE(m_pThreadPool);
		DEF_RELEASE(m_pSidXidLock);

		g_App = 0;
	}

	CApp::ObjMan* CApp::CreateObjMan(UInt32 iType)
	{
		ObjMan* pObjMan = GetObjMan(iType);
		if (!pObjMan)
		{
			pObjMan = new ObjMan;
			m_mObjMan[iType] = pObjMan;
		}
		return pObjMan;
	}

	CApp::ObjMan* CApp::GetObjMan(UInt32 iType)
	{
		ObjManMap::const_iterator it = m_mObjMan.find(iType);
		if (it != m_mObjMan.end())
		{
			return (ObjMan*)it->second;
		}
		return 0;
	}

	CAppObj* CApp::CreateObj(const XID& sXid, SID iSid)
	{
		if(!sXid.IsValid())
			return 0;		

		ObjMan* pObjMan = GetObjMan(sXid.Type);
		Assert(pObjMan);

		CAppObj* pObj = g_App->AppCreateObj(sXid);
		if (pObj)
		{
			if(pObjMan->AllocObject(sXid, pObj))	
			{
				if(iSid) 
					BindSidXid(iSid, sXid);	

				return pObj;
			}

			DEF_RELEASE(pObj);
			return 0;
		}

		Assert(false && "Create Obj Null.");
		return 0;
	}

	CAppObj* CApp::AppCreateObj(const XID& sXid)
	{
		return 0;
	}

	Bool  CApp::DeleteObj(const XID& sXid)
	{
		if (sXid.IsValid())
		{
			ObjMan* pObjMan = GetObjMan(sXid.Type);
			Assert(pObjMan);

			return pObjMan->FreeObject(sXid);
		}
		return true;
	}

	XID CApp::GetXidBySid(SID iSid)
	{
		AutoSpinLock(lock, m_pSidXidLock);
		SidXidMap::iterator it = m_mSidXid.find(iSid);
		if (it != m_mSidXid.end())
			return it->second;
		
		return XID(0,0);
	}

	void CApp::UnbindSidXid(SID iSid)
	{
		if(iSid) 
		{
			AutoSpinLock(lock, m_pSidXidLock);
			SidXidMap::iterator it = m_mSidXid.find(iSid);
			if (it != m_mSidXid.end())
				m_mSidXid.erase(it);
		}
	}

	void CApp::BindSidXid(SID iSid,const XID& sXid)
	{
		if (iSid && sXid.IsValid())
		{
			AutoSpinLock(lock, m_pSidXidLock);
			m_mSidXid[iSid] = sXid;
		}
	}

	Bool CApp::Init(Int32 iThread)
	{
		if (!m_pThreadPool)
		{
			m_pThreadPool = new CThreadPool;

			if (!m_pThreadPool->InitPool(iThread, false))
				return false;

			if(!m_pThreadPool->Start())
				return false;	

			return true;
		}
		
		return false;
	}

	Bool CApp::Run()
	{
		if (!m_bRunning)
		{
			m_bRunning = true;

			return true;
		}
		return false;
	}

	Bool CApp::Stop()
	{
		if (m_bRunning)
		{
			m_bRunning = false;

			if (m_pThreadPool)
				m_pThreadPool->Close();
		}
		return true;
	}

	Bool CApp::IsRunning() const
	{
		return m_bRunning;
	}

	Int32 CApp::GetThreadNum() const
	{
		if (m_pThreadPool)
			return m_pThreadPool->GetThreadNum();
		
		return 0;
	}

	Bool CApp::SendProtocol(SID iSid, Protocol* pProto)
	{
		return true;
	}

	Bool CApp::PostProtocol(SID iSid, Protocol* pProto)
	{
		if (m_bRunning && iSid && pProto)
		{
			XID sXid = GetXidBySid(iSid);
			if (sXid.IsValid())
			{
				Int32 iCount = GetThreadNum();
				Assert(iCount > 0);
				Int32 iIdx = sXid.Id % iCount;

				CApp_ProtoTask* pTask = new CApp_ProtoTask(sXid, iSid, pProto);
				CScope::ObjPtr scope(pTask);
				return PostAppTask(pTask, iIdx);
			}			
		}

		g_ProtocolManager->ReleaseProto(pProto);
		return true;
	}

	Bool CApp::PostProtocol(const XID& sXid, SID iSid, Protocol* pProto)
	{
		if (m_bRunning && iSid && pProto && sXid.IsValid())
		{
			Int32 iCount = GetThreadNum();
			Assert(iCount > 0);
			Int32 iIdx = sXid.Id % iCount;

			CApp_ProtoTask* pTask = new CApp_ProtoTask(sXid, iSid, pProto);
			CScope::ObjPtr scope(pTask);
			return PostAppTask(pTask, iIdx);		
		}

		g_ProtocolManager->ReleaseProto(pProto);
		return true;
	}

	Bool CApp::PostMsg(CMsg* pMsg)
	{
		if (m_bRunning && pMsg)
		{
			Int32 iCount = GetThreadNum();
			Assert(iCount > 0);
			Int32 iIdx = pMsg->Target.Id % iCount;

			CApp_MsgTask* pTask = new CApp_MsgTask(pMsg->Target, pMsg);
			CScope::ObjPtr scope(pTask);
			return PostAppTask(pTask, iIdx);
		}

		g_MsgManager->FreeMsg(pMsg);
		return false;
	}

	Bool CApp::PostMsg(const XID& sXid, CMsg* pMsg)
	{
		CMsg* pRealMsg = (CMsg*)pMsg;
		if (pRealMsg && sXid.IsValid())
		{
			pRealMsg->Target = sXid;
			return PostMsg(pRealMsg);
		}
		
		g_MsgManager->FreeMsg(pRealMsg);
		return false;
	}

	Bool CApp::PostMsg(const XIDVector& vXID, CMsg* pMsg)
	{
		if (pMsg && vXID.size())
		{
			Int32 iCount = GetThreadNum();
			Assert(iCount > 0);
			map<Int32, XIDVector> mThreadXid;
		
			for(Size_t i=0;i<vXID.size();i++)
			{
				Int32 iIdx = vXID[i].Id % iCount;
				if (mThreadXid.find(iIdx) == mThreadXid.end())
					mThreadXid[iIdx] = XIDVector();
				
				mThreadXid[iIdx].push_back(vXID[i]);
			}

			map<Int32, XIDVector>::iterator it = mThreadXid.begin();
			for (;it!=mThreadXid.end();it++)
			{
				CApp_MsgTask* pTask = new CApp_MsgTask(it->second, pMsg);
				CScope::ObjPtr scope(pTask);
				PostAppTask(pTask, it->first);
			}
		}

		g_MsgManager->FreeMsg(pMsg);
		return true;
	}

	Bool CApp::PostTick(const XIDVector& vXID)
	{
		if (vXID.size())
		{
			Int32 iCount = GetThreadNum();
			Assert(iCount > 0);
			map<Int32, XIDVector> mThreadXid;

			for(Size_t i=0;i<vXID.size();i++)
			{
				Int32 iIdx = vXID[i].Id % iCount;
				if (mThreadXid.find(iIdx) == mThreadXid.end())
					mThreadXid[iIdx] = XIDVector();

				mThreadXid[iIdx].push_back(vXID[i]);
			}

			map<Int32, XIDVector>::iterator it = mThreadXid.begin();
			for (;it!=mThreadXid.end();it++)
			{
				static map<Int32, CApp_TickTask> mTickTask;

				CApp_TickTask* pTask = 0;
				map<Int32, CApp_TickTask>::iterator tit = mTickTask.find(it->first);
				if (tit == mTickTask.end())
				{
					mTickTask.insert(std::make_pair(it->first, CApp_TickTask(it->second)));
					pTask = &mTickTask.find(it->first)->second;
				}
				else
				{
					pTask = &tit->second;
					pTask->ResetXids(it->second);
				}

				if (pTask)
				{
					pTask->AddRef();
					PostAppTask(pTask, it->first);
				}				
			}
		}

		return true;
	}

	Bool CApp::PostAppTask(CTask* pTask, Int32 iThreadIdx)
	{
		if(m_bRunning && m_pThreadPool)
		{
			if(iThreadIdx < 0)
			{
				A_Exception(m_pThreadPool->GetThreadNum() > 0);
				iThreadIdx = CRand::RandInt(0, m_pThreadPool->GetThreadNum()-1);
			}

			return m_pThreadPool->AddTask(pTask, iThreadIdx);
		}
		return false;
	}

	Bool CApp::DispatchProto(const XID& sXid, SID iSid, Protocol* pProto)
	{
		if(sXid.IsValid() && pProto)
		{
			SafeObj obj(sXid);
			if (obj.IsObjValid())
				return obj->OnProtocol(iSid, pProto);
		}	
		return false;
	}

	Bool CApp::DispatchMsg(const XID& sXid, CMsg* pMsg)
	{
		if(sXid.IsValid() && pMsg && pMsg->Msg > 0 && pMsg->Target.IsValid())
		{
			SafeObj obj(pMsg->Target);
			if (obj.IsObjValid())
				return obj->OnMessage(*pMsg);		
		}
		return false;
	}

	Bool CApp::DispatchTick(const XID& sXid)
	{
		if(sXid.IsValid())
		{
			SafeObj obj(sXid);
			if (obj.IsObjValid())
				return obj->OnTick();		
		}
		return false;
	}
}
