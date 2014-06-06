#ifndef _BYTEORDER_H_
#define _BYTEORDER_H_

#include "BaseHeader.h"

namespace Util
{
	/************************************************************************/
	/* 转换到网络字节序函数封装                                             */
	/************************************************************************/
	class UTIL_API CByteOrder
	{
	public:
		//16位整数转换网络字节序
		static inline UInt16 ByteOrder16(UInt16 iVal)
		{
		#if !defined(UTIL_BIG_ENDIAN)
			#if defined(PLATFORM_WINDOWS)
				__asm ror iVal, 8
				return iVal;				
			#elif defined(PLATFORM_LINUX)
				register UInt16 iTmp = 0;
				__asm__ ("xchg %b0, %h0" : "=q"(iTmp) : "0"(iVal));
				return iTmp;
			#endif
		#else
			return iVal;
		#endif
		}

		//32位整数转换网络字节序
		static inline UInt32 ByteOrder32(UInt32 iVal)
		{
		#if !defined(UTIL_BIG_ENDIAN)
			#if defined(PLATFORM_WINDOWS)
				__asm mov eax, iVal
				__asm bswap eax
				__asm mov iVal, eax
				return iVal;				
			#elif defined(PLATFORM_LINUX)
				register UInt32 iTmp = 0;
				__asm__ ("bswap %0" : "=r"(iTmp) : "0"(iVal));
				return iTmp;
			#endif
		#else
			return iVal;
		#endif
		}

		//64位整数转换网络字节序
		static inline UInt64 ByteOrder64(UInt64 iVal)
		{
		#if !defined(UTIL_BIG_ENDIAN)
			union
			{
				UInt64 __ll;
				UInt32 __l[2];
			} i, o;
			i.__ll = iVal;
			o.__l[0] = ByteOrder32(i.__l[1]);
			o.__l[1] = ByteOrder32(i.__l[0]);
			return o.__ll;
		#else
			return iVal;
		#endif
		}
	};
}
#endif //_BYTEORDER_H_
