#ifndef UTIL_ZIPREADER_H
#define UTIL_ZIPREADER_H

#include "RefCounter.h"

namespace Util
{
	/************************************************************************/
	/* Zip�ļ���ȡ��װ                                                      */
	/************************************************************************/
	class UTIL_API UtilZipReader : public CRefCounter
	{
	public:
		//����
		UtilZipReader(const AString& sFile = "");

		//����
		virtual ~UtilZipReader();

	public:
		//�ļ���Ϣ�б�
		struct ZipInfo
		{
			AString FileName;
			Int32	FileSize;
			Int32   ComSize;
			Bool    IsFile;

			ZipInfo()
			{
				FileName = "";
				FileSize = 0;
				ComSize  = 0;
				IsFile   = false;
			}

			Bool operator == (const ZipInfo& sInfo)
			{
				return FileName == sInfo.FileName;
			}
		};

		//���Ͷ���
		typedef map<AString, ZipInfo>  ZipFiles;
		
		//���Զ���
		DEFINE_PROPERTY(ZipFiles, m_mFiles, ZipFiles);

	public:
		//��Zip�ļ�
		virtual Bool  Open(const AString& sFile);

		//�Ƿ�����ļ�
		virtual Bool  ExistFile(const AString& sZipFile, ZipInfo* pInfo = 0);

		//���ļ�
		virtual Bool  Read(const AString& sZipFile, void*  pData, Int32 iSize);

		//ѹ��һ���ļ�������Ŀ¼��
		virtual Bool  UnzipFile(const AString& sZipFile, const AString& sDiskFile = "");

		//��ѹ����ָ��Ŀ¼
		virtual Bool  Unzip(const AString& sPath);

		//�ر��ļ���
		virtual Bool  Close();

	protected:
		//�ļ����
		PVoid  m_pZip;
	};
}
#endif
