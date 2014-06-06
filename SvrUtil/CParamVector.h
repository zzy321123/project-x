#ifndef C_PARAMVECTOR_H
#define C_PARAMVECTOR_H

#include "CValueHolder.h"

namespace Util
{
	/************************************************************************/
	/* ��������з�װ                                                       */
	/************************************************************************/
	class UTIL_API CParamVector : public CRefCounter
	{
	public:
		//��������й���
		CParamVector();

		//�������������
		virtual ~CParamVector();		

		//�������и�ֵ������(������ʹ�ø�ֵ�Ϳ�������)
		CParamVector& operator = (const CParamVector& sParam);

		//������������
		typedef CAnyType<true>	 BaseHolder;

		//����������������Ͷ���
		typedef vector<BaseHolder*>  HolderVector;

	public:
		//��Ч������
		virtual UInt32 Size() const;

		//����(���ͷ�)����
		virtual void   Clear();
		
		//�������п���
		virtual Int32  Copy(const CParamVector& sParam);

	protected:
		//���(���ͷ�)��������
		virtual void   ClearParams();		

	public:
		//������
		template <class T> void Push(const T& tVal)
		{
			if (m_vParam.size() <= m_iSize)
			{
				CValueHolder<T>* pHolder = new CValueHolder<T>(tVal);
				UInt32 iType = 0;
				iType = pHolder->GetType();
				m_vParam.push_back(pHolder);
			}
			else 
			{
				BaseHolder* pHolder = (BaseHolder*)m_vParam[m_iSize];
				UInt32 iType = (UInt32)pHolder->GetType();
				if (!pHolder || iType != CValueHolder<T>::GetHolderId())
				{
					DEF_RELEASE(pHolder);
					m_vParam[m_iSize] = 0;

					pHolder = new CValueHolder<T>(tVal);
					m_vParam[m_iSize] = pHolder;
				}
				else
				{
					((CValueHolder<T>*)pHolder)->SetValue(tVal);
				}				
			}		
			m_iSize ++;
		}		

		//��ȡ����
		template <class T> const T& Get(UInt32 iIdx) const
		{
			Assert(iIdx < m_iSize);
			BaseHolder* pHolder = (BaseHolder*)m_vParam[iIdx];

			UInt32 iType = 0;
			iType = (UInt32)pHolder->GetType();
			Assert(pHolder && iType == CValueHolder<T>::GetHolderId());
			
			return ((CValueHolder<T>*)pHolder)->GetValue();
		}		

	protected:		
		//������Ŀ
		UInt32		 m_iSize;
		//��������
		HolderVector m_vParam;
	};
}
#endif
