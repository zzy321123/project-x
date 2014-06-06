#include "UtilMalloc.h"

#if defined(_DEBUG) && defined(PLATFORM_WINDOWS)
#define NED_MALLOC  0
#else
#define NED_MALLOC  1
#endif

#if NED_MALLOC
#include "nedmalloc.c"
#endif

namespace Util
{
	void* UtilMalloc(Size_t iSize)
	{
		if (iSize)
		{
#if NED_MALLOC
			return nedalloc::nedmalloc(iSize);		
#else
			return malloc(iSize);
#endif
		}

		return 0;
	}

	void* UtilCalloc(Size_t iNum, Size_t iSize)
	{
		if (iSize)
		{
#if NED_MALLOC
			return nedalloc::nedcalloc(iNum, iSize);		
#else
			return calloc(iNum, iSize);
#endif
		}

		return 0;
	}

	void* UtilRealloc(void* pMem, Size_t iSize)
	{
		if (!pMem)
			return UtilMalloc(iSize);

		if (!iSize)
		{
			UtilFree(pMem);
			return 0;
		}

#if NED_MALLOC
		return nedalloc::nedrealloc(pMem, iSize);		
#else
		return realloc(pMem, iSize);
#endif
	}

	void UtilFree(void* pMem)
	{
		if (pMem)
		{
#if NED_MALLOC
			nedalloc::nedfree(pMem);		
#else
			free(pMem);
#endif
		}
	}
}

//////////////////////////////////////////////////////////////////////////
using namespace Util;

void* operator	new(Size_t iSize, const std::nothrow_t&) throw()
{
	return UtilMalloc(iSize);
}

void* operator new(Size_t iSize) throw(std::bad_alloc)
{
	return UtilMalloc(iSize);
}

void* operator new[](Size_t iSize) throw(std::bad_alloc)
{
	return UtilMalloc(iSize);
}

void  operator delete(void* pMem) throw()
{
	return UtilFree(pMem);
}

void  operator delete[](void* pMem) throw()
{
	return UtilFree(pMem);
}

void* operator new(Size_t iSize, const Char* szFile, Int32 iLine)
{
	return UtilMalloc(iSize);
}

void* operator new[](Size_t iSize, const Char* szFile, Int32 iLine)
{
	return UtilMalloc(iSize);
}

void  operator delete(void* pMem, const Char* szFile, Int32 iLine)
{
	return UtilFree(pMem);
}

void  operator delete[](void* pMem, const Char* szFile, Int32 iLine)
{
	return UtilFree(pMem);
}
