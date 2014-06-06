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
		//����
		CDumpWin32();

		//����
		~CDumpWin32();

	public:
		//װ��Win32ģ��
		static Bool Install();

		//ж��Win32 ģ��
		static Bool Uninstall();

	public:
		//����Coredump�����߳�
		static Bool LaunchCoredump(UChar uKey = 'd');
	};

	//windows�쳣�������������
	extern Bool UTIL_API DumpHandleException(struct _EXCEPTION_POINTERS *pExceptionInfo);
}
#endif
