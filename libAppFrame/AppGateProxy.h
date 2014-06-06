#ifndef C_APPGATEPROXY_H
#define C_APPGATEPROXY_H

#include "BaseGateProxy.h"

namespace Util
{
	class CAppFrame;
	/************************************************************************/
	/* �����ʹ�����ش�����                                                 */
	/************************************************************************/
	class FRAME_API CAppGateProxy : public BaseGateProxy
	{
	public:
		//����
		CAppGateProxy(CAppFrame* pApp = 0);

		//����
		virtual ~CAppGateProxy();

	public:
		//��ʼ��
		virtual Bool  Init();

	public:
		//����������
		virtual Bool  OnProxyConnected();

		//�������Ͽ�
		virtual Bool  OnProxyDisconnect();

		//���յ�Э��
		virtual Bool  OnRecvProtocol(const GateMsgHeader& sHeader, CProtocol* pProto);

	public:
		//�Ƿ�����״̬
		virtual Bool  IsConnect() const;

	protected:
		//App����
		CAppFrame*		m_pApp;
		//����״̬
		Bool			m_bConnect;
	};
}
#endif
