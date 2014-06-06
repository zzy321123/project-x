#ifndef _OCTETS_H_
#define _OCTETS_H_

#include "RefCounter.h"

namespace Util
{
	/************************************************************************/
	/* ���ݲ�����װ                                                          */
	/************************************************************************/
	class UTIL_API COctets : public CRefCounter
	{
	public:
		//����������
		COctets();
		
		//��ʼ�����ȹ���
		COctets(UInt32 iSize);

		//��ʼ�����ݹ���
		COctets(const void* pData, UInt32 iSize);

		//��ʼ�����ݹ���
		COctets(void* pBegin, void* pEnd);

		//��������
		COctets(const COctets& xOctets);

		//����
		virtual ~COctets();
		
	public:
		//��ֵ������
		COctets& operator = (const COctets& xOctets);

		//��ȱȽ�
		Bool operator == (const COctets& xOctets);

		//���ȱȽ�
		Bool operator != (const COctets& xOctets);

	public:
		//�������׵�ַ
		void*   Begin();

		//������β��ַ
		void*   End();

		//�������ֽڴ�С
		UInt32  Size() const;

		//�Ƿ���Ч,�ڴ��ѿ���?
		Bool    IsValid() const;

		//����������
		UInt32  Capacity() const;

		//ʣ��ռ��ֽڴ�С
		UInt32  EmptyCap() const;

	public:
		//�������׵�ַ
		const void*  Begin() const;

		//������β��ַ
		const void*  End() const;

		//�������
		COctets&  Clear();

		//Ĩ������
		COctets&  Erase(void* pBegin, void* pEnd);

		//Ĩ������
		COctets&  Erase(void* pBegin, UInt32 iSize);

		//��������
		COctets&  Insert(void* pPos, const void* pBegin, void* pEnd);

		//��������
		COctets&  Insert(void* pPos, const void* pData, UInt32 iSize);

		//��������
		COctets&  Insert(UInt32 iPos, const void* pData, UInt32 iSize);

		//�ҽ�����
		COctets&  Append(const void* pData, UInt32 iSize);

		//���ô�С
		COctets&  Resize(UInt32 iSize);

		//���ٿռ�
		COctets&  Reserve(UInt32 iSize);

		//�滻����
		COctets&  Replace(const void* pData, UInt32 iSize);

		//����������
		COctets&  Swap(COctets& xOctets);

	protected:
		//�������׵�ַ
		void*   m_pBase;
		//������β��ַ
		void*   m_pHigh;
		//��������
		UInt32  m_iCap;
	};
}
#endif //_OCTETS_H_
