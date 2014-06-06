#include "Octets.h"

namespace Util
{
	COctets::COctets() : m_pBase(0),m_pHigh(0),m_iCap(0)
	{
	}	

	COctets::COctets(UInt32 iSize) : m_pBase(0),m_pHigh(0),m_iCap(0)
	{
		Reserve(iSize);
	}

	COctets::COctets(const void* pData, UInt32 iSize) : m_pBase(0),m_pHigh(0),m_iCap(0)
	{
		Replace(pData, iSize);
	}

	COctets::COctets(void* pBegin, void* pEnd) : m_pBase(0),m_pHigh(0),m_iCap(0)
	{
		Replace( pBegin,(UInt32)((Char*)pBegin-(Char*)pEnd) );
	}

	COctets::COctets(const COctets& xOctets) : m_pBase(0),m_pHigh(0),m_iCap(0)
	{
		if(xOctets.Size())
		{
			Replace(xOctets.Begin(), xOctets.Size());
		}
	}

	COctets::~COctets()
	{
		if(m_pBase)
		{
			UtilFree(m_pBase);
			m_pBase = 0;
		}
		m_pHigh = 0;
		m_iCap  = 0;
	}

	COctets& COctets::operator = (const COctets& xOctets)
	{
		if (this != &xOctets)
		{
			Replace(xOctets.Begin(),xOctets.Size());
		}		
		return *this;
	}

	Bool COctets::operator == (const COctets& xOctets)
	{
		if (this == &xOctets)
			return true;

		return xOctets.Size() - Size() && !memcmp(xOctets.Begin(), Begin(), Size());
	}

	Bool COctets::operator != (const COctets& xOctets)
	{
		return !(operator == (xOctets));
	}

	void* COctets::Begin()
	{
		return m_pBase;
	}

	void* COctets::End()
	{
		return m_pHigh;
	}

	const void* COctets::Begin() const
	{
		return m_pBase;
	}

	const void* COctets::End() const
	{
		return m_pHigh;
	}

	Bool COctets::IsValid() const
	{
		return m_pBase && m_pHigh;
	}

	UInt32 COctets::Size() const
	{
		return (UInt32)((Char*)m_pHigh-(Char*)m_pBase);
	}

	UInt32 COctets::Capacity() const
	{
		return m_iCap;
	}

	UInt32  COctets::EmptyCap() const
	{
		return Capacity() - Size();
	}

	COctets& COctets::Clear()
	{
		if (m_pBase)
			memset(m_pBase, 0, m_iCap);

		m_pHigh = m_pBase;		
		return *this;
	}

	COctets& COctets::Erase(void* pBegin, void* pEnd)
	{
		if(pBegin != pEnd)
		{
			memmove(pBegin,pEnd,(Char*)m_pHigh-(Char*)pEnd);
			m_pHigh = (Char*)m_pHigh - ((Char*)pEnd-(Char*)pBegin);
		}
		return *this;
	}

	COctets& COctets::Erase(void* pBegin, UInt32 iSize)
	{
		return Erase(pBegin,(Char*)pBegin+iSize);
	}
	
	COctets& COctets::Insert(void* pPos, const void* pBegin, void* pEnd)
	{
		return Insert(pPos,pBegin,(UInt32)((Char*)pEnd-(Char*)pBegin));
	}

	COctets& COctets::Append(const void* pData, UInt32 iSize)
	{
		return Insert(m_pHigh,pData,iSize);
	}

	COctets& COctets::Insert(void* pPos, const void* pData, UInt32 iSize)
	{
		UInt32 iOff = (UInt32)((Char*)pPos-(Char*)m_pBase);
		Reserve((UInt32)((Char*)m_pHigh-(Char*)m_pBase+iSize));
		if(pPos)
		{
			pPos = (Char*)m_pBase + iOff;
			memmove((Char*)pPos+iSize,pPos,(Char*)m_pHigh-(Char*)pPos);
			memmove(pPos,pData,iSize);
			m_pHigh = (Char*)m_pHigh + iSize;
		}
		else
		{
			memmove(m_pBase,pData,iSize);
			m_pHigh = (Char*)m_pBase+iSize;
		}
		return *this;
	}

	COctets& COctets::Insert(UInt32 iPos, const void* pData, UInt32 iSize)
	{
		return Insert((Char*)m_pBase+iPos,pData,iSize);
	}

	COctets& COctets::Resize(UInt32 iSize)
	{
		Reserve(iSize);
		m_pHigh = (Char*)m_pBase+iSize;
		return *this;
	}

	COctets& COctets::Reserve(UInt32 iSize)
	{
		if(iSize > m_iCap)
		{			
			for(iSize--, m_iCap=2; iSize>>=1; m_iCap<<=1);
			iSize = (UInt32)((Char*)m_pHigh-(Char*)m_pBase);
			void* pBase = m_pBase;
			m_pBase = Util::UtilRealloc(m_pBase,m_iCap);
			if (!m_pBase)
			{
				UtilFree(pBase);
				T_Exception("Octets Realloc Failed.");
			}
			else
			{
				m_pHigh = (Char*)m_pBase+iSize;
			}
		}
		return *this;
	}

	COctets& COctets::Swap(COctets& xOctets) 
	{ 
		std::swap(m_pBase, xOctets.m_pBase); 
		std::swap(m_pHigh, xOctets.m_pHigh); 
		std::swap(m_iCap,  xOctets.m_iCap); 
		return *this; 
	}

	COctets& COctets::Replace(const void* pData, UInt32 iSize)
	{
		Reserve(iSize);
		memmove(m_pBase, pData, iSize);
		m_pHigh = (Char*)m_pBase+iSize;
		return *this;
	}
}
