#ifndef BASE_GATEPROXY_H
#define BASE_GATEPROXY_H

#include "BaseGateDef.h"

namespace Util
{
	/************************************************************************/
	/* ������ƥ������Ӷ˷�װ                                               */
	/************************************************************************/
	class GATE_API BaseGateProxy : public CRefCounter
	{
	public:
		//����
		BaseGateProxy();

		//����
		virtual ~BaseGateProxy();

		//�߳�˽�о��
		typedef map<UInt32, CZmq*>	ThreadProxyMap;

		//ע��Э������
		typedef map<ProtoType, UInt8>   ProtoTypeDriveMap;

	public:
		//��ʼ��
		virtual Bool  Init(const AString& sAddr, UInt32 iSvrId, UInt32 iConnLimit = 0);

		//������Ϣ
		virtual Bool  Update(Int32 iTimeout = 0);

	public:
		//ע��Э������
		virtual Bool  RegisterProtoType(const ProtoTypeDriveMap& mTypeDrive);

		//ע������߳�
		virtual Bool  RegisterThreads(const vector<UInt32>& vThreads);

		//����Э��
		virtual Bool  SendProtocol(SID iSid, CProtocol* pProto);

		//����Э��
		virtual Bool  SendProtocol(const SvrMsgHeader& sHeader, CProtocol* pProto);		

		//��������
		virtual Bool  SendRawData(SID iSid, const OctetsStream* pData);

		//��������
		virtual Bool  SendRawData(const SvrMsgHeader& sHeader, const OctetsStream* pData);	

		//�㲥Э��
		virtual Bool  BroadcastProto(const vector<SID>& vSid, CProtocol* pProto);

		//����Э��(ֻ��������Э��, һ�㲻����ʹ��)
		virtual Bool  RecvProtocol(GateMsgHeader& sHeader, CProtocol*& pProto, Int32 iTimeout = -1);

		//�رջỰ
		virtual Bool  CloseSession(SID iSid);

	protected:
		//���ӵ����ػص�
		virtual Bool  OnProxyConnected();

		//����Э��ص�(�߳�ģʽ��,�̵߳���), ����true����ʾЭ�鱻Ӧ�ý���, Ӧ�ø����ͷ�, ���������ͷ�
		virtual Bool  OnRecvProtocol(const GateMsgHeader& sHeader, CProtocol* pProto);

		//�����ضϿ����ӻص�
		virtual Bool  OnProxyDisconnect();

	protected:
		//�����������״̬(���߳�ģʽ�����ֶ�����)
		virtual Bool  CheckProxyState();

		//����֪ͨ(SIDΪĿ�����ID, Ϊ0����ʾ��ϵͳ��)
		virtual Bool  SendNotify(const GateNotify& sNotify, const OctetsStream* pExtData = 0);

		//�����ط���Ĭ��ע����Ϣ
		virtual Bool  SendRegsterMsg();

		//��ȡ�߳�˽��ͨ�Ŷ���
		CZmq*		  GetThreadProxy();		

	protected:
		//ͨ�û�����
		OctetsStream*		m_pOctets;
		//���ص�ַ
		AString				m_sGateAddr;
		//�����ʶID
		UInt32				m_iSvrId;
		//�������������
		UInt32				m_iConnLimit;
		//��Ϣͨ��
		CZmq*				m_pProxyZmq;
		//�Ƿ����������
		UInt32				m_iProxyState;
		//�߳����ݽ�����
		CZmq*				m_pCollectZmq;
		//�߳�˽��ͨ�Ŷ���
		ThreadProxyMap		m_mThreadProxy;
		//ע��Э���б�
		ProtoTypeDriveMap	m_mProtoTypeDrive;
	};
}
#endif
