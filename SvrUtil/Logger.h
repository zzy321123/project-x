#ifndef _LOGGER_H
#define _LOGGER_H

#include "Mutex.h"

namespace Util
{
	/************************************************************************/
	/* 线程安全日志类,处理所有日志记录											*/
	/************************************************************************/
	class UTIL_API CLogger : public CRefCounter
	{
	protected:
		//构造
		CLogger(const AString& sFile, Bool bAppend = false);

		//析构
		virtual ~CLogger();
	
		//管理器友员访问
		friend class CLoggerManager;

	public:
		//日志等级
		enum LogLevel
		{
			//信息
			LEVEL_INFO,
			//警告
			LEVEL_WARNING,
			//错误
			LEVEL_ERROR
		};
		
	public:
		//记录信息
		virtual void LogMsg(const AString& sMsg, LogLevel eLevel = LEVEL_INFO);

		//记录格式化信息
		virtual void LogFmtMsg(const Char* pFmt, ...);

		//记录格式化错误
		virtual void LogFmtErr(const Char* pFmt, ...);

	protected:
		//记录时间信息
		virtual AString GetLogTimeInfo(LogLevel eLevel) const;

		//获取日志级别的描述
		virtual AString GetLogLevelDesc(LogLevel eLevel) const;

	private:
		//日志所对应的文件句柄
		FILE* m_pFile;
		//日志文件名
		AString m_sFileName;
		//是否为追加模式
		Bool m_bAppend;
		//日志线程安全互斥
		CMutex* m_pMutex;
	};
}
#endif //_LOGGER_H
