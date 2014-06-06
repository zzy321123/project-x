#ifndef C_APPGATEPROXY_H
#define C_APPGATEPROXY_H

#include "BaseGateProxy.h"

namespace Util
{
	class CAppFrame;
	/************************************************************************/
	/* 框架类使用网关代理器                                                 */
	/************************************************************************/
	class FRAME_API CAppGateProxy : public BaseGateProxy
	{
	public:
		//构造
		CAppGateProxy(CAppFrame* pApp = 0);

		//析构
		virtual ~CAppGateProxy();

	public:
		//初始化
		virtual Bool  Init();

	public:
		//代理器连接
		virtual Bool  OnProxyConnected();

		//代理器断开
		virtual Bool  OnProxyDisconnect();

		//接收到协议
		virtual Bool  OnRecvProtocol(const GateMsgHeader& sHeader, CProtocol* pProto);

	public:
		//是否连接状态
		virtual Bool  IsConnect() const;

	protected:
		//App对象
		CAppFrame*		m_pApp;
		//连接状态
		Bool			m_bConnect;
	};
}
#endif
