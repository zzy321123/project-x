#ifndef C_MSGMANAGER_H
#define C_MSGMANAGER_H

#include "CMsg.h"
#include "ManagerBase.h"

namespace Util
{
	/************************************************************************/
	/* ��Ϣ������,�ڴ������ͷ���Ϣ֮ǰ,����ע�����õ���Ϣ����               */
	/************************************************************************/
	class UTIL_API CMsgManager : public CManagerBase
	{
	protected:
		//����
		CMsgManager();

		//����
		virtual ~CMsgManager();

		//��������������
		SINGLETON_DECL(MsgManager);

		//ע���б�
		typedef map<Int32, Int32> MsgRegMap;

	public:
		//ע����Ϣ����
		virtual Bool	 Register(Int32 iMsg);	

		//��ȡ��Ϣ
		virtual CMsg*	GetMsg(Int32 iMsg);

		//�ͷ���Ϣ
		virtual Bool     FreeMsg(CMsg*& pMsg);

		//��ȡע��Э��ID�б�
		virtual UInt32	 GetRegMsgIds(vector<Int32>& vMsgIds);

	protected:		
		//ע�������б�
		MsgRegMap m_mReg;
	};

	#define g_MsgManager  CMsgManager::GetInstance()
}
#endif
