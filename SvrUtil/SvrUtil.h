#ifndef _UTIL_H_
#define _UTIL_H_

#if !defined(UTIL_EXPORT)
#include "Atomic.h"
#include "ByteOrder.h"
#include "Exception.h"
#include "IPAddr.h"
#include "Logger.h"
#include "LoggerManager.h"
#include "UtilMalloc.h"
#include "ManagerBase.h"
#include "Marshal.h"
#include "MarshalData.h"
#include "CMath.h"
#include "Mutex.h"
#include "Octets.h"
#include "OctetsStream.h"
#include "OSOperator.h"
#include "Protocol.h"
#include "ProtocolManager.h"
#include "CRand.h"
#include "RefCounter.h"
#include "CScope.h"
#include "Security.h"
#include "Session.h"
#include "Socket.h"
#include "SocketAddr.h"
#include "SocketPair.h"
#include "SpinLock.h"
#include "BaseHeader.h"
#include "StringUtil.h"
#include "SysProtocol.h"
#include "Task.h"
#include "Thread.h"
#include "ThreadPool.h"
#include "TimerManager.h"
#include "CDatabase.h"
#include "CDBManager.h"
#include "CMysql.h"
#include "CZmq.h"
#include "CZmqManager.h"
#include "UtilZlib.h"
#include "CFile.h"
#include "CDiskFile.h"
#include "CXmlAttribute.h"
#include "CXmlDocument.h"
#include "CXmlElement.h"
#include "CXmlFile.h"
#include "CJson.h"
#include "CLockMap.h"
#include "CApp.h"
#include "CAppObj.h"
#include "CCounter.h"
#include "XID.h"
using namespace Util;
#else
#include "BaseHeader.h"
#endif

namespace Util
{
	/************************************************************************/
	/* Ӧ�õײ��ʼ��,����,ֹͣ,�ͷŵ�ͳһ�ӿڷ�װ								*/
	/************************************************************************/
	class UTIL_API CSvrUtil
	{
	public:
		//��ʼ��
		static void Init();

		//���ڸ���
		static void Tick(UInt32 iPeriod);

		//ֹͣ����
		static void Stop();

		//�ͷ���Դ
		static void Release();
	};
};
#endif //_UTIL_H_
