#ifndef C_APP_H
#define C_APP_H

#include "CAppObj.h"
#include "CObjManager.h"
#include "CMsgManager.h"
#include "ThreadPool.h"

namespace Util
{
	/************************************************************************/
	/* Ӧ�ò��װ                                                           */
	/************************************************************************/
	class UTIL_API CApp : public CAppObj
	{
	public:
		//����
		CApp(const XID& sXid = XID());

		//����
		virtual ~CApp();

		//�ڲ��������
		friend  class  CApp_ProtoTask;

		friend  class  CApp_MsgTask;

		friend  class  CApp_TickTask;

	public:
		//�������Լ��������������
		typedef CObjBase<XID, CAppObj>		ObjBase;

		//���������Ͷ���
		typedef CObjManager<XID, CAppObj>	ObjMan;

		//������ӳ���
		typedef map<UInt32, ObjMan*>		ObjManMap;

		//�ỰID�Ͷ���ID��ӳ���
		typedef map<SID, XID>				SidXidMap;

		//��ȫ������
		class SafeObj : public ObjMan::SafeObj
		{
		public:
			//����
			SafeObj(XID sXid = XID());
			//����
			virtual ~SafeObj();
		};

	public:
		//��ʼ��Ӧ��(�����߳�����)
		virtual Bool		Init(Int32 iThread = 1);
		//��������
		virtual Bool		Run();
		//�رշ���
		virtual Bool		Stop();

	public:
		//�Ƿ�����״̬
		virtual Bool		IsRunning() const;

		//�̳߳��߳���Ŀ
		virtual Int32		GetThreadNum() const;	

		//��������(Ӧ�ò�ʵ��)
		virtual Bool		SendProtocol(SID iSid, Protocol* pProto);

	public:
		//ע�ᴴ�����������
		virtual ObjMan*		CreateObjMan(UInt32 iType);

		//��ȡ���������
		virtual ObjMan*		GetObjMan(UInt32 iType);

		//��������
		virtual CAppObj*	CreateObj(const XID& sXid, SID iSid = 0);

		//���ٶ���
		virtual Bool		DeleteObj(const XID& sXid);

	public:
		//���յ�Э���Ͷ�ݵ�Ӧ��, δ�󶨵ĻỰID�����Ҳ��������Ͷ��ʧ��
		virtual Bool		PostProtocol(SID iSid, Protocol* pProto);

		//���յ�Э���Ͷ�ݵ�Ӧ��, ����ID��Ҫ��Ч����Ͷ�ݳɹ�
		virtual Bool		PostProtocol(const XID& sXid, SID iSid, Protocol* pProto);

		//ֱ��Ͷ����Ϣ
		virtual Bool		PostMsg(CMsg* pMsg);

		//���ض�����Ͷ����Ϣ
		virtual Bool		PostMsg(const XID& sXid, CMsg* pMsg);	

		//�㲥��Ϣ
		virtual Bool		PostMsg(const XIDVector& vXID, CMsg* pMsg);

		//�ύ����
		virtual Bool		PostTick(const XIDVector& vXID);

		//Ͷ��Ӧ������
		virtual Bool		PostAppTask(CTask* pTask, Int32 iThreadIdx = -1);
	
	public:
		//���ݻ�ȥ����ID
		virtual XID			GetXidBySid(SID iSid);

		//�󶨻ỰID�Ͷ���ID
		virtual void		BindSidXid(SID iSid, const XID& sXid);

		//��󶨻ỰID�Ͷ���ID
		virtual void		UnbindSidXid(SID iSid);

	protected:
		//�ַ�Э��
		virtual Bool		DispatchProto(const XID& sXid, SID iSid, Protocol* pProto);

		//�ַ���Ϣ
		virtual Bool		DispatchMsg(const XID& sXid, CMsg* pMsg);

		//��������
		virtual Bool		DispatchTick(const XID& sXid);

		//Ӧ�ò㴴������
		virtual CAppObj*	AppCreateObj(const XID& sXid);

	protected:
		//�Ƿ�����״̬
		volatile Bool	m_bRunning;
		//���߳�ID
		UInt32			m_iMainTid;
		//���������
		ObjManMap		m_mObjMan;
		//�Ựӳ���
		SidXidMap		m_mSidXid;
		//�Ự����
		CSpinLock*		m_pSidXidLock;
		//�̳߳�
		CThreadPool*	m_pThreadPool;
	};
}
#endif
