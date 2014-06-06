#ifndef _CRASHREPORT_H_
#define _CRASHREPORT_H_

#include "BaseHeader.h"

namespace Util
{
	/************************************************************************/
	/*	程序调用CMiniDump模块自动生成dump文件							*/
	/*	在使用的程序主函数所在的cpp中申明CCrashReport对象就可以			*/
	/************************************************************************/
	class WIN32_API CCrashReport 
	{
	public:
		//构造
		CCrashReport();

		//析构
		~CCrashReport();

	protected:
		//异常捕获回调
		static LPTOP_LEVEL_EXCEPTION_FILTER m_pCrashFilter;
	};	
}
#endif
