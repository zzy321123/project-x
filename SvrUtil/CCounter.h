#ifndef C_COUNTER_H
#define C_COUNTER_H

#include "RefCounter.h"

namespace Util
{
	/************************************************************************/
	/* ������,Ҳ����������ʱ                                                */
	/************************************************************************/
	class UTIL_API CCounter : public CRefCounter
	{
	public:
		//���������
		CCounter(UInt32 iPeriod = 0);

		//����
		virtual ~CCounter();

	public:
		//��������
		DEFINE_PROPERTY(UInt32,	m_iPeriod,  Period);
		//��ǰ����
		DEFINE_PROPERTY(UInt32,	m_iCounter, Counter);
		//��ͣ״̬
		DEFINE_PROPERTY(Bool,	m_bPause,   IsPause);

	public:
		//�����Ƿ�ﵽ��ֵ
		virtual Bool IsFull() const;

		//����(bFull:�Ѽ����������)
		virtual void Reset(bool bFull = false);

		//��ͣ
		virtual void SetPause(bool bPause);
		
		//���Ӽ���
		virtual Bool IncCounter(UInt32 iCounter = 1);
		
		//���ټ���
		virtual void DecCounter(UInt32 iCounter = 1);
	};
}
#endif

