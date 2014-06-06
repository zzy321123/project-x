#ifndef UTIL_ZIP_H
#define UTIL_ZIP_H

#include "BaseHeader.h"

namespace Util
{
	/************************************************************************/
	/* ZIP压缩和解压                                                        */
	/************************************************************************/
	class UTIL_API UtilZip 
	{
	public:
		//压缩后的 预期数据大小
		static ULong GetRequiredSize(ULong iOriginal);
		
		//压缩
		static Bool  Compress(void* pOut, ULong& iOutLen, const void* pIn, ULong iInLen);

		//解压缩
		static Bool  UnCompress(void* pOut, ULong& iOutLen, const void* pIn, ULong iInLen);

	public:
		//Zlib流压缩初始化
		static Bool  DeflateInit(void* pStream, Int32 iLvl);

		//Zlib流解压缩初始化
		static Bool  InflateInit(void* pStream);

		//Zlib流压缩
		static Bool  Deflate(void* pStream, Int32 iFlush);

		//Zlib流解压缩
		static Bool  Inflate(void* pStream, Int32 iFlush);
	};
}
#endif
