#ifndef _CRASHREPORT_H_
#define _CRASHREPORT_H_

#include "BaseHeader.h"

namespace Util
{
	/************************************************************************/
	/*	�������CMiniDumpģ���Զ�����dump�ļ�							*/
	/*	��ʹ�õĳ������������ڵ�cpp������CCrashReport����Ϳ���			*/
	/************************************************************************/
	class WIN32_API CCrashReport 
	{
	public:
		//����
		CCrashReport();

		//����
		~CCrashReport();

	protected:
		//�쳣����ص�
		static LPTOP_LEVEL_EXCEPTION_FILTER m_pCrashFilter;
	};	
}
#endif
