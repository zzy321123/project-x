#include "CCrashReport.h"
#include "CMiniDump.h"

//全局唯一CoreDump对象
extern Util::CCrashReport g_CrashReport;

namespace Util
{
	LPTOP_LEVEL_EXCEPTION_FILTER CCrashReport::m_pCrashFilter = 0;

	Bool DumpHandleException(struct _EXCEPTION_POINTERS *pExceptionInfo)
	{
		if(!CMiniDump::CreateMiniDump(pExceptionInfo))
			return false;

		if(!CMiniDump::BackupDumpFiles())
			return false;

		CMiniDump::LaunchBugReport();

		return true;
	}

	LONG WINAPI UnhandledExceptionFilter(struct _EXCEPTION_POINTERS *pExceptionInfo)
	{
		if( IsDebuggerPresent() )
			return EXCEPTION_CONTINUE_SEARCH;		

		DumpHandleException(pExceptionInfo);
	
		return EXCEPTION_EXECUTE_HANDLER;
	}

	//////////////////////////////////////////////////////////////////////////

	CCrashReport::CCrashReport()
	{
		//系统只存在此一对象
		assert(m_pCrashFilter == 0 && "CrashReport Singleton.");

		//设置未处理异常准备
		m_pCrashFilter = SetUnhandledExceptionFilter(UnhandledExceptionFilter);

		OutputDebugStringA("CrashReport: 开启未知异常捕获模块.\r\n");
	}

	CCrashReport::~CCrashReport()
	{
		if(m_pCrashFilter)
		{
			SetUnhandledExceptionFilter(m_pCrashFilter);
		}

		OutputDebugStringA("CrashReport: 关闭未知异常捕获模块.\r\n");
	}
}
