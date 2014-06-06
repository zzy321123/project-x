#ifndef _OCTETS_H_
#define _OCTETS_H_

#include "RefCounter.h"

namespace Util
{
	/************************************************************************/
	/* 数据操作封装                                                          */
	/************************************************************************/
	class UTIL_API COctets : public CRefCounter
	{
	public:
		//数据流构造
		COctets();
		
		//初始化长度构造
		COctets(UInt32 iSize);

		//初始化数据构造
		COctets(const void* pData, UInt32 iSize);

		//初始化数据构造
		COctets(void* pBegin, void* pEnd);

		//拷贝构造
		COctets(const COctets& xOctets);

		//析构
		virtual ~COctets();
		
	public:
		//赋值操作符
		COctets& operator = (const COctets& xOctets);

		//相等比较
		Bool operator == (const COctets& xOctets);

		//不等比较
		Bool operator != (const COctets& xOctets);

	public:
		//数据流首地址
		void*   Begin();

		//数据流尾地址
		void*   End();

		//数据流字节大小
		UInt32  Size() const;

		//是否有效,内存已开辟?
		Bool    IsValid() const;

		//数据流容量
		UInt32  Capacity() const;

		//剩余空间字节大小
		UInt32  EmptyCap() const;

	public:
		//数据流首地址
		const void*  Begin() const;

		//数据流尾地址
		const void*  End() const;

		//清空数据
		COctets&  Clear();

		//抹除数据
		COctets&  Erase(void* pBegin, void* pEnd);

		//抹除数据
		COctets&  Erase(void* pBegin, UInt32 iSize);

		//插入数据
		COctets&  Insert(void* pPos, const void* pBegin, void* pEnd);

		//插入数据
		COctets&  Insert(void* pPos, const void* pData, UInt32 iSize);

		//插入数据
		COctets&  Insert(UInt32 iPos, const void* pData, UInt32 iSize);

		//挂接数据
		COctets&  Append(const void* pData, UInt32 iSize);

		//设置大小
		COctets&  Resize(UInt32 iSize);

		//开辟空间
		COctets&  Reserve(UInt32 iSize);

		//替换数据
		COctets&  Replace(const void* pData, UInt32 iSize);

		//数据流交换
		COctets&  Swap(COctets& xOctets);

	protected:
		//数据流首地址
		void*   m_pBase;
		//数据流尾地址
		void*   m_pHigh;
		//数据容量
		UInt32  m_iCap;
	};
}
#endif //_OCTETS_H_
