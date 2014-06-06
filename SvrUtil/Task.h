#ifndef _TASK_H
#define _TASK_H

#include "RefCounter.h"

namespace Util
{
	/************************************************************************/
	/* 线程任务对象，每个对象表示一个线程能执行的任务				            */
	/************************************************************************/
	class UTIL_API CTask : public CRefCounter 
	{
	public:
		//任务函数定义
		typedef PVoid (*PThreadRoutine)(void* pArgs);

	public:
		//构造
		CTask(PThreadRoutine pRoutine = 0, PVoid pParam = 0);

		//析构
		virtual ~CTask();

	public:
		//获取回调参数
		virtual PVoid GetParam();

		//执行线程任务
		virtual PVoid Run(void* pArgs = 0);

		//关闭任务,释放资源
		virtual Int32 Close();

	protected:
		//任务回调入口
		PThreadRoutine m_pRoutine;
		//任务回调参数
		PVoid m_pParam;
	};
	//任务队列定义
	typedef deque<CTask*> TaskQueue;
}
#endif //_TASK_H
