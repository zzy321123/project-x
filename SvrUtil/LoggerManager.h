#ifndef _LOGGERMANAGER_H
#define _LOGGERMANAGER_H

#include "ManagerBase.h"
#include "Logger.h"

namespace Util
{
	/************************************************************************/
	/* 日志管理器封装														*/
	/************************************************************************/
	class UTIL_API CLoggerManager : public CManagerBase
	{
	protected:
		//日志管理器构造
		CLoggerManager();

		//日志管理器析构
		virtual ~CLoggerManager();

		//单例类声明
	protected:
		static CLoggerManager* m_pLoggerManager;
	public:											
		static Bool InitInstance();			
		static CLoggerManager* GetInstance();					
		static Bool ExitInstance();
	
	public:
		//日志表类型定义
		typedef map<AString, CLogger*> LoggerMap;	

	public:
		//创建一个日志记录器
		CLogger*   CreateLogger(const AString& sFile, const AString& sName = "", Bool bAppend = false);	

		//获取一个日志记录器
		CLogger*	GetLogger(const AString& sName);

	public:
		//设置日志文件路径(以'/'结尾)
		virtual Bool  SetupLogs(const AString& sDir = "Logs/");

		//Print信息
		virtual void  Print(const AString& sMsg);

		//设置控制台打印输出
		virtual void  EnableConsole(Bool bEnable);

		//设置打印信息记入日志
		virtual void  EnablePrintLog(Bool bEnable = true);

	protected:
		//日志文件路径
		AString		m_sLogDir;
		//日志映射表
		LoggerMap	m_mLoggerMap;
		//是否允许打印输出
		Bool		m_bConsole;
		//控制台输出日志
		CLogger*	m_pPrintLog;
	};

	//控制台打印信息
	void  UTIL_API Print(const AString& sMsg, Bool bNewLine = true);
	void  UTIL_API FmtPrint(const Char* sFmt, ...);
	void  UTIL_API FmtPrintLine(const Char* sFmt, ...);

	//系统默认日志记录
	void  UTIL_API Log(const AString& sMsg, CLogger::LogLevel eLevel = CLogger::LEVEL_INFO, const AString& sName = "Util");
	void  UTIL_API FmtLog(const Char* sFmt,  ...);

	//系统默认错误记录
	void  UTIL_API Error(const AString& sMsg, const AString& sName = "Error");
	void  UTIL_API FmtError(const Char* sFmt, ...);
	
	//日志全局管理器对象
	#define g_LoggerManager CLoggerManager::GetInstance()
}
#endif //MY_LOGGERMANAGER_H
