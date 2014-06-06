#ifndef MY_BASEOBJECT_H
#define MY_BASEOBJECT_H

#include "Exception.h"

namespace Util
{
	class UTIL_API CRefCounter
	{
	public:
		//������
		CRefCounter();

		//������������
		virtual ~CRefCounter();

	public:
		//��������
		virtual Int32 AddRef();

		//��������
		virtual Int32 DecRef();

		//��ȡ����
		virtual Int32 GetRef() const;

		//�ͷ�(���ü���<=0ʱ����ɾ��)
		virtual void Release();

	public:
		//���󴴽��ڴ濪�ٲ���������
		void* operator new(Size_t iSize);

		//����ɾ������������
		void operator delete(void* pData, Size_t iSize);

	protected:
		//���ü���ֵ
		volatile Long m_iRef;
	};
}


#endif //MY_BASEOBJECT_H