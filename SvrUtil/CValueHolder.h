#ifndef C_VALUEHOLDER_H
#define C_VALUEHOLDER_H

#include "CAnyType.h"

namespace Util
{
	/************************************************************************/
	/* 泛型数据容器定义,模板类型需要提供赋值操作符(operator =)              */
	/************************************************************************/	
	template<class T> class CValueHolder : public CAnyType<true>
	{
	public:
		//构造函数
		CValueHolder(const T& tVal = 0) : m_tVal(tVal) 
		{
		}

		//析构函数
		virtual ~CValueHolder()
		{
		}

		//赋值操作符
		CValueHolder<T>& operator = (const CValueHolder<T>& sHolder)
		{
			if (this != &sHolder)
			{
				m_tVal = sHolder.m_tVal;
			}
			return *this;
		}

	public:
		//获取模板类型的全局ID
		static ULong  GetHolderId()
		{
			static T* static_HolderId = 0;
			ULong iAddr = reinterpret_cast<ULong>(&static_HolderId);
			return iAddr;
		}

	public:
		//获取类型
		virtual ULong GetType()
		{
			if (!m_iType)
				m_iType = GetHolderId();

			return m_iType;
		}

		//获取值地址
		virtual void*  GetValPtr()
		{
			return &m_tVal;
		}

		//复制对象
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
		//获取数据
		const T&  GetValue() const
		{
			return m_tVal;
		}		

		//设置数据
		void  SetValue(const T& tVal)
		{
			m_tVal = tVal;
		}	

	protected:
		//泛型数据对象
		T  m_tVal;
	};
}
#endif
