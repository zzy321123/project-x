#include "CMsgManager.h"
#include "LoggerManager.h"
#include "OSOperator.h"

namespace Util
{
	SINGLETON_IMPL(MsgManager);

	CMsgManager::CMsgManager()
	{
	}

	CMsgManager::~CMsgManager()
	{		
		m_mReg.clear();
	}

	Bool CMsgManager::Register(Int32 iMsg)
	{
		MsgRegMap::iterator it = m_mReg.find(iMsg);
		Assert(it == m_mReg.end());
		if (it == m_mReg.end())
		{
			m_mReg[iMsg] = iMsg;
			return true;
		}
		return false;
	}

	UInt32 CMsgManager::GetRegMsgIds(vector<Int32>& vMsgIds)
	{
		vMsgIds.clear();

		MsgRegMap::iterator it = m_mReg.begin();
		for (;it!=m_mReg.end();it++)
		{
			vMsgIds.push_back(it->first);
		}

		return (UInt32)vMsgIds.size();
	}

	CMsg* CMsgManager::GetMsg(Int32 iMsg)
	{
		CMsg* pMsg = new CMsg(iMsg);

		pMsg->SysFlag = CMsg::SYS_CREATE;
		pMsg->Time    = COSOperator::GetTickCount();

		return pMsg;
	}

	Bool CMsgManager::FreeMsg(CMsg*& pMsg)
	{
		if (pMsg)
		{
			if (pMsg->SysFlag & CMsg::SYS_CREATE)
			{
				DEF_RELEASE(pMsg);
			}			
			
			return true;
		}
		return false;
	}
}
