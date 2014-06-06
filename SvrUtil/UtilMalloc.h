#ifndef UTIL_MALLOC_H
#define UTIL_MALLOC_H

#include "BaseHeader.h"

namespace Util
{
	/************************************************************************/
	/* Malloc相关操作封装													*/
	/************************************************************************/
	//内存分配
	void* UTIL_API UtilMalloc(Size_t iSize);

	//内存分配
	void* UTIL_API UtilCalloc(Size_t iNum, Size_t iSize);

	//内存重分配
	void* UTIL_API UtilRealloc(void* pMem, Size_t iSize);

	//内存释放
	void UTIL_API  UtilFree(void* pMem);
}

//////////////////////////////////////////////////////////////////////////
#ifdef _MALLOC
	using namespace Util;

	//内存分配失败不会抛出异常而是返回空指针
	void* operator	new(Size_t iSize, const std::nothrow_t&) throw();

	//普通版本new
	void* operator  new(Size_t iSize) throw(std::bad_alloc);
	void* operator  new[](Size_t iSize) throw(std::bad_alloc);
	void  operator  delete(void* pMem) throw();
	void  operator  delete[](void* pMem) throw();	

	//带调试信息版本
	void* operator  new(Size_t iSize, const Char* szFile, Int32 iLine);
	void* operator  new[](Size_t iSize, const Char* szFile, Int32 iLine);
	void  operator  delete(void* pMem, const Char* szFile, Int32 iLine);
	void  operator  delete[](void* pMem, const Char* szFile, Int32 iLine);

#endif 
//////////////////////////////////////////////////////////////////////////

#endif //UTIL_MALLOC_H
