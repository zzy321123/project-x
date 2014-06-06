#ifndef UTIL_ZIP_H
#define UTIL_ZIP_H

#include "BaseHeader.h"

namespace Util
{
	/************************************************************************/
	/* ZIPѹ���ͽ�ѹ                                                        */
	/************************************************************************/
	class UTIL_API UtilZip 
	{
	public:
		//ѹ����� Ԥ�����ݴ�С
		static ULong GetRequiredSize(ULong iOriginal);
		
		//ѹ��
		static Bool  Compress(void* pOut, ULong& iOutLen, const void* pIn, ULong iInLen);

		//��ѹ��
		static Bool  UnCompress(void* pOut, ULong& iOutLen, const void* pIn, ULong iInLen);

	public:
		//Zlib��ѹ����ʼ��
		static Bool  DeflateInit(void* pStream, Int32 iLvl);

		//Zlib����ѹ����ʼ��
		static Bool  InflateInit(void* pStream);

		//Zlib��ѹ��
		static Bool  Deflate(void* pStream, Int32 iFlush);

		//Zlib����ѹ��
		static Bool  Inflate(void* pStream, Int32 iFlush);
	};
}
#endif
