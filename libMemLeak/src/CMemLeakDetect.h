#ifndef C_MEMLEAKDETECTOR_H
#define C_MEMLEAKDETECTOR_H

#include "BaseHeader.h"

namespace Util
{
	/************************************************************************/
	/*	�����ڴ�й¶������												*/
	/*	��ʹ�õĳ������������ڵ�cpp������CMemLeakDetect����Ϳ���			*/
	/************************************************************************/
	class CMemLeakDetect
	{
#ifdef _DEBUG
	public:
		//����
		CMemLeakDetect();

		//����
		~CMemLeakDetect();

		//�ڴ���乳�����Ͷ���
		typedef _CRT_ALLOC_HOOK CrtHook;

	public:
		//�ڴ湹�캯������
		static Int32 AllocHook(Int32 iType, void* pData, Size_t iSize, Int32 iUse, Long iRequest, const UChar* szFile, Int32 iLine);

	protected:
		//��������·��
		Char*  BuildSymbolPath();

		//��ȡ׷��ջ
		void   GetStackTrace(void* pCallStack);	

		//����й¶��Ϣ
		void   ReportLeaks();

	protected:
		//Free����Hook����
		void   HookFree(const void* pData);

		//Malloc����Hook����
		void   HookMalloc(Int32 iRequest);

		//Realloc����Hook����
		void   HookRealloc(const void* pData, Int32 iRequest);

	protected:
		//������Ϣ
		void   Report(const Char* pFormat, ...);

	protected:
		//������Ϣ��
		void*   m_pMallocMap;		   
		//���̾��
		HANDLE	m_hProcess;
		//��־�ļ����
		FILE*	m_pLogFile;		
		//�ڴ���乳��
		CrtHook m_pOldHook; 
#endif
	};	
}
#endif
