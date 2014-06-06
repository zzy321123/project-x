#ifndef C_ANYTYPE_H
#define C_ANYTYPE_H

#include "RefCounter.h"

namespace Util
{
	/************************************************************************/
	/* ����ģ�����,ֻ�洢����ID�ͱ���ӿ�                                  */
	/************************************************************************/
	template<Bool typeEnable> class CAnyType : public CRefCounter
	{
	public:
		//����
		CAnyType(ULong iType = 0) : m_iType(iType)
		{
		}		

		//����
		virtual ~CAnyType()
		{
		}

		//��¡һ������(���಻�ɿ�¡)
		virtual CAnyType<typeEnable>* Clone() const
		{
			return 0;
		}

	public:
		//��ȡ����
		virtual ULong GetType()
		{
			return m_iType;
		}

		//��ȡֵ��ַ
		virtual void* GetValPtr()
		{
			return 0;
		}

		//���ͼ��
		virtual Bool  Check()
		{
			return typeEnable && m_iType;
		}

		//���ƶ���
		virtual Bool  Clone(CAnyType<typeEnable>*& pType) const
		{
			T_Exception("CAnyType::Clone Must Implement.");
			return false;
		}

	protected:
		//����ID
		ULong  m_iType;
	};
}
#endif
