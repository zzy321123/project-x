#ifndef _SOCKETPAIR_H_
#define _SOCKETPAIR_H_

#include "Socket.h"
#include "Mutex.h"

namespace Util
{
	/************************************************************************/
	/* Socket�Բ�����װ,��Ҫ�����̼߳���Ϣ֪ͨ									*/
	/************************************************************************/
	class UTIL_API CSocketPair : public CRefCounter
	{		
	public:		
		//����
		CSocketPair();

		//����
		virtual ~CSocketPair();

	public:
		//�����׽��ֶ�
		virtual Bool	Create(Bool bThreadSafe = false);

		//�Ƿ���Ч
		virtual Bool    IsValid() const;

		//������Ϣ
		virtual Bool	Send(const void* pData, Size_t& iSize, Int32 iFlags = 0);

		//������Ϣ
		virtual Bool	Receive(void* pData, Size_t& iSize, Int32 iFlags = 0);

		//����δ���¼�
		virtual Bool    Clear();

		//�ر�
		virtual Bool    Close();

	public:
		//��ȡдFD
		virtual SOCKET	GetWriteFd() const;

		//��ȡ��FD
		virtual SOCKET  GetReadFd() const;

	protected:
		//Socket���
		SOCKET		m_hSocket[2];
		//���Ͷ˵��̰߳�ȫ��
		CMutex*		m_pMutex;
		//������
		UInt64		m_iSend;
		//������
		UInt64		m_iRecv;
	};	

	////Э�����ͼ�㶨��
	typedef CSocketPair SocketPair;
}
#endif
