#ifndef BASE_GATEWAY_H
#define BASE_GATEWAY_H

#include "BaseGateDef.h"
#include "BaseGateThread.h"
#include "CProfiler.h"
#include "BaseGateSecurity.h"

namespace Util
{
	/************************************************************************/
	/* ���ط�����������װ(����libevent�ı�Ե�¼�����ģ��)                   */
	/************************************************************************/
	class GATE_API BaseGateway : public CRefCounter
	{
	public:
		//����
		BaseGateway();

		//����
		virtual ~BaseGateway();

		//�����б�
		struct Service 
		{
			UInt32  SvrId;
			SOCKET  SvrFd;
			UInt32  ConnCap;			
			UInt32  CurCap;

			Service(UInt32 iSvrId = 0, SOCKET iSvrFd = INVALID_SOCKET, UInt32 iConnCap = 0) : SvrId(iSvrId), SvrFd(iSvrFd), ConnCap(iConnCap), CurCap(0) {}
		};
		//��ǰ���صķ������б�
		typedef vector<Service>		ServiceVec;
		//�ỰID�ͷ�����ID��ӳ��
		typedef map<SID, UInt32>	SidBindMap;

		//����ģʽ
		enum
		{
			GATEMODE_ECHO = 0,					//����ģʽ
			GATEMODE_SINGLE_SERVICE,			//����������ģʽ
			GATEMODE_MULTI_SERVICE_STOCK,		//������������ģʽ
			GATEMODE_MULTI_SERVICE_BALANCE,		//����������ؾ���ģʽ
		};

	public:
		//��ʼ�����ط���
		virtual Bool    Init(const AString& sFrontend, const AString& sBackend, Int32 iThread = 4, Int32 iGateMode = GATEMODE_SINGLE_SERVICE);

		//��������
		virtual Bool    Run();

		//�رշ���(֪ͨ����,������)
		virtual Bool    Stop();		

	public:
		//��ȡ�߳���
		virtual Int32   GetThreadNum() const;

		//��ȡ�߳�ID
		virtual Int32   GetThreadId(Int32 iIdx) const;

		//������ѭ����������
		virtual Bool    SetBlocking(Bool bBlocking = true);

		//���ûỰ�����С
		virtual void    SetBufSize(Int32 iBufSize);

		//��ȡ�Ự�����С
		virtual Int32   GetBufSize() const;	

		//������ȫ����ֱ�����е�����ģʽ
		virtual Bool    SetWaitIdle(Bool bWaitIdle);

		//�Ƿ�Ϊ��������ֱ���������ģʽ
		virtual Bool    IsWaitIdle() const;

		//�ж�����״̬
		virtual Bool    IsRunning() const;		

		//���ûỰ��ʱʱ��
		virtual void    SetSessionTimeout(Int32 iTimeout);

		//��ȡ�Ự��ʱʱ��
		virtual Int32   GetSessionTimeout() const;

		//����IO�������ӽ������
		virtual Bool    TurnGateSecurity();

		//�������ܼ�����
		virtual Bool    TurnOnProfiler(const AString& sAddr);

		//�����������(flash 843�˿���֤)
		//virtual Bool    TurnOnCrossDomain();	

	public:
		//���������߳�
		virtual Bool    CreateGateThread(BaseGateThread*& pThread);

		//�����Ự�������
		virtual Bool    CreateISecurity(CSecurity*& pSecurity);

		//�����Ự�������
		virtual Bool    CreateOSecurity(CSecurity*& pSecurity);

		//��ȡ���ܼ�����
		CProfiler*   GetProfiler();

		//��ȡ�߳�ZMQ�ķ����ַ
		virtual AString GetThreadZmqAddr() const;

		//��ȡ��˷������ZMQ��ַ
		virtual AString GetServerZmqAddr() const;		

		//�Ự����У��(��GateThread���)
		virtual Bool    TrustSession(void* pSession, OctetsStream* pIBuffer);

		//�رջỰ
		virtual Bool    CloseSession(SID iSid, UInt32 iSvrId = 0);

	protected:
		//���������̸߳�ʽ������
		virtual Bool    RecvThreadMsg(UInt32& iThread, ThreadMsgHeader& sHeader, OctetsStream* pOctets);		

		//�������̷߳���֪ͨ��Ϣ
		virtual Bool    SendThreadMsg(UInt32 iThread, const GateMsgHeader& sHeader, void* pData, Size_t iSize, const OctetsStream* pExtData = 0);

		//���պ�˷����ʽ������
		virtual Bool    RecvServerMsg(UInt32& iSvrId, SvrMsgHeader& sHeader, OctetsStream* pOctets);

		//���պ�˷����ʽ������(Я������)
		virtual Bool    RecvServerExtData(OctetsStream* pOctets);

		//���˷�����֪ͨ��Ϣ
		virtual Bool    SendServerMsg(UInt32 iSvrId, const GateMsgHeader& sHeader, void* pData, Size_t iSize);		

	protected:
		//����������
		virtual Bool    OnSessionAccept();

		//ThreadZmq�ɶ��¼�(�������������̵߳�����)
		virtual Bool    OnGateThreadEvent();

		//���տͻ���Э��ص�(��OnGateThreadEvent����, ����falseֱ�ӻ�CloseSession)
		virtual Bool    OnSessionProtocol(SID iSid, const OctetsStream* pOctets);

		//ServerZmq�ɶ��¼�
		virtual Bool    OnGateServerEvent();

		//MonitorZmq�ɶ��¼�
		virtual Bool    OnGateMonitorEvent();

		//���ؿ���״̬����
		virtual Bool    OnGateIdleEvent();

		//�رշ���(�����ȴ�����)
		virtual Bool    OnGatewayClose();		

	protected:
		//��˷������ӵ�����
		virtual Bool    OnServerConnected(SOCKET hSocket);

		//��˷�������ضϿ�
		virtual Bool    OnServerDisConnect(SOCKET hSocket);

	protected:
		//ע��������, ���ذ󶨷�����ID
		virtual Bool    RegConnection(const SocketAddr& sAddr, UInt32& iSvrId, Int32& iErrCode);

		//���ؾܾ�����
		virtual Bool    OnGateRefuseConn(Int32 iReason, CSocket& sSocket, const SocketAddr* pAddr = 0);

		//�Ự���ص�������
		virtual Bool    OnSessionAttachServer(SID iSid, UInt32 iSvrId);

		//�Ự�ӷ�����ж��
		virtual Bool    OnSessionDetachServer(SID iSid, UInt32 iSvrId);

	protected:		
		//�¼��߳�
		BaseGateThread** m_ppThread;		
		//����ģʽ
		Int32			 m_iGateMode;
		//�ỰID���󶨵ķ���ID
		SidBindMap		 m_mSidBind;
		//�����߳���
		Int32			 m_iThread;
		//�����߳�ID
		UInt32			 m_iTurnIdx;
		//�����ỰID
		UInt32			 m_iBaseSid;
		//�ỰBuffer��С
		Int32			 m_iBufSize;
		//�Ự��ʱʱ��
		Int32			 m_iTimeout;
		//��Ϣ����Buffer
		OctetsStream*	 m_pOctets;
		//�������׽���
		CSocket		 m_sSocket;
		//���صķ���ID
		ServiceVec		 m_vService;
		//�������߳�ͨ�ŵ�ZMQ
		CZmq*			 m_pThreadZmq;
		//�ͺ�˷���ͨ�ŵ�ZMQ
		CZmq*			 m_pServerZmq;
		//��˷���ZMQ������
		CZmq*		 m_pMonitorZmq;
		//���ܼ�����
		CProfiler*	 m_pProfiler;
		//�����ӽ���ģʽ
		Bool			 m_bSecurity;
		//ѭ������״̬
		Bool			 m_bIdle;
		//�ȴ�ֱ������(ƽ�ⵥ����������ʱ��)
		Bool			 m_bWaitIdle;
		//��������ѭ���Ƿ�����(Ĭ������)
		Bool			 m_bBlocking;
		//����״̬
		volatile Bool    m_bRunning;

	protected:
		//��������߳�
		CThread*		 m_pCrossDomain;
	};
}
#endif
