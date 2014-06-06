#ifndef _OCTETSSTREAM_H_
#define _OCTETSSTREAM_H_

#include "Octets.h"

namespace Util
{
	/************************************************************************/
	/* ������������װ                                                       */
	/************************************************************************/
	class CMarshal;
	class UTIL_API COctetsStream : public COctets
	{
	public:
		//������Ĭ�Ϲ��캯��
		COctetsStream();

		//�趨��������������
		COctetsStream(Int32 iSize);

		//�趨��������������
		COctetsStream(void* pData, Int32 iSize);

		//�ֽ����������ݹ���
		COctetsStream(const COctets& xOctets);

		//��������
		COctetsStream(const COctetsStream& xOS);

		//��ֵ�����
		COctetsStream& operator = (const COctetsStream& xOS);

	public:
		//���������ﶨ��
		enum Transaction 
		{ 
			TransBegin, 
			TransCommit, 
			TransRollback 
		};	

	public:
		//�Ƿ����
		Bool   Eos() const;	

		//��ȡƫ��λ��
		UInt32 GetPos() const;

		//��ȡ����
		void   Pop(void* pData, UInt32 iSize);		

		//��ȡ��Ч�����ֽ���
		UInt32 AvailableSize() const;

		//�ƶ������α�
		UInt32 MoveNonius(Int32 iMark);

		//������Ч����ָ��
		void*  AvailableData() const;

	public:
		//�������
		COctetsStream& Clear();

		//�Ƴ��հ���
		COctetsStream& RemoveBlank();

		//д������
		COctetsStream& Push(const void* pData,UInt32 iSize);

		//�滻����
		COctetsStream& Replace(const void* pData,UInt32 iSize);

	public:
		//Int32д�뺯��
		COctetsStream& operator << (Int32 iVal);

		//Int32д������
		COctetsStream& operator >> (Int32& iVal);

		//UInt32д�뺯��
		COctetsStream& operator << (UInt32 iVal);

		//UInt32д������
		COctetsStream& operator >> (UInt32& iVal);

		//AStringд�뺯��
		COctetsStream& operator << (AString& sVal);

		//AStringд�뺯��
		COctetsStream& operator << (const AString& sVal);

		//AStringд������
		COctetsStream& operator >> (AString& sVal);

		//UStringд�뺯��
		COctetsStream& operator << (UString& sVal);

		//UStringд�뺯��
		COctetsStream& operator << (const UString& sVal);

		//UStringд������
		COctetsStream& operator >> (UString& sVal);

		//WStringд�뺯��
		COctetsStream& operator << (WString& sVal);

		//WStringд�뺯��
		COctetsStream& operator << (const WString& sVal);

		//WStringд������
		COctetsStream& operator >> (WString& sVal);

		//COctetsд�뺯��
		COctetsStream& operator << (COctets& xOctets);

		//COctetsд�뺯��
		COctetsStream& operator << (const COctets& xOctets);

		//COctetsд������
		COctetsStream& operator >> (COctets& xOctets);

		//CMarshalд�뺯��
		COctetsStream& operator << (CMarshal& xMarshal);

		//CMarshalд������
		COctetsStream& operator >> (CMarshal& xMarshal);

		//�����ύ����
		COctetsStream& operator >> (Transaction eTrans);

	public:
		//ģ����������Pop
		template<typename T> void Pop(T& tVal)
		{
			Pop(&tVal,sizeof(T));
		}	

		//ģ����������Push
		template<typename T> COctetsStream& Push(T tVal)
		{
			return Push(&tVal,sizeof(T));
		}			

	public:
		//vector����д��
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

		//vector����д��
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
		
		//list����д��
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

		//list����д��
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

		//map����д��
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

		//map����д��
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
		//�Զ����������͵����л�����
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
		//32λ�з�������ѹ��
		COctetsStream& CompactSint32(Int32 iVal);

		//32λ�з���������ѹ
		COctetsStream& UncompactSint32(Int32& iVal);

		//32λ�޷�������ѹ��
		COctetsStream& CompactUint32(UInt32 iVal);

		//32λ�޷���������ѹ
		COctetsStream& UncompactUint32(UInt32& iVal);	

	protected:
		UInt32  m_iPos;
		UInt32  m_iTransPos;
	};

	//����������
	typedef COctetsStream  OctetsStream;
}
#endif //_OCTETSSTREAM_H_
