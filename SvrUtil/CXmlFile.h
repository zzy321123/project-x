#ifndef C_XMLFILE_H
#define C_XMLFILE_H

#include "CDiskFile.h"
#include "CXmlAttribute.h"
#include "CXmlElement.h"
#include "CXmlDocument.h"

namespace Util
{
	/************************************************************************/
	/* XML�ļ���д��װ(�����ʽ: ASCII | UTF8)								*/
	/************************************************************************/
	class UTIL_API CXmlFile : public CRefCounter
	{
	public:
		//XML�ļ�����
		CXmlFile();

		//XML�ļ�����
		virtual ~CXmlFile();

	public:
		//���ļ�
		template<class StringType> Bool  Open(const AString& sFile, CXmlDocument<StringType>& sDoc)
		{
			return ParseFile(sFile, &sDoc);
		}

		//���ڴ����pData�ڴ�����,iSizeΪ�����ֽ���,bCodeHeader���������ֽ�ͷ�ֽ�,eCode��������
		template<class StringType> Bool  Open(const void* pData, Int32 iSize, CXmlDocument<StringType>& sDoc)
		{
			return ParseData(pData, iSize, &sDoc);
		}

	protected:
		//����XML�ļ�����
		virtual Bool  ParseFile(const AString& sFile, PVoid pDoc);

		//����XML�ڴ�����
		virtual Bool  ParseData(const void* pData, Size_t iSize, PVoid pDoc);
	};
}
#endif
