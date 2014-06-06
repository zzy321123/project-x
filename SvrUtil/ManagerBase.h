#ifndef _MANAGERBASE_H_
#define _MANAGERBASE_H_

#include "RefCounter.h"

namespace Util
{
	/************************************************************************/
	/* ����������Ļ��ඨ��													*/
	/************************************************************************/
	class UTIL_API CManagerBase : public CRefCounter
	{
	public:
		//����
		CManagerBase();

		//����
		virtual ~CManagerBase();

	public:
		//����������
		virtual Bool Start();

		//���¹�����
		virtual Bool Tick(UInt32 iPeriod);

		//�жϹ������Ƿ�����
		virtual Bool IsRunning() const;
		
		//ֹͣ����������,������Stop�������Դ�ͷ�
		virtual Bool Stop();

	protected:
		volatile Bool m_bRunning;
	};
}

#endif //_MANAGERBASE_H_
