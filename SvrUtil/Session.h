#ifndef C_SESSION_H
#define C_SESSION_H

#include "Security.h"
#include "Socket.h"
#include "Protocol.h"

namespace Util
{
	/************************************************************************/
	/* ����Ự�����װ                                                     */
	/************************************************************************/
	class UTIL_API CSession : public CRefCounter
	{
	public:
		//����
		CSession();

		//����
		virtual ~CSession();

	public:
		//��ʼ��
		virtual Bool	Init(const SocketAddr& sAddr, Int32 iTimeout = DEFAULT_TIMEOUT);		

		//���»Ự,��ȡ����IO����(����false��ʾ����Ͽ�)
		virtual Bool    Tick(UInt32 iPeriod);

		//�����뻺�����н�����Э��
		virtual Bool    DecodeProtocol(Protocol*& pProto);

		//��������(��Ҫ�������üӽ������)
		virtual Bool    Reconnect();

		//�رջỰ
		virtual Bool	Close();

	public:
		//���ö���������
		virtual void	SetISecurity(CSecurity* pSecurity);

		//����д���������
		virtual void	SetOSecurity(CSecurity* pSecurity);

		//����ԭʼ����
		virtual Bool	SendRawData(void* pData, Int32 iSize, Bool bFlush = true);

		//����Э��
		virtual Bool	SendProtocol(Protocol* pProto, Bool bFlush = true);	

		//д�����ͻ�����(֡����ʱ����)
		virtual Bool	FlushOutput();

		//��ȡ����Buffer
		OctetsStream*	GetInputBuf();

		//��ȡ�ỰSocket
		CSocket*		GetSocket();

		//��ȡ�Զ˵�ַ
		SocketAddr*		GetAddr();

	protected:
		//�Ƿ���Ч
		virtual Bool    IsValid() const;

	protected:
		//�Ự�׽���
		CSocket			m_sSocket;
		//��ַ
		SocketAddr		m_sAddress;	
		//���ܶ���
		CSecurity*		m_pISecurity;
		//���ܶ���
		CSecurity*		m_pOSecurity;
		//���뻺����
		OctetsStream*	m_pInputBuf;
		//���������
		OctetsStream*	m_pOutputBuf;
		//��ʱ������
		OctetsStream*	m_pCommonBuf;
	};
}
#endif
