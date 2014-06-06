#ifndef _TASK_H
#define _TASK_H

#include "RefCounter.h"

namespace Util
{
	/************************************************************************/
	/* �߳��������ÿ�������ʾһ���߳���ִ�е�����				            */
	/************************************************************************/
	class UTIL_API CTask : public CRefCounter 
	{
	public:
		//����������
		typedef PVoid (*PThreadRoutine)(void* pArgs);

	public:
		//����
		CTask(PThreadRoutine pRoutine = 0, PVoid pParam = 0);

		//����
		virtual ~CTask();

	public:
		//��ȡ�ص�����
		virtual PVoid GetParam();

		//ִ���߳�����
		virtual PVoid Run(void* pArgs = 0);

		//�ر�����,�ͷ���Դ
		virtual Int32 Close();

	protected:
		//����ص����
		PThreadRoutine m_pRoutine;
		//����ص�����
		PVoid m_pParam;
	};
	//������ж���
	typedef deque<CTask*> TaskQueue;
}
#endif //_TASK_H
