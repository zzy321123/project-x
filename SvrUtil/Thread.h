#ifndef _THREAD_H_
#define _THREAD_H_

#include "Task.h"
#include "SpinLock.h"

namespace Util
{
	/************************************************************************/
	/* �̲߳�����װ                                                         */
	/************************************************************************/
	class UTIL_API CThread : public CRefCounter
	{
	public:
		//�Ƿ�ȴ��߳�
		CThread(Bool bWaitTask = false);

		//�߳�������
		CThread(CTask* pTask);

		//�̺߳�������
		typedef CTask::PThreadRoutine PThreadRoutine;

		//�̹߳���
		CThread(PThreadRoutine pRoutine);

		//����
		virtual ~CThread();

	public:
		//״̬����
		enum
		{
			STATE_NONE = 0,
			STATE_RUNNING,
			STATE_CLOSING,
			STATE_CLOSED,
		};

	public:
		//�����߳�
		virtual Bool   Start(void* pArgs = 0, Bool bHighPriority = false);

		//�����ȴ������߳�
		virtual Bool   Close();

		//����������,ֻ��ʹ���ڶ������߳�
		virtual Bool   AddTask(CTask* pTask, Bool bFirst = false);

		//��ȡPush��ȥ��������
		virtual UInt64 GetPushTaskCnt() const;

		//��ȡPop������������
		virtual UInt64 GetPopTaskCnt() const;

		//��ȡ�߳�ID
		virtual UInt32 GetThreadId() const;

		//��ȡ�߳�����״̬
		virtual Bool   IsRunning() const;

		//�Ƿ�ȴ��ر�
		virtual Bool   IsClosing() const;

		//����ӵ����
		virtual void   SetOwner(PVoid pPool);

		//��ȡӵ����
		virtual PVoid  GetOwner();

	public:
		//�̺߳�������
		virtual PVoid  RunLoop();

	protected:
		//�ڲ���ʼ��
		virtual Bool   InnerInit();

		//����������
		virtual void   ClearTaskQueue();

		//�����������
		virtual void   RunTaskQueue();

	protected:
		//�߳̾��
		PVoid		    m_pThread;
		//�̺߳�������
		PVoid			m_pArgs;
		//ӵ����(�����̳߳�)
		PVoid			m_pOwner;
		//�߳�ID
		UInt32			m_iId;
		//�߳�״̬
		volatile UInt32	m_iState;
		//������
		volatile Bool   m_bRunning;
		//�߳��������
		TaskQueue		m_qTask;
		//�̺߳�����ڵ�ַ
		PThreadRoutine	m_pRoutine;
		//������л�����
		CSpinLock*	m_pLock;
		//��ӵ���������
		UInt64			m_iPushTaskCnt;
		//ִ�е���������
		UInt64			m_iPopTaskCnt;
	};
}
#endif