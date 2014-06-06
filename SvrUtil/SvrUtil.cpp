#include "SvrUtil.h"
#include "OSOperator.h"
#include "LoggerManager.h"
#include "CDBManager.h"
#include "ProtocolManager.h"
#include "TimerManager.h"
//#include "MsgManager.h"
#include "CZmqManager.h"
#include "pthread.h"

#ifdef PLATFORM_WINDOWS
#include "CDumpWin32.h"
#ifdef _DEBUG
#pragma comment(lib, "libDumpWin32_D.lib")
#else
#pragma comment(lib, "libDumpWin32.lib")
#endif
#endif

namespace Util
{
	void CheckTypeSize()
	{
		Print("==============================================================");
		FmtPrint("TypeSize:");
		FmtPrint("    Bool:       %u", sizeof(Bool));
		FmtPrint("    Char:       %u", sizeof(Char));
		FmtPrint("    UChar:      %u", sizeof(UChar));
		FmtPrint("    Utf8:       %u", sizeof(Utf8));
		FmtPrint("    Short:      %u", sizeof(Short));
		FmtPrint("    UShort:     %u", sizeof(UShort));
		FmtPrint("    WChar:      %u", sizeof(WChar));
		FmtPrint("    Int8:       %u", sizeof(Int8));
		FmtPrint("    UInt8:      %u", sizeof(UInt8));
		FmtPrint("    Int16:      %u", sizeof(Int16));
		FmtPrint("    UInt16:     %u", sizeof(UInt16));
		FmtPrint("    Int32:      %u", sizeof(Int32));
		FmtPrint("    UInt32:     %u", sizeof(UInt32));
		FmtPrint("    Int64:      %u", sizeof(Int64));
		FmtPrint("    UInt64:     %u", sizeof(UInt64));
		FmtPrint("    Float:      %u", sizeof(Float));
		FmtPrint("    Double:     %u", sizeof(Double));
		FmtPrint("    Size_t:     %u", sizeof(Size_t));
		FmtPrint("    Long:       %u", sizeof(Long));
		FmtPrint("    ULong:      %u", sizeof(ULong));
		FmtPrint("    LongLong:   %u", sizeof(LongLong));
		FmtPrint("    ULongLong:  %u", sizeof(ULongLong));
		FmtPrint("    PVoid:      %u", sizeof(PVoid));
		FmtPrint("    SID:        %u", sizeof(SID));
		FmtPrint("    ProtoType:  %u", sizeof(ProtoType));
		FmtPrint("    ProtoSize:  %u", sizeof(ProtoSize));
		FmtPrint("    ProtoCrc:   %u", sizeof(ProtoCrc));
		Print("==============================================================");
	}

	void CSvrUtil::Init()
	{
#ifdef PLATFORM_WINDOWS
		CDumpWin32::Install();

		//使用pthread的线程和进程装载		
		pthread_win32_process_attach_np();
		pthread_win32_thread_attach_np();

		//网络初始化
		WSADATA sData;
		WSAStartup(MAKEWORD(2, 2),&sData);		

		//设置CRT调试模式
		COSOperator::SetCrtDebugMode();
#else
		struct sigaction sAction;  
		sAction.sa_handler = SIG_IGN;  
		sigaction(SIGPIPE, &sAction, 0); 
#endif	

		//初始化管理器
		CLoggerManager::InitInstance();
		CTimerManager::InitInstance();
		CProtocolManager::InitInstance();		
		CDBManager::InitInstance();
		CZmqManager::InitInstance();

		//建立日志目录
		g_LoggerManager->SetupLogs("Logs/");
		//注册系统协议
		g_ProtocolManager->RegSysProtocol();		

		//运行管理器
		CLoggerManager::GetInstance()->Start();
		CTimerManager::GetInstance()->Start();
		CProtocolManager::GetInstance()->Start();
		CDBManager::GetInstance()->Start();
		CZmqManager::GetInstance()->Start();

#ifdef _DEBUG
		CheckTypeSize();
#endif
	}

	void CSvrUtil::Tick(UInt32 iPeriod)
	{
		//更新管理器
		CZmqManager::GetInstance()->Tick(iPeriod);
		CDBManager::GetInstance()->Tick(iPeriod);
		CProtocolManager::GetInstance()->Tick(iPeriod);
		CTimerManager::GetInstance()->Tick(iPeriod);
		CLoggerManager::GetInstance()->Tick(iPeriod);
	}

	void CSvrUtil::Stop()
	{
		//停止管理器
		CZmqManager::GetInstance()->Stop();
		CDBManager::GetInstance()->Stop();
		CProtocolManager::GetInstance()->Stop();	
		CTimerManager::GetInstance()->Stop();
		CLoggerManager::GetInstance()->Stop();
	}

	void CSvrUtil::Release()
	{
		//释放管理器
		CZmqManager::ExitInstance();
		CDBManager::ExitInstance();
		CProtocolManager::ExitInstance();	
		CTimerManager::ExitInstance();
		CLoggerManager::ExitInstance();
		
#ifdef PLATFORM_WINDOWS
		//网络清理
		WSACleanup();

		//使用pthread的线程和进程卸载
		pthread_win32_thread_detach_np();
		pthread_win32_process_detach_np();

		CDumpWin32::Uninstall();
#endif
	}
}
