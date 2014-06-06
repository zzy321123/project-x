#ifndef UTIL_ZMQMANAGER_H
#define UTIL_ZMQMANAGER_H

#include "ManagerBase.h"
#include "CZmq.h"

namespace Util
{
	/************************************************************************/
	/* ZMQ对象管理器                                                        */
	/************************************************************************/
	class UTIL_API CZmqManager : public CManagerBase
	{
	protected:
		//构造和析构
		CZmqManager();

		//析构
		virtual ~CZmqManager();

		//单例申明
		SINGLETON_DECL(ZmqManager);

		//对象表类型定义
		typedef map<CZmq*, CZmq*> ZmqMap;

	public:
		//创建上下文
		virtual Bool	 SetupZmqCtx(Int32 iThreads = 1);

		//获取上下文
		virtual void*	 GetZmqCtx();

		//创建ZMQ
		virtual CZmq*	CreateZmq(Int32 iType);
		
		//关闭ZMQ
		virtual Bool	 CloseZmq(CZmq*& pZmq);
		
		//开启ZMQ数据通道,Bothway: 双向传输, Timeout: 时间超时(-1为无限等待), Once: 传输一次
		virtual Bool     ProxyZmq(CZmq* pFrontend, CZmq* pBackend, Bool bBothway, Int32 iTimeout, Bool bOnce);

	protected:
		//上下文句柄
		void*   m_pZmqCtx;
		//创建的Zmq列表
		ZmqMap  m_mZmq;
	};

	//简易宏定义
	#define g_ZmqManager  CZmqManager::GetInstance()
}
#endif
