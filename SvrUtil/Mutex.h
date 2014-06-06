#ifndef _MUTEX_H_
#define _MUTEX_H_

#include "RefCounter.h"

namespace Util
{
	/************************************************************************/
	/* 互斥对象封装操作                                                      */
	/************************************************************************/
	class UTIL_API CMutex : public CRefCounter
	{
	public:
		//构造(Recursive表示线程内嵌套使用)
		CMutex(Bool bRecursive = false);

		//析构
		virtual ~CMutex();

	public:
		//区域锁
		class Scope : public CRefCounter
		{
		public:
			//区域锁功能标签
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
			//锁标记
			UInt32		m_iFlag;
			//锁对象
			CMutex*		m_pMutex;
		};

	public:
		//锁定(阻塞等待)
		virtual Bool  Lock(const AString& sFile = "", Int32 iLine = 0);

		//锁定(立即返回)
		virtual Bool  TryLock(const AString& sFile = "", Int32 iLine = 0);

		//解锁
		virtual Bool  Unlock();

	protected:
		//互斥锁
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
