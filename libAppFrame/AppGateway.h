#ifndef APPGATEWAY_H
#define APPGATEWAY_H

#include "BaseGateway.h"

namespace Util
{
	class CAppFrame;
	/************************************************************************/
	/* �����ʹ����Ƕ���ط�����                                             */
	/************************************************************************/	
	class FRAME_API CAppGateway : public BaseGateway
	{
	public:
		//����
		CAppGateway(CAppFrame* pApp = 0);

		//����
		virtual ~CAppGateway();

		//֧������ƽ̨�Ķ���
		enum
		{
			GATE_PF_NONE = 0,
			GATE_PF_QQ,
		};

	public:
		//��ʼ��
		virtual Bool    Init();

		//ֹͣ
		virtual Bool	Stop();

		//�Ự����
		virtual Bool	TrustSession(void* pSession, OctetsStream* pIBuffer);

	protected:
		//App����
		CAppFrame*		m_pApp;
		//���ط������߳�
		CThread*		m_pThread;
		//����ƽ̨
		Int32			m_iPlatform;
	};
}
#endif
