#include "CFile.h"

namespace Util
{
	CFile::CFile()
	{
		m_iFileSize = 0;
		m_eOpenType = OPEN_READ;
	}

	CFile::~CFile(void)
	{
		Close();
	}

	Bool CFile::Open(const AString& sFile, OpenType eOpen)
	{
		Close();
		return true;
	}
	
	Int64 CFile::Read(void* pData, Int64 iSize)
	{
		return 0;
	}

	Int64 CFile::Write(const void* pData, Int64 iSize, Bool bFlush)
	{
		return 0;
	}

	Int64 CFile::Seek(Int64 iSize, SeekPos ePos)
	{
		return 0;
	}

	Bool CFile::Flush()
	{
		return true;
	}

	Bool CFile::Close()
	{
		return true;
	}

	Int64 CFile::Tell()
	{
		return 0;
	}

	CFile::CodeType CFile::GetCodeType(const void* pData)
	{
		const  UChar UTF8_TAG[]		= {0xEF,0xBB,0xBF};
		const  UChar UNICODE_TAG[]	= {0xFF,0xFE};

		UChar* pTmp  = (UChar*)pData;
		Size_t iSize = strlen((Char*)pData);

		if (iSize >= 3)
		{
			if(pTmp[0] == UTF8_TAG[0] && pTmp[1] == UTF8_TAG[1] && pTmp[2] == UTF8_TAG[2])
				return CODE_UTF8;
		}

		if (iSize >= 2)
		{
			if(pTmp[0] == UNICODE_TAG[0] && pTmp[1] == UNICODE_TAG[1])
				return CODE_UNICODE;
		}

		return CODE_ASCII;
	}

	Bool CFile::Chsize(Int64 iSize)
	{
		return false;
	}

	Bool CFile::IsWriteAble() const
	{
		if (m_eOpenType != OPEN_WRITE &&  m_eOpenType != OPEN_RW &&  m_eOpenType != OPEN_APPEND && m_eOpenType != OPEN_WR)
		{
			return false;
		}

		return true;
	}

	Bool CFile::IsReadAble() const
	{
		if (m_eOpenType != OPEN_READ && m_eOpenType != OPEN_RW && m_eOpenType != OPEN_WR)
		{
			return false;
		}

		return true;
	}
}

