#ifndef _RWLOCK_H_
#define _RWLOCK_H_

#include "RefCounter.h"

namespace Util
{
	/************************************************************************/
	/* ��д��������װ                                                       */
	/************************************************************************/
	class UTIL_API CRWLock : public CRefCounter
	{
	public:
		//����
		CRWLock();

		//����
		virtual ~CRWLock();

		//��������
		enum
		{
			READ  = 0x01,
			WRITE = 0x02,
		};

	public:
		//������
		class UTIL_API Scope : public CRefCounter
		{
		public:
			Scope(CRWLock* pRWLock, UInt32 iType, const AString& sFile = "", Int32 iLine = 0) ;

			virtual ~Scope();

		protected:
			CRWLock*  m_pRWLock;
		};

	public:
		//����(�����ȴ�)
		virtual Bool  Lock(UInt32 iType, const AString& sFile = "", Int32 iLine = 0);

		//����(��������)
		virtual Bool  TryLock(UInt32 iType, const AString& sFile = "", Int32 iLine = 0);

		//����
		virtual Bool  Unlock();

	protected:
		PVoid   m_pRWLock;
	};

	#define AutoRWLock(name, rwlock, type) CRWLock::Scope name(rwlock, type, __FILE__, __LINE__)
}
#endif
