#ifndef C_VALUEHOLDER_H
#define C_VALUEHOLDER_H

#include "CAnyType.h"

namespace Util
{
	/************************************************************************/
	/* ����������������,ģ��������Ҫ�ṩ��ֵ������(operator =)              */
	/************************************************************************/	
	template<class T> class CValueHolder : public CAnyType<true>
	{
	public:
		//���캯��
		CValueHolder(const T& tVal = 0) : m_tVal(tVal) 
		{
		}

		//��������
		virtual ~CValueHolder()
		{
		}

		//��ֵ������
		CValueHolder<T>& operator = (const CValueHolder<T>& sHolder)
		{
			if (this != &sHolder)
			{
				m_tVal = sHolder.m_tVal;
			}
			return *this;
		}

	public:
		//��ȡģ�����͵�ȫ��ID
		static ULong  GetHolderId()
		{
			static T* static_HolderId = 0;
			ULong iAddr = reinterpret_cast<ULong>(&static_HolderId);
			return iAddr;
		}

	public:
		//��ȡ����
		virtual ULong GetType()
		{
			if (!m_iType)
				m_iType = GetHolderId();

			return m_iType;
		}

		//��ȡֵ��ַ
		virtual void*  GetValPtr()
		{
			return &m_tVal;
		}

		//���ƶ���
		virtual Bool   Clone(CAnyType<true>*& pHolder) const
		{
			if (pHolder && pHolder->GetType() == m_iType)
			{
				void* pVal = pHolder->GetValPtr();
				Assert(pVal != 0);
				if (pVal)
				{
					*((T*)pVal) = m_tVal;
				}
			}
			else
			{
				DEF_RELEASE(pHolder);
				pHolder = new CValueHolder<T>(m_tVal);
			}
			return pHolder != 0;
		}

	public:
		//��ȡ����
		const T&  GetValue() const
		{
			return m_tVal;
		}		

		//��������
		void  SetValue(const T& tVal)
		{
			m_tVal = tVal;
		}	

	protected:
		//�������ݶ���
		T  m_tVal;
	};
}
#endif
