#ifndef _THREADPOOL_H
#define _THREADPOOL_H

#include "Thread.h"

namespace Util
{
	/************************************************************************/
	/* �̳߳ز�����װ														*/
	/************************************************************************/
	class UTIL_API CThreadPool : public CRefCounter
	{
	public:
		//�̳߳ع���
		CThreadPool();

		//�̳߳�����
		virtual ~CThreadPool();

		//�߳��б����Ͷ���
		typedef vector<CThread*> ThreadList;

	public:
		//��ʼ��(PoolSize��ʾ�߳���, PoolQueue��ʾ�Ƿ�ʹ�ó��������)
		virtual Bool   InitPool(Int32 iPoolSize = 2, Bool bPoolQueue = true);

		//���ִ������(iThread�����ڷ�PoolQueueģʽ��ָ���߳�ִ��)
		virtual Bool   AddTask(CTask* pTask, Int32 iThreadIdx = -1, Bool bFirst = false);

		//��ʼִ��
		virtual Bool   Start();

		//��������߳���
		virtual Int32  GetThreadNum();

		//��ȡ�߳�ID
		virtual Int32  GetThreadId(Int32 iIdx);

		//���������߳�
		virtual void   Close();

		//��ѯ�Ƿ�ʼ����(������)
		virtual Bool   IsRunning() const;

		//��ȡPush��ȥ��������
		virtual UInt64 GetPushTaskCnt() const;

		//��ȡPop������������
		virtual UInt64 GetPopTaskCnt() const;

		//�������е�һ������(����ֵ��ʾ�Ƿ��������)
		virtual Bool   PopTask(CTask** ppTask);

	protected:
		//�߳��б�
		ThreadList		m_vThread;
		//�̳߳ص��߳���Ŀ
		Int32			m_iThreadNum;
		//�Ƿ�ӵ���Լ����������
		Bool			m_bPoolQueue;
		//�̳߳�����״̬
		volatile Bool   m_bRunning;		
		//��ǰʹ�õ��߳�����
		volatile UInt32	m_iTurnIdx;
		//�߳��������
		TaskQueue		m_qTask;
		//������л�����
		CSpinLock*		m_pLock;
		//��ӵ���������
		UInt64			m_iPushTaskCnt;
		//ִ�е���������
		UInt64			m_iPopTaskCnt;
	};
}
#endif //MY_THREADPOOL_H
