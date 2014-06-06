#include "RefCounter.h"
#include "Atomic.h"

namespace Util
{
	CRefCounter::CRefCounter() : m_iRef(1)
	{
	}

	CRefCounter::~CRefCounter()
	{
	}

	Int32 CRefCounter::AddRef()
	{
		return CAtomic::Inc<Long>(&m_iRef);
	}

	Int32 CRefCounter::DecRef()
	{ 
		return CAtomic::Dec<Long>(&m_iRef);
	}

	Int32 CRefCounter::GetRef() const
	{ 
		return m_iRef;
	}

	void CRefCounter::Release()
	{
		//判断引用计数有效,避免无效对象释放
		A_Exception(m_iRef > 0 && "Reference count Is Error.");

		Long iRef = DecRef();

		//删除对象
		if ( iRef == 0 )
		{
			delete this;
		}
	}

	void* CRefCounter::operator new(Size_t iSize)
	{
		return UtilMalloc(iSize);
	}

	void CRefCounter::operator delete(void* pData, Size_t iSize)
	{
		UtilFree(pData);
	}
}

