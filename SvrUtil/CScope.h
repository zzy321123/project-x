#ifndef _SCOPE_H_
#define _SCOPE_H_

#include "RefCounter.h"

namespace Util
{
	/************************************************************************/
	/* �����������, �����Զ��ͷ�											*/
	/************************************************************************/
	class UTIL_API CScope
	{
	public:
		//����ɾ��������(new����������)
		class UTIL_API CDataPtr : public CRefCounter
		{
		public:
			//����
			CDataPtr(void* pData)
			{
				m_pData = (Char*)pData;
			}

			//����
			~CDataPtr()
			{
				DEF_DELETE(m_pData);
			}

		protected:
			//���ݶ���
			Char*  m_pData;
		};

		//malloc�ڴ�������(CMalloc�������ڴ�)
		class UTIL_API CMallocPtr : public CRefCounter
		{
		public:
			//����
			CMallocPtr(void* pData)
			{
				m_pData = pData;
			}

			//����
			~CMallocPtr()
			{
				UtilFree(m_pData);
			}

		protected:
			//���ݶ���
			void*  m_pData;
		};

		//����ɾ��������(new����������)
		class UTIL_API CDataArrayPtr : public CRefCounter
		{
		public:
			//����
			CDataArrayPtr(void* pDataArray)
			{
				m_pDataArray = (Char*)pDataArray;
			}

			//����
			~CDataArrayPtr()
			{
				DEF_DELETE_ARRAY(m_pDataArray);
			}

		protected:
			//�������
			Char*  m_pDataArray;
		};

		//���ü�������ɾ��������(new������RefCounter����)
		class UTIL_API ObjPtr : public CRefCounter
		{
		public:
			//����
			ObjPtr(CRefCounter* pObj)
			{
				m_pObj = pObj;
			}

			//����
			~ObjPtr()
			{
				DEF_RELEASE(m_pObj);
			}

		protected:
			//���ü�������
			CRefCounter*  m_pObj;
		};
	};
}
#endif //_SCOPE_H_
