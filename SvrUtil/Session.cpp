#include "Session.h"
#include "ProtocolManager.h"
#include "LoggerManager.h"

namespace Util
{	
	CSession::CSession() : m_pISecurity(0), m_pOSecurity(0), m_pInputBuf(0)
	{
		m_pInputBuf  = new OctetsStream(RCVBUF_SIZE);		
		m_pOutputBuf = new OctetsStream(SNDBUF_SIZE);
		m_pCommonBuf = new OctetsStream(PAGE_SIZE);
	}

	CSession::~CSession()
	{
		if (m_sSocket.IsValid())
			m_sSocket.Close();

		DEF_RELEASE(m_pISecurity);
		DEF_RELEASE(m_pOSecurity);
		DEF_RELEASE(m_pInputBuf);
		DEF_RELEASE(m_pOutputBuf);
		DEF_RELEASE(m_pCommonBuf);		
	}

	Bool CSession::Init(const SocketAddr& sAddr, Int32 iTimeout)
	{
		//�ȹر�
		Close();

		//����ָ����ַ
		if (!m_sSocket.IsValid() && !m_sSocket.InitTcpClient(sAddr, true, iTimeout))
		{
#ifdef _DEBUG
			FmtLog("InitTcpClient Failed, Addr: %s", sAddr.ToString().c_str());
#endif
			return false;
		}
		m_sAddress = sAddr;

		return true;
	}

	Bool CSession::Reconnect()
	{
		//�ȹر�
		Close();	

		//����ָ����ַ
		if (!m_sSocket.IsValid() && !m_sSocket.InitTcpClient(m_sAddress))
			return false;

		return true;
	}

	Bool CSession::Close()
	{
		//�ر��Ѵ����׽���
		if (m_sSocket.IsValid())
			m_sSocket.Close();

		//�ͷż������
		DEF_RELEASE(m_pISecurity);
		DEF_RELEASE(m_pOSecurity);

		//������ݻ�����
		if (m_pInputBuf)
			m_pInputBuf->Clear();

		if (m_pOutputBuf)
			m_pOutputBuf->Clear();

		if (m_pCommonBuf)
			m_pCommonBuf->Clear();		

		return true;
	}	

	Bool CSession::IsValid() const
	{
		return m_sSocket.IsValid();
	}

	OctetsStream* CSession::GetInputBuf()
	{
		return m_pInputBuf;
	}

	CSocket* CSession::GetSocket()
	{
		return &m_sSocket;
	}

	SocketAddr*	CSession::GetAddr()
	{
		return &m_sAddress;
	}

	void CSession::SetISecurity(CSecurity* pSecurity)
	{
		DEF_RELEASE(m_pISecurity);
		if (pSecurity)
		{
			m_pISecurity = pSecurity;
			m_pISecurity->AddRef();
		}
	}

	void CSession::SetOSecurity(CSecurity* pSecurity)
	{
		DEF_RELEASE(m_pOSecurity);
		if (pSecurity)
		{
			m_pOSecurity = pSecurity;
			m_pOSecurity->AddRef();
		}
	}

	Bool CSession::SendRawData(void* pData, Int32 iSize, Bool bFlush)
	{
		if (IsValid())
		{
			//�������
			COctetsStream xOS(pData, iSize);
			if(m_pOSecurity)
				m_pOSecurity->Update(xOS);

			//�����������, ��д��
			if (m_pOutputBuf->EmptyCap() < xOS.Size())
				FlushOutput();

			//������
			Assert(m_pOutputBuf->EmptyCap() >= xOS.Size());

			//��ӵ����ͻ�����
			m_pOutputBuf->Insert(m_pOutputBuf->End(), xOS.Begin(), xOS.End());

			//ʵʱ����
			if (bFlush)
				FlushOutput();

			return true;
		}
		return false;
	}

	Bool CSession::SendProtocol(Protocol* pProto, Bool bFlush)
	{
		if (IsValid())
		{
			//Э����Ϊ������
			COctetsStream xOS;
			if(!pProto->Encode(xOS))
				return false;

			return SendRawData(xOS.Begin(), xOS.Size(), bFlush);
		}
		return false;
	}

	Bool CSession::FlushOutput()
	{
		if (IsValid())
		{
			if (m_pOutputBuf && m_pOutputBuf->Size())
			{
				//��������
				Size_t iSendSize = (Size_t)m_pOutputBuf->Size();
				if (!m_sSocket.Send(m_pOutputBuf->Begin(), iSendSize))
				{
					m_sSocket.Close();
					return false;
				}

				//�Ƴ��ѷ��͵�����
				m_pOutputBuf->Erase(m_pOutputBuf->Begin(), (UInt32)iSendSize);
			}
			return true;
		}
		return false;
	}

	Bool CSession::Tick(UInt32 iPeriod)
	{
		//�ǻ�Ự
		if (!IsValid())
			return false;

		//����ͨ�û�����������ԭʼ����
		m_pCommonBuf->Clear();
		Size_t iRecv = m_pCommonBuf->Capacity();

		//��������
		if (!m_sSocket.Receive(m_pCommonBuf->Begin(), iRecv))
			return false;

		//���ܲ����������뻺����
		if (iRecv > 0)
		{
			//����ͨ�û�����
			m_pCommonBuf->Resize((UInt32)iRecv);

			//����
			if (m_pISecurity)
				m_pISecurity->Update(*m_pCommonBuf);

			//��ӵ����뻺����
			m_pInputBuf->Insert(m_pInputBuf->End(), m_pCommonBuf->Begin(), m_pCommonBuf->Size());	
		}

		return true;
	}

	Bool CSession::DecodeProtocol(Protocol*& pProto)
	{
		try
		{
			pProto = g_ProtocolManager->Decode(*m_pInputBuf);
		}
		catch (CException& rhsExcep)
		{
			//�쳣�˳�
			Print(rhsExcep.GetMsg());

			//�ͷ�Э��
			g_ProtocolManager->ReleaseProto(pProto);

			return false;
		}

		//�Ƴ����뻺���ǰ�οհ�
		m_pInputBuf->RemoveBlank();	
		return true;
	}	
}
