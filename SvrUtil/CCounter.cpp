#include "CCounter.h"

namespace Util
{
	CCounter::CCounter(UInt32 iPeriod)
	{
		m_iCounter = 0;
		m_bPause   = true;
		m_iPeriod  = iPeriod;
	}

	CCounter::~CCounter()
	{
	}

	Bool CCounter::IsFull() const
	{ 
		return (m_iCounter >= m_iPeriod) ? true : false; 
	}

	void CCounter::Reset(bool bFull)
	{ 
		m_iCounter = bFull ? m_iPeriod : 0;	
	}

	void CCounter::SetPause(bool bPause)
	{ 
		m_bPause = bPause; 
	}
	
	Bool CCounter::IncCounter(UInt32 iCounter)
	{ 
		if (!m_bPause)
			m_iCounter += iCounter;

		return (m_iCounter >= m_iPeriod) ? true : false;
	}
	
	void CCounter::DecCounter(UInt32 iCounter)
	{ 
		if (m_bPause)
			return;

		if (m_iCounter <= iCounter)
			m_iCounter = 0;
		else
			m_iCounter -= iCounter;
	}
}
