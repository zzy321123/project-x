#ifndef _DUMP_WIN32_H_
#define _DUMP_WIN32_H_

#ifndef WIN32_EXPORT
#include "CCrashReport.h"
#include "CDetour.h"
#include "CMemLeakDetect.h"
#include "CMiniDump.h"
#else
#include "BaseHeader.h"
#endif

namespace Util
{
	class WIN32_API CDumpWin32
	{
	public:
		//构造
		CDumpWin32();

		//析构
		~CDumpWin32();

	public:
		//装载Win32模块
		static Bool Install();

		//卸载Win32 模块
		static Bool Uninstall();

	public:
		//开启Coredump生成线程
		static Bool LaunchCoredump(UChar uKey = 'd');
	};

	//windows异常处理函数句柄申明
	extern Bool UTIL_API DumpHandleException(struct _EXCEPTION_POINTERS *pExceptionInfo);
}
#endif
