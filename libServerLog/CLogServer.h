#ifndef C_LOGSERVER_H
#define C_LOGSERVER_H

#include "SvrUtil.h"

namespace Util
{
	/************************************************************************/
	/* 日志服务器封装, 数据库模式下默认写logs表                             */
	/************************************************************************/
	class UTIL_API CLogServer : public CRefCounter
	{
	public:
		//构造
		CLogServer();

		//析构
		virtual ~CLogServer();

	public:
		//初始化文件日志
		virtual Bool Init(const AString& sSvrAddr, const AString& sLogFile, Int32 iCacheSize = MEGABYTE, Int32 iCacheTime = 60000);

		//初始化数据库日志
		virtual Bool Init(const AString& sSvrAddr, const CDBConn& sConn, Int32 iCacheSize = MEGABYTE, Int32 iCacheTime = 60000);

		//启动日志服务器
		virtual Bool Run();

		//关闭日志服务器
		virtual Bool Stop();

		//设置日志打印标记
		virtual Bool EnableConsole(Bool bEnable);

	protected:
		//缓存日志
		virtual Bool CacheLogs(Int32 iSvrId, UInt32 iUsrId, const UString& sUsrName, Int32	iType, const UString& sKey, const UString& sMsg);

		//日志落地
		virtual Bool FlushLogs();

	protected:
		//运行状态
		volatile Bool m_bRunning;
		//打印控制
		Bool		  m_bConsole;
		//UDP模式日志接收
		CSocket		  m_sSocket;					
		//UDP数据接收Buffer
		OctetsStream* m_pRecvBuf;
		//日志缓存Buffer
		COctets*	  m_pLogCache;
		//日志格式化Buffer
		Utf8*		  m_pFmtBuf;
		//日志存储数据库
		CDatabase*	  m_pLogDB;
		//日志存储文件
		CDiskFile*	  m_pLogFile;
		//缓存刷新事件
		Int32		  m_iCacheTime;
	};
}
#endif
