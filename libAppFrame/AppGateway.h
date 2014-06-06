#ifndef APPGATEWAY_H
#define APPGATEWAY_H

#include "BaseGateway.h"

namespace Util
{
	class CAppFrame;
	/************************************************************************/
	/* 框架类使用内嵌网关服务器                                             */
	/************************************************************************/	
	class FRAME_API CAppGateway : public BaseGateway
	{
	public:
		//构造
		CAppGateway(CAppFrame* pApp = 0);

		//析构
		virtual ~CAppGateway();

		//支持网关平台的定义
		enum
		{
			GATE_PF_NONE = 0,
			GATE_PF_QQ,
		};

	public:
		//初始化
		virtual Bool    Init();

		//停止
		virtual Bool	Stop();

		//会话信任
		virtual Bool	TrustSession(void* pSession, OctetsStream* pIBuffer);

	protected:
		//App对象
		CAppFrame*		m_pApp;
		//网关服务器线程
		CThread*		m_pThread;
		//所属平台
		Int32			m_iPlatform;
	};
}
#endif
