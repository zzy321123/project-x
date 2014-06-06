#ifndef C_DISKFILE_H
#define C_DISKFILE_H

#include "CFile.h"

namespace Util
{
	/************************************************************************/
	/* �ļ���������,�ṩ�ļ������Ļ����ӿ�                                  */
	/************************************************************************/
	class UTIL_API CDiskFile : public CFile
	{
	public:
		//Ĭ�Ϲ��캯��
		CDiskFile();

		//�����ļ���Դ
		virtual ~CDiskFile();

	public:
		//�ļ���
		DEFINE_PROPERTY(AString, m_sFileName, FileName);	

	public:
		//���ļ�,sFileΪ�ļ���,eCodeֻ������д�ļ���ʱ�����Ч
		virtual Bool   Open(const AString& sFile, OpenType eOpen = OPEN_READ);

		//��ȡ�ļ�����,pDataΪ�洢�Ŀռ�,iSizeΪ��ȡ�ֽ���
		virtual Int64  Read(void* pData, Int64 iSize);

		//д�ļ�,pDataΪд�������,iSizeΪд���ֽ���
		virtual Int64  Write(const void* pData, Int64 iSize, Bool bFlush = false);

		//ָ��ƫ��
		virtual Int64  Seek(Int64 iOffset, SeekPos ePos = POS_BEGIN);

		//���㵽�ļ�ͷƫ���ֽ�
		virtual Int64  Tell();

		//flush�ļ�����
		virtual Bool   Flush();

		//�ر��ļ����ͷ���վ���ڴ�
		virtual Bool   Close();

		virtual Bool   Chsize(Int64 iSize);

	public:
		//�ļ����
		virtual FILE*  GetFileHandle();

		//д�����ַ�
		virtual Bool   WriteCodeType(CodeType eType);

		//��һ��
		virtual Int64  ReadLine(void* pData, Int64 iBufSize,UChar cSeparate = '\n');

		//�ļ�ָ���Ƿ񵽽�β
		virtual Bool   IsEndPos();

	protected:
		//�ļ����
		FILE*	m_pFile;
	};
}
#endif
