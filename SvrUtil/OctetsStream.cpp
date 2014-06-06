#include "OctetsStream.h"
#include "ByteOrder.h"
#include "Marshal.h"
#include "CScope.h"

namespace Util
{
	COctetsStream::COctetsStream() : m_iPos(0),m_iTransPos(0)
	{
	}

	COctetsStream::COctetsStream(Int32 iSize) : COctets(iSize),m_iPos(0),m_iTransPos(0)
	{
	}

	COctetsStream::COctetsStream(void* pData,Int32 iSize) : COctets(pData,iSize),m_iPos(0),m_iTransPos(0)
	{
	}

	COctetsStream::COctetsStream(const COctets& xOctets) : COctets(xOctets),m_iPos(0),m_iTransPos(0)
	{
	}

	COctetsStream::COctetsStream(const COctetsStream& xOS) : COctets(xOS),m_iPos(0),m_iTransPos(0)
	{
	}

	COctetsStream& COctetsStream::operator = (const COctetsStream& xOS)
	{
		if(&xOS != this)
		{
			m_iPos      = xOS.m_iPos;
			m_iTransPos = xOS.m_iTransPos;
			Replace(xOS.Begin(),xOS.Size());
		}
		return *this;
	}

	Bool COctetsStream::Eos() const
	{
		return m_iPos >= Size();
	}

	UInt32 COctetsStream::GetPos() const
	{
		return m_iPos;
	}

	void COctetsStream::Pop(void* pData,UInt32 iSize) 
	{
		if(!pData || !iSize || m_iPos + iSize > Size())
		{
			T_Exception("OctetsStream Pop Error.");
			return;
		}

		memcpy(pData, (Char*)Begin()+ m_iPos, iSize);
		m_iPos += iSize;
	}

	UInt32 COctetsStream::AvailableSize() const
	{
		if (Size() > m_iPos)
			return Size() - m_iPos;

		return 0;
	}

	UInt32 COctetsStream::MoveNonius(Int32 iMark)
	{
		Int32 iPos = (Int32)m_iPos;

		if (iPos + iMark < 0)
			m_iPos = 0;
		else if (iPos + iMark > (Int32)Size())
			m_iPos = Size();
		else
			m_iPos += (UInt32)iMark;

		return m_iPos;
	}

	void*  COctetsStream::AvailableData() const
	{
		return (Char*)m_pBase + m_iPos;
	}

	COctetsStream& COctetsStream::Clear()
	{
		m_iPos      = 0;
		m_iTransPos = 0;
		COctets::Clear();
		return *this;
	}

	COctetsStream& COctetsStream::Push(const void* pData,UInt32 iSize)
	{
		if (pData && iSize)
			Insert(End(),pData,iSize);

		return *this;
	}

	COctetsStream& COctetsStream::Replace(const void* pData,UInt32 iSize)
	{
		m_iPos      = 0;
		m_iTransPos = 0;
		COctets::Replace(pData,iSize);
		return *this;
	}	

	COctetsStream& COctetsStream::operator << (Int32 iVal)
	{
#ifdef COMPACT_INT32
		return CompactSint32(iVal);
#endif

		Push<Int32>(iVal);
		return *this;
	}

	COctetsStream& COctetsStream::operator >> (Int32& iVal)
	{
#ifdef COMPACT_INT32
		return UncompactSint32(iVal);
#endif

		Pop<Int32>(iVal);
		return *this;
	}

	COctetsStream& COctetsStream::operator << (UInt32 iVal)
	{
#ifdef COMPACT_INT32
		return CompactUint32(iVal);
#endif

		Push<UInt32>(iVal);
		return *this;
	}

	COctetsStream& COctetsStream::operator >> (UInt32& iVal)
	{
#ifdef COMPACT_INT32
		return UncompactUint32(iVal);
#endif

		Pop<UInt32>(iVal);
		return *this;
	}

	COctetsStream& COctetsStream::operator << (AString& sVal)
	{
		UInt32 iSize = (UInt32)sVal.size();
		*this << iSize;

		Push(sVal.c_str(),iSize*sizeof(Char));

		return *this;
	}

	COctetsStream& COctetsStream::operator << (const AString& sVal)
	{
		UInt32 iSize = (UInt32)sVal.size();
		*this << iSize;

		Push(sVal.c_str(),iSize*sizeof(Char));

		return *this;
	}

	COctetsStream& COctetsStream::operator >> (AString& sVal)
	{
		UInt32 iSize = 0;
		*this >> iSize;

		if (AvailableSize() < iSize*sizeof(Char))
		{
			T_Exception("OctetsStream Pop Error.");
			return *this;
		}

		sVal.clear();
		if (iSize)
		{
			Char* pBuf = (Char*)UtilMalloc((iSize+1)*sizeof(Char));
			CScope::CMallocPtr scope(pBuf);
			memset(pBuf,0,(iSize+1)*sizeof(Char));	

			Pop(pBuf,iSize*sizeof(Char));
			sVal = pBuf;
		}

		return *this;
	}

	COctetsStream& COctetsStream::operator << (UString& sVal)
	{
		UInt32 iSize = (UInt32)sVal.size();
		*this << iSize;
		
		Push(sVal.c_str(),iSize*sizeof(Utf8));

		return *this;
	}

	COctetsStream& COctetsStream::operator << (const UString& sVal)
	{
		UInt32 iSize = (UInt32)sVal.size();
		*this << iSize;

		Push(sVal.c_str(),iSize*sizeof(Utf8));

		return *this;
	}

	COctetsStream& COctetsStream::operator >> (UString& sVal)
	{
		UInt32 iSize = 0;
		*this >> iSize;

		if (AvailableSize() < iSize*sizeof(UChar))
		{
			T_Exception("OctetsStream Pop Error.");
			return *this;
		}

		sVal.clear();
		if (iSize)
		{
			Utf8* pBuf = (Utf8*)UtilMalloc((iSize+1)*sizeof(Utf8));
			CScope::CMallocPtr scope(pBuf);
			memset(pBuf,0,(iSize+1)*sizeof(Utf8));	

			Pop(pBuf,iSize*sizeof(Utf8));
			sVal = pBuf;
		}

		return *this;
	}

	COctetsStream& COctetsStream::operator << (WString& sVal)
	{
		UInt32 iSize = (UInt32)sVal.size();
		*this << iSize;

		Push(sVal.c_str(),iSize*sizeof(WChar));

		return *this;
	}

	COctetsStream& COctetsStream::operator << (const WString& sVal)
	{
		UInt32 iSize = (UInt32)sVal.size();
		*this << iSize;

		Push(sVal.c_str(),iSize*sizeof(WChar));

		return *this;
	}

	COctetsStream& COctetsStream::operator >> (WString& sVal)
	{
		UInt32 iSize = 0;
		*this >> iSize;

		if (AvailableSize() < iSize*sizeof(WChar))
		{
			T_Exception("OctetsStream Pop Error.");
			return *this;
		}

		sVal.clear();
		if (iSize)
		{
			WChar* pBuf = (WChar*)UtilMalloc((iSize+1)*sizeof(WChar));
			CScope::CMallocPtr scope(pBuf);
			memset(pBuf,0,(iSize+1)*sizeof(WChar));
			
			Pop(pBuf,iSize*sizeof(WChar));
			sVal = pBuf;
		}

		return *this;
	}

	COctetsStream& COctetsStream::operator << (COctets& xOctets)
	{
		UInt32 iSize = xOctets.Size();
		*this << iSize;

		Push(xOctets.Begin(),iSize);

		return *this;
	}

	COctetsStream& COctetsStream::operator << (const COctets& xOctets)
	{
		UInt32 iSize = xOctets.Size();
		*this << iSize;

		Push(xOctets.Begin(),iSize);

		return *this;
	}

	COctetsStream& COctetsStream::operator >> (COctets& xOctets)
	{
		UInt32 iSize = 0;
		*this >> iSize;

		if (AvailableSize() < iSize)
		{
			T_Exception("OctetsStream Pop Error.");
			return *this;
		}

		xOctets.Clear();
		if (iSize)
		{
			xOctets.Reserve(iSize);

			Char* pData = (Char*)UtilMalloc(iSize);
			CScope::CMallocPtr scope(pData);
			memset(pData,0,iSize);

			Pop(pData,iSize);
			xOctets.Replace(pData,iSize);
		}

		return *this;
	}

	COctetsStream& COctetsStream::operator << (CMarshal& xMarshal)
	{
		return xMarshal.Marshal(*this);
	}

	COctetsStream& COctetsStream::operator >> (CMarshal& xMarshal)
	{
		xMarshal.Clear();
		return xMarshal.Unmarshal(*this);
	}

	COctetsStream& COctetsStream::operator >> (COctetsStream::Transaction eTrans)
	{
		switch (eTrans)
		{
		case COctetsStream::TransBegin:
			m_iTransPos = m_iPos;
			break;

		case COctetsStream::TransRollback:
			m_iPos = m_iTransPos;
			break;

		case COctetsStream::TransCommit:
			if (m_iPos >= PAGE_SIZE)
			{
				if (!IsValid())
				{
					T_Exception("OctetsStream Invalid.");
					return *this;
				}

				Erase((Char*)Begin(), (Char*)Begin()+m_iPos);	
				m_iPos = 0;
			}
			break;
		}
		return *this;
	}

	COctetsStream& COctetsStream::RemoveBlank()
	{
		if (!IsValid())
		{
			T_Exception("OctetsStream Invalid.");
			return *this;
		}

		if (m_iPos)
		{
			Erase((Char*)Begin(), (Char*)Begin()+m_iPos);	
			m_iPos = 0;
		}
		return *this;
	}

	COctetsStream& COctetsStream::CompactSint32(Int32 iVal)
	{
		if (iVal >= 0)
		{
			if (iVal < 0x40) 
				return Push<UInt8>((UInt8)iVal);
			else if (iVal < 0x2000) 
				return Push<UInt16>(CByteOrder::ByteOrder16(iVal|0x8000));
			else if (iVal < 0x10000000) 
				return Push<UInt32>(CByteOrder::ByteOrder32(iVal|0xc0000000));

			Push<UInt8>((UInt8)0xe0);
			return Push<UInt32>(CByteOrder::ByteOrder32(iVal));
		}

		if (-iVal > 0)
		{
			iVal = -iVal;
			if (iVal < 0x40) 
				return Push<UInt8>((UInt8)iVal|0x40);
			else if (iVal < 0x2000) 
				return Push<UInt16>(CByteOrder::ByteOrder16(iVal|0xa000));
			else if (iVal < 0x10000000) 
				return Push<UInt32>(CByteOrder::ByteOrder32(iVal|0xd0000000));

			Push<UInt8>((unsigned char)0xf0);
			return Push<UInt32>(CByteOrder::ByteOrder32(iVal));
		}

		Push<UInt8>((UInt8)0xf0);
		return Push<UInt32>(CByteOrder::ByteOrder32(iVal));
	}

	COctetsStream& COctetsStream::UncompactSint32(Int32& iVal)
	{
		if (!IsValid())
		{
			T_Exception("OctetsStream Invalid.");
			return *this;
		}

		UInt8 iFlag   = (*((UInt8*)Begin() + m_iPos) & 0xf0);
		UInt8 iInt8   = 0;
		UInt16 iInt16 = 0;
		UInt32 iInt32 = 0;

		switch (iFlag)
		{
		case 0xf0:
			Pop<UInt8>(iInt8);
			Pop<UInt32>(iInt32);
			iInt32 = CByteOrder::ByteOrder32(iInt32);
			iVal   = -((Int32)iInt32);
			return *this;

		case 0xe0:
			Pop<UInt8>(iInt8);
			Pop<UInt32>(iInt32);
			iInt32 = CByteOrder::ByteOrder32(iInt32);
			iVal   = iInt32;
			return *this;

		case 0xd0:
			Pop<UInt32>(iInt32);
			iInt32 = CByteOrder::ByteOrder32(iInt32);
			iVal   = -((Int32)(iInt32 & ~0xd0000000));
			return *this;

		case 0xc0:
			Pop<UInt32>(iInt32);
			iInt32 = CByteOrder::ByteOrder32(iInt32);
			iVal   = iInt32 & ~0xc0000000;
			return *this;

		case 0xb0:
		case 0xa0:
			Pop<UInt16>(iInt16);
			iInt16 = CByteOrder::ByteOrder16(iInt16);
			iVal   = -((Int32)(iInt16 & ~0xa000));
			return *this;

		case 0x90:
		case 0x80:
			Pop<UInt16>(iInt16);
			iInt16 = CByteOrder::ByteOrder16(iInt16);
			iVal   = iInt16 & ~0x8000;
			return *this;

		case 0x70:
		case 0x60:
		case 0x50:
		case 0x40:
			Pop<UInt8>(iInt8);
			iVal = -((Int32)(iInt8 & ~0x40));
			return *this;
		}

		Pop<UInt8>(iInt8);
		iVal = iInt8;
		return *this;
	}

	COctetsStream& COctetsStream::CompactUint32(UInt32 iVal)
	{
		if (iVal < 0x80) 
			return Push<UInt8>((UInt8)iVal);
		else if (iVal < 0x4000) 
			return Push<UInt16>(CByteOrder::ByteOrder16(iVal|0x8000));
		else if (iVal < 0x20000000) 
			return Push<UInt32>(CByteOrder::ByteOrder32(iVal|0xc0000000));

		Push<UInt8>((UInt8)0xe0);
		return Push<UInt32>(CByteOrder::ByteOrder32(iVal));
	}

	COctetsStream& COctetsStream::UncompactUint32(UInt32& iVal)
	{
		if (!IsValid())
		{
			T_Exception("OctetsStream Invalid.");
			return *this;
		}

		UInt8 iFlag   = (*((UInt8*)Begin() + m_iPos) & 0xe0);
		UInt8 iInt8   = 0;
		UInt16 iInt16 = 0;
		UInt32 iInt32 = 0;

		switch (iFlag)
		{
		case 0xe0:
			Pop<UInt8>(iInt8);
			Pop<UInt32>(iVal);
			iVal = CByteOrder::ByteOrder32(iVal);
			return *this;

		case 0xc0:
			Pop<UInt32>(iInt32);
			iInt32 = CByteOrder::ByteOrder32(iInt32);
			iVal   = iInt32 & ~0xc0000000;
			return *this;

		case 0xa0:
		case 0x80:
			Pop<UInt16>(iInt16);
			iInt16 = CByteOrder::ByteOrder16(iInt16);
			iVal   = iInt16 & ~0x8000;
			return *this;
		}

		Pop<UInt8>(iInt8);
		iVal = iInt8;
		return *this;
	}
}
