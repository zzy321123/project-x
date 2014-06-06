#ifndef UTIL_MALLOC_H
#define UTIL_MALLOC_H

#include "BaseHeader.h"

namespace Util
{
	/************************************************************************/
	/* Malloc��ز�����װ													*/
	/************************************************************************/
	//�ڴ����
	void* UTIL_API UtilMalloc(Size_t iSize);

	//�ڴ����
	void* UTIL_API UtilCalloc(Size_t iNum, Size_t iSize);

	//�ڴ��ط���
	void* UTIL_API UtilRealloc(void* pMem, Size_t iSize);

	//�ڴ��ͷ�
	void UTIL_API  UtilFree(void* pMem);
}

//////////////////////////////////////////////////////////////////////////
#ifdef _MALLOC
	using namespace Util;

	//�ڴ����ʧ�ܲ����׳��쳣���Ƿ��ؿ�ָ��
	void* operator	new(Size_t iSize, const std::nothrow_t&) throw();

	//��ͨ�汾new
	void* operator  new(Size_t iSize) throw(std::bad_alloc);
	void* operator  new[](Size_t iSize) throw(std::bad_alloc);
	void  operator  delete(void* pMem) throw();
	void  operator  delete[](void* pMem) throw();	

	//��������Ϣ�汾
	void* operator  new(Size_t iSize, const Char* szFile, Int32 iLine);
	void* operator  new[](Size_t iSize, const Char* szFile, Int32 iLine);
	void  operator  delete(void* pMem, const Char* szFile, Int32 iLine);
	void  operator  delete[](void* pMem, const Char* szFile, Int32 iLine);

#endif 
//////////////////////////////////////////////////////////////////////////

#endif //UTIL_MALLOC_H
