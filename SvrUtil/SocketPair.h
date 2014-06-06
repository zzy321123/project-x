#ifndef _SOCKETPAIR_H_
#define _SOCKETPAIR_H_

#include "Socket.h"
#include "Mutex.h"

namespace Util
{
	/************************************************************************/
	/* Socket对操作封装,主要用来线程间信息通知									*/
	/************************************************************************/
	class UTIL_API CSocketPair : public CRefCounter
	{		
	public:		
		//构造
		CSocketPair();

		//析构
		virtual ~CSocketPair();

	public:
		//创建套接字对
		virtual Bool	Create(Bool bThreadSafe = false);

		//是否有效
		virtual Bool    IsValid() const;

		//发送消息
		virtual Bool	Send(const void* pData, Size_t& iSize, Int32 iFlags = 0);

		//接收消息
		virtual Bool	Receive(void* pData, Size_t& iSize, Int32 iFlags = 0);

		//清理未读事件
		virtual Bool    Clear();

		//关闭
		virtual Bool    Close();

	public:
		//获取写FD
		virtual SOCKET	GetWriteFd() const;

		//获取读FD
		virtual SOCKET  GetReadFd() const;

	protected:
		//Socket句柄
		SOCKET		m_hSocket[2];
		//发送端的线程安全锁
		CMutex*		m_pMutex;
		//发送量
		UInt64		m_iSend;
		//接收量
		UInt64		m_iRecv;
	};	

	////协议类型简便定义
	typedef CSocketPair SocketPair;
}
#endif
