#include "UtilZipReader.h"
#include "CDiskFile.h"
#include "OSOperator.h"
#include "StringUtil.h"
#include "LoggerManager.h"
#include "CScope.h"
#include "unzip.h"

namespace Util
{
	UtilZipReader::UtilZipReader(const AString& sFile) : m_pZip(0)
	{
		if (sFile.size())
			Open(sFile);
	}

	UtilZipReader::~UtilZipReader()
	{
		Close();
	}

	Bool UtilZipReader::Open(const AString& sFile)
	{
		Close();

		m_pZip = unzOpen(sFile.c_str());
		if (m_pZip)
		{
			if (unzGoToFirstFile(m_pZip) != UNZ_OK)
				return false;

#ifdef _DEBUG
			FmtPrint("Package<%s>: ", sFile.c_str());
#endif
			do 
			{
				unz_file_info sInfo;
				Char szFileName[MAX_PATH] = {0};
				if (unzGetCurrentFileInfo(m_pZip, &sInfo, szFileName, MAX_PATH, 0, 0, 0, 0) == UNZ_OK)
				{
					ZipInfo sZip;
					sZip.FileName = szFileName;
					sZip.FileSize = (Int32)sInfo.uncompressed_size;
					sZip.ComSize  = (Int32)sInfo.compressed_size;
					sZip.IsFile   = (sInfo.external_fa & 0x10) == 0 ? true : false;
					m_mFiles[szFileName] = sZip;

#ifdef _DEBUG
					FmtPrint("\t%s", szFileName);
#endif
				}
			} while (unzGoToNextFile(m_pZip) == UNZ_OK);

			return true;
		}
		return false;
	}

	Bool UtilZipReader::ExistFile(const AString& sZipFile, ZipInfo* pInfo)
	{
		ZipFiles::iterator it = m_mFiles.find(sZipFile);
		if (it != m_mFiles.end())
		{
			if (pInfo)
				*pInfo = it->second;

			return true;
		}
		return false;
	}

	Bool UtilZipReader::Read(const AString& sZipFile, void*  pData, Int32 iSize)
	{
		ZipInfo sInfo;
		if (m_pZip && ExistFile(sZipFile, &sInfo) && sInfo.IsFile)
		{
			if (unzLocateFile(m_pZip, sZipFile.c_str(), 0) != UNZ_OK)
				return false;
			
			if (unzOpenCurrentFile(m_pZip) != UNZ_OK)
				return false;

			Int32 iRet = unzReadCurrentFile(m_pZip, pData, iSize) == UNZ_OK ? true : false;
			unzCloseCurrentFile(m_pZip);

			return iRet == UNZ_OK;
		}
		return false;
	}

	Bool UtilZipReader::UnzipFile(const AString& sZipFile, const AString& sDiskFile)
	{
		ZipInfo sInfo;
		if (m_pZip && ExistFile(sZipFile, &sInfo) && sInfo.IsFile)
		{
			Char* pBuf = new Char[sInfo.FileSize];
			memset(pBuf, 0, sInfo.FileSize);
			CScope::CDataArrayPtr scope(pBuf);
			if (!Read(sZipFile, pBuf, sInfo.FileSize))
				return false;

			COSOperator::MakeSureFileName(sDiskFile);

			CDiskFile sFile;
			if (!sFile.Open(sDiskFile, CFile::OPEN_WRITE))
				return false;

			sFile.Write(pBuf, sInfo.FileSize, true);
			sFile.Close();

			return true;
		}
		return false;
	}

	Bool UtilZipReader::Unzip(const AString& sPath)
	{
		if (m_pZip)
		{
			AString sFolder = sPath;
			CStringUtil::Replace<AString>(sFolder, "\\", "/");
			if (sFolder[sFolder.size() - 1] != '/')
				sFolder += "/";

			ZipFiles::iterator it = m_mFiles.begin();
			for (;it!=m_mFiles.end();it++)
			{
				AString sDiskFile = sFolder + it->second.FileName;

				COSOperator::MakeSureFileName(sDiskFile);

				if (it->second.IsFile)
				{
					Char* pBuf = new Char[it->second.FileSize];
					memset(pBuf, 0, it->second.FileSize);
					CScope::CDataArrayPtr scope(pBuf);

					if (!Read(it->second.FileName, pBuf, it->second.FileSize))
						return false;

					CDiskFile sFile;
					if (!sFile.Open(sDiskFile, CFile::OPEN_WRITE))
						return false;

					sFile.Write(pBuf, it->second.FileSize, true);
					sFile.Close();
				}				
			}

			return true;
		}
		return false;
	}

	Bool UtilZipReader::Close()
	{
		m_mFiles.clear();
		if (m_pZip)
		{
			unzClose(m_pZip);
			m_pZip = 0;
		}
		return true;
	}
}
