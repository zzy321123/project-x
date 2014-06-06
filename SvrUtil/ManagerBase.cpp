#include "ManagerBase.h"

namespace Util
{
	CManagerBase::CManagerBase() : m_bRunning(false)
	{
	}

	CManagerBase::~CManagerBase()
	{
	}

	Bool CManagerBase::Start()
	{
		if ( !m_bRunning )
		{
			m_bRunning = true;
			return true;
		}
		return false;
	}

	Bool CManagerBase::IsRunning() const
	{
		return m_bRunning;
	}

	Bool CManagerBase::Tick(UInt32 iPeriod)
	{
		return m_bRunning;
	}

	Bool CManagerBase::Stop()
	{
		if ( m_bRunning )
		{
			m_bRunning = false;
			return true;
		}
		return false;
	}
}
