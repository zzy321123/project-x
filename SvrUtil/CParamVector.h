#ifndef C_PARAMVECTOR_H
#define C_PARAMVECTOR_H

#include "CValueHolder.h"

namespace Util
{
	/************************************************************************/
	/* 多参数队列封装                                                       */
	/************************************************************************/
	class UTIL_API CParamVector : public CRefCounter
	{
	public:
		//多参数队列构造
		CParamVector();

		//多参数队列析构
		virtual ~CParamVector();		

		//参数队列赋值操作符(不建议使用赋值和拷贝构造)
		CParamVector& operator = (const CParamVector& sParam);

		//数据容器对象
		typedef CAnyType<true>	 BaseHolder;

		//多参数队列数据类型定义
		typedef vector<BaseHolder*>  HolderVector;

	public:
		//有效参数量
		virtual UInt32 Size() const;

		//清理(不释放)队列
		virtual void   Clear();
		
		//参数队列拷贝
		virtual Int32  Copy(const CParamVector& sParam);

	protected:
		//清除(并释放)参数容器
		virtual void   ClearParams();		

	public:
		//填充参数
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

		//获取参数
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
		//参数数目
		UInt32		 m_iSize;
		//参数队列
		HolderVector m_vParam;
	};
}
#endif
