#include "Task.h"

namespace Util
{
	CTask::CTask(PThreadRoutine pRoutine, PVoid pParam) : m_pRoutine(pRoutine), m_pParam(pParam)
	{
	}

	CTask::~CTask()
	{
	}

	PVoid CTask::GetParam()
	{
		return m_pParam;
	}

	PVoid CTask::Run(void* pArgs)
	{
		if (m_pRoutine)
			return (*m_pRoutine)(pArgs? pArgs : m_pParam);
		
		return 0;
	}

	Int32 CTask::Close()
	{
		return 0;
	}
}
