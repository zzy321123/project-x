#ifndef C_COUNTER_H
#define C_COUNTER_H

#include "RefCounter.h"

namespace Util
{
	/************************************************************************/
	/* 计数器,也可以用来计时                                                */
	/************************************************************************/
	class UTIL_API CCounter : public CRefCounter
	{
	public:
		//构造和析构
		CCounter(UInt32 iPeriod = 0);

		//析构
		virtual ~CCounter();

	public:
		//计数上限
		DEFINE_PROPERTY(UInt32,	m_iPeriod,  Period);
		//当前计数
		DEFINE_PROPERTY(UInt32,	m_iCounter, Counter);
		//暂停状态
		DEFINE_PROPERTY(Bool,	m_bPause,   IsPause);

	public:
		//计数是否达到满值
		virtual Bool IsFull() const;

		//重置(bFull:把计数重置最大)
		virtual void Reset(bool bFull = false);

		//暂停
		virtual void SetPause(bool bPause);
		
		//增加计数
		virtual Bool IncCounter(UInt32 iCounter = 1);
		
		//减少计数
		virtual void DecCounter(UInt32 iCounter = 1);
	};
}
#endif

