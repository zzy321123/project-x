#ifndef _SCOPE_H_
#define _SCOPE_H_

#include "RefCounter.h"

namespace Util
{
	/************************************************************************/
	/* 作用域对象定义, 便于自动释放											*/
	/************************************************************************/
	class UTIL_API CScope
	{
	public:
		//数据删除作用域(new出来的数据)
		class UTIL_API CDataPtr : public CRefCounter
		{
		public:
			//构造
			CDataPtr(void* pData)
			{
				m_pData = (Char*)pData;
			}

			//析构
			~CDataPtr()
			{
				DEF_DELETE(m_pData);
			}

		protected:
			//数据对象
			Char*  m_pData;
		};

		//malloc内存作用域(CMalloc出来的内存)
		class UTIL_API CMallocPtr : public CRefCounter
		{
		public:
			//构造
			CMallocPtr(void* pData)
			{
				m_pData = pData;
			}

			//析构
			~CMallocPtr()
			{
				UtilFree(m_pData);
			}

		protected:
			//数据对象
			void*  m_pData;
		};

		//数组删除作用域(new出来的数组)
		class UTIL_API CDataArrayPtr : public CRefCounter
		{
		public:
			//构造
			CDataArrayPtr(void* pDataArray)
			{
				m_pDataArray = (Char*)pDataArray;
			}

			//析构
			~CDataArrayPtr()
			{
				DEF_DELETE_ARRAY(m_pDataArray);
			}

		protected:
			//数组对象
			Char*  m_pDataArray;
		};

		//引用计数对象删除作用域(new出来的RefCounter对象)
		class UTIL_API ObjPtr : public CRefCounter
		{
		public:
			//构造
			ObjPtr(CRefCounter* pObj)
			{
				m_pObj = pObj;
			}

			//析构
			~ObjPtr()
			{
				DEF_RELEASE(m_pObj);
			}

		protected:
			//引用计数对象
			CRefCounter*  m_pObj;
		};
	};
}
#endif //_SCOPE_H_
