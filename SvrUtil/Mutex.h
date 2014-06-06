#ifndef _MUTEX_H_
#define _MUTEX_H_

#include "RefCounter.h"

namespace Util
{
	/************************************************************************/
	/* ��������װ����                                                      */
	/************************************************************************/
	class UTIL_API CMutex : public CRefCounter
	{
	public:
		//����(Recursive��ʾ�߳���Ƕ��ʹ��)
		CMutex(Bool bRecursive = false);

		//����
		virtual ~CMutex();

	public:
		//������
		class Scope : public CRefCounter
		{
		public:
			//���������ܱ�ǩ
			enum
			{
				LOCK	= 0x01,
				UNLOCK	= 0x02,
				ALL		= (LOCK | UNLOCK),
			};

		public:
			Scope(CMutex* pMutex, UInt32 iFlag = ALL, const AString& sFile = "", Int32 iLine = 0);

			virtual ~Scope();

		protected:
			//�����
			UInt32		m_iFlag;
			//������
			CMutex*		m_pMutex;
		};

	public:
		//����(�����ȴ�)
		virtual Bool  Lock(const AString& sFile = "", Int32 iLine = 0);

		//����(��������)
		virtual Bool  TryLock(const AString& sFile = "", Int32 iLine = 0);

		//����
		virtual Bool  Unlock();

	protected:
		//������
		PVoid   m_pMutex;

#ifdef _DEBUG
		Bool    m_bLocked;
		AString m_sFile;
		Int32   m_iLine;
		Int32   m_iThread;
#endif
	};

	#define AutoMutex(name, mutex) CMutex::Scope name(mutex, 0x03, __FILE__, __LINE__)
}
#endif //_MUTEX_H_
