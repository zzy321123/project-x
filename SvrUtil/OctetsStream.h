#ifndef _OCTETSSTREAM_H_
#define _OCTETSSTREAM_H_

#include "Octets.h"

namespace Util
{
	/************************************************************************/
	/* 数据流操作封装                                                       */
	/************************************************************************/
	class CMarshal;
	class UTIL_API COctetsStream : public COctets
	{
	public:
		//数据流默认构造函数
		COctetsStream();

		//设定长度数据流构造
		COctetsStream(Int32 iSize);

		//设定数据内容流构造
		COctetsStream(void* pData, Int32 iSize);

		//字节流基类数据构造
		COctetsStream(const COctets& xOctets);

		//拷贝构造
		COctetsStream(const COctetsStream& xOS);

		//赋值运算符
		COctetsStream& operator = (const COctetsStream& xOS);

	public:
		//流操作事物定义
		enum Transaction 
		{ 
			TransBegin, 
			TransCommit, 
			TransRollback 
		};	

	public:
		//是否结束
		Bool   Eos() const;	

		//读取偏移位置
		UInt32 GetPos() const;

		//读取数据
		void   Pop(void* pData, UInt32 iSize);		

		//获取有效数据字节数
		UInt32 AvailableSize() const;

		//移动数据游标
		UInt32 MoveNonius(Int32 iMark);

		//但其有效数据指针
		void*  AvailableData() const;

	public:
		//清除数据
		COctetsStream& Clear();

		//移除空白区
		COctetsStream& RemoveBlank();

		//写入数据
		COctetsStream& Push(const void* pData,UInt32 iSize);

		//替换数据
		COctetsStream& Replace(const void* pData,UInt32 iSize);

	public:
		//Int32写入函数
		COctetsStream& operator << (Int32 iVal);

		//Int32写出函数
		COctetsStream& operator >> (Int32& iVal);

		//UInt32写入函数
		COctetsStream& operator << (UInt32 iVal);

		//UInt32写出函数
		COctetsStream& operator >> (UInt32& iVal);

		//AString写入函数
		COctetsStream& operator << (AString& sVal);

		//AString写入函数
		COctetsStream& operator << (const AString& sVal);

		//AString写出函数
		COctetsStream& operator >> (AString& sVal);

		//UString写入函数
		COctetsStream& operator << (UString& sVal);

		//UString写入函数
		COctetsStream& operator << (const UString& sVal);

		//UString写出函数
		COctetsStream& operator >> (UString& sVal);

		//WString写入函数
		COctetsStream& operator << (WString& sVal);

		//WString写入函数
		COctetsStream& operator << (const WString& sVal);

		//WString写出函数
		COctetsStream& operator >> (WString& sVal);

		//COctets写入函数
		COctetsStream& operator << (COctets& xOctets);

		//COctets写入函数
		COctetsStream& operator << (const COctets& xOctets);

		//COctets写出函数
		COctetsStream& operator >> (COctets& xOctets);

		//CMarshal写入函数
		COctetsStream& operator << (CMarshal& xMarshal);

		//CMarshal写出函数
		COctetsStream& operator >> (CMarshal& xMarshal);

		//事物提交函数
		COctetsStream& operator >> (Transaction eTrans);

	public:
		//模板类型数据Pop
		template<typename T> void Pop(T& tVal)
		{
			Pop(&tVal,sizeof(T));
		}	

		//模板类型数据Push
		template<typename T> COctetsStream& Push(T tVal)
		{
			return Push(&tVal,sizeof(T));
		}			

	public:
		//vector容器写入
		template<typename T> COctetsStream& operator << (vector<T>& rhsVal)
		{
			UInt32 iSize = (UInt32)rhsVal.size();
			*this << iSize;

			for (UInt32 i=0; i < iSize; i++)
				*this << (T&)rhsVal[i];

			return *this;
		}

		template<typename T> COctetsStream& operator << (const vector<T>& rhsVal)
		{
			UInt32 iSize = (UInt32)rhsVal.size();
			*this << iSize;

			for (UInt32 i=0;i<iSize;i++)
				*this << (const T&)rhsVal[i];

			return *this;
		}

		//vector容器写出
		template<typename T> COctetsStream& operator >> (vector<T>& rhsVal)
		{
			UInt32 iSize = 0;
			*this >> iSize;

			rhsVal.clear();
			if (iSize)
			{				
				for (UInt32 i=0; i < iSize; i++)
				{
					T tVal;
					*this >> tVal;
					rhsVal.push_back(tVal);
				}
			}

			return *this;
		}
		
		//list容器写入
		template<typename T> COctetsStream& operator << (list<T>& rhsVal)
		{
			UInt32 iSize = (UInt32)rhsVal.size();
			*this << iSize;

			typename list<T>::iterator it = rhsVal.begin();
			for (;it!=rhsVal.end();it++)
				*this << (T&)*it;

			return *this;
		}

		template<typename T> COctetsStream& operator << (const list<T>& rhsVal)
		{
			UInt32 iSize = (UInt32)rhsVal.size();
			*this << iSize;

			typename list<T>::iterator it = rhsVal.begin();
			for (;it!=rhsVal.end();it++)
				*this << (const T&)*it;

			return *this;
		}

		//list容器写入
		template<typename T> COctetsStream& operator >> (list<T>& rhsVal)
		{
			UInt32 iSize = 0;
			*this >> iSize;

			rhsVal.clear();
			for (UInt32 i=0;i<iSize;i++)
			{
				T tVal;
				*this >> tVal;
				rhsVal.push_back(tVal);
			}

			return *this;
		}

		//map容器写入
		template<typename TKey,typename TVal> COctetsStream& operator << (map<TKey,TVal>& rhsVal)
		{
			UInt32 iSize = (UInt32)rhsVal.size();
			*this << iSize;

			typename map<TKey,TVal>::iterator it = rhsVal.begin();
			for (;it!=rhsVal.end(); ++it)
			{
				*this << (TKey&)it->first;
				*this << (TVal&)it->second;
			}

			return *this;
		}

		template<typename TKey,typename TVal> COctetsStream& operator << (const map<TKey,TVal>& rhsVal)
		{
			UInt32 iSize = (UInt32)rhsVal.size();
			*this << iSize;

			typename map<TKey,TVal>::const_iterator it = rhsVal.begin();
			for (;it!=rhsVal.end();it++)
			{
				*this << (const TKey&)it->first;
				*this << (const TVal&)it->second;
			}

			return *this;
		}

		//map容器写出
		template<typename TKey,typename TVal> COctetsStream& operator >> (map<TKey,TVal>& rhsVal)
		{
			UInt32 iSize = 0;
			*this >> iSize;

			rhsVal.clear();
			for (UInt32 i=0;i<iSize;i++)
			{
				TKey _tKey;
				TVal _tVal;
				*this >> _tKey;
				*this >> _tVal;
				rhsVal[_tKey] = _tVal;
			}

			return *this;
		}

	public:
		//自定义数据类型的序列化操作
		#define OCTETS_STREAM_SERIALIZE(Type)\
		COctetsStream& operator << (Type& rhsVal)\
		{\
			Push<Type>(rhsVal);\
			return *this;\
		};\
		COctetsStream& operator >> (Type& rhsVal)\
		{\
			rhsVal = 0;\
			Pop<Type>(rhsVal);\
			return *this;\
		}

		OCTETS_STREAM_SERIALIZE(Bool);
		OCTETS_STREAM_SERIALIZE(WChar);
		OCTETS_STREAM_SERIALIZE(Int8);
		OCTETS_STREAM_SERIALIZE(UInt8);
		OCTETS_STREAM_SERIALIZE(Int16);
		OCTETS_STREAM_SERIALIZE(UInt16);
		OCTETS_STREAM_SERIALIZE(Int64);
		OCTETS_STREAM_SERIALIZE(UInt64);
		OCTETS_STREAM_SERIALIZE(Float);
		OCTETS_STREAM_SERIALIZE(Double);

		#undef OCTETS_STREAM_SERIALIZE

	protected:
		//32位有符号整数压缩
		COctetsStream& CompactSint32(Int32 iVal);

		//32位有符号整数解压
		COctetsStream& UncompactSint32(Int32& iVal);

		//32位无符号整数压缩
		COctetsStream& CompactUint32(UInt32 iVal);

		//32位无符号整数解压
		COctetsStream& UncompactUint32(UInt32& iVal);	

	protected:
		UInt32  m_iPos;
		UInt32  m_iTransPos;
	};

	//简便操作定义
	typedef COctetsStream  OctetsStream;
}
#endif //_OCTETSSTREAM_H_
