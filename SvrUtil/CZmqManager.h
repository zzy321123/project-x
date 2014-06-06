#ifndef UTIL_ZMQMANAGER_H
#define UTIL_ZMQMANAGER_H

#include "ManagerBase.h"
#include "CZmq.h"

namespace Util
{
	/************************************************************************/
	/* ZMQ���������                                                        */
	/************************************************************************/
	class UTIL_API CZmqManager : public CManagerBase
	{
	protected:
		//���������
		CZmqManager();

		//����
		virtual ~CZmqManager();

		//��������
		SINGLETON_DECL(ZmqManager);

		//��������Ͷ���
		typedef map<CZmq*, CZmq*> ZmqMap;

	public:
		//����������
		virtual Bool	 SetupZmqCtx(Int32 iThreads = 1);

		//��ȡ������
		virtual void*	 GetZmqCtx();

		//����ZMQ
		virtual CZmq*	CreateZmq(Int32 iType);
		
		//�ر�ZMQ
		virtual Bool	 CloseZmq(CZmq*& pZmq);
		
		//����ZMQ����ͨ��,Bothway: ˫����, Timeout: ʱ�䳬ʱ(-1Ϊ���޵ȴ�), Once: ����һ��
		virtual Bool     ProxyZmq(CZmq* pFrontend, CZmq* pBackend, Bool bBothway, Int32 iTimeout, Bool bOnce);

	protected:
		//�����ľ��
		void*   m_pZmqCtx;
		//������Zmq�б�
		ZmqMap  m_mZmq;
	};

	//���׺궨��
	#define g_ZmqManager  CZmqManager::GetInstance()
}
#endif
