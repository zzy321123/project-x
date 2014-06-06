#include "CParamVector.h"
#include "StringUtil.h"
#include "LoggerManager.h"

namespace Util
{
	CParamVector::CParamVector() : m_iSize(0)
	{
	}

	CParamVector::~CParamVector()
	{
		ClearParams();
	}

	void CParamVector::ClearParams()
	{
		HolderVector::iterator it = m_vParam.begin();
		for (;it!=m_vParam.end();it++)
		{
			BaseHolder* pHolder = *it;
			DEF_RELEASE(pHolder);
		}
		m_vParam.clear();
		m_iSize = 0;
	}

	CParamVector& CParamVector::operator = (const CParamVector& sParam)
	{
		if (this != &sParam)
		{
			Copy(sParam);
		}
		return *this;
	}

	Int32  CParamVector::Copy(const CParamVector& sParam)
	{
		if (this != &sParam)
		{
			//清空队列数据
			m_iSize = 0;

			//逐次复制
			for (UInt32 i=0;i<sParam.m_iSize;i++)
			{				
				A_Exception(sParam.m_vParam[i] != 0);

				//已存在Holder可直接复制
				if (i < m_vParam.size())
				{
					sParam.m_vParam[i]->Clone(m_vParam[i]);
					m_iSize ++;
				}
				//不存在Holder即新建
				else
				{
					BaseHolder* pHolder = 0;
					sParam.m_vParam[i]->Clone(pHolder);	
					
					Assert(pHolder != 0);
					m_vParam.push_back(pHolder);
					m_iSize ++;
				}
			}
		}		

		return m_iSize;
	}
	
	UInt32 CParamVector::Size() const
	{
		return m_iSize;
	}

	void CParamVector::Clear()
	{
		m_iSize = 0;
	}
}
