#include "CCrashReport.h"
#include "CMiniDump.h"

//ȫ��ΨһCoreDump����
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
		//ϵͳֻ���ڴ�һ����
		assert(m_pCrashFilter == 0 && "CrashReport Singleton.");

		//����δ�����쳣׼��
		m_pCrashFilter = SetUnhandledExceptionFilter(UnhandledExceptionFilter);

		OutputDebugStringA("CrashReport: ����δ֪�쳣����ģ��.\r\n");
	}

	CCrashReport::~CCrashReport()
	{
		if(m_pCrashFilter)
		{
			SetUnhandledExceptionFilter(m_pCrashFilter);
		}

		OutputDebugStringA("CrashReport: �ر�δ֪�쳣����ģ��.\r\n");
	}
}
