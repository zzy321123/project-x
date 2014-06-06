#ifndef BASE_GATEDEF_H
#define BASE_GATEDEF_H

#include "SvrUtil.h"

namespace Util
{
#pragma pack(push, 1)
	//�����̷߳��͵�����ͷ(Args��Ҫ�������ش����������)
	struct ThreadMsgHeader 
	{
		enum
		{
			MSG_NORMAL = 0,
		};

		//��Ӧ������ID
		UInt32	SvrId;
		//�ỰID
		SID		Sid;
		//��������(0 ��ʾ��ͨ��Ϣ, ����������)
		Int8    Type;
		//���ݲ���(>0 ��ʾЭ��ID, <=0 ��ʾ��Ϣ)
		Int32	Args;

		ThreadMsgHeader(UInt32 iSvrId = 0, SID iSid = 0, Int8 iType = 0, Int32 iArgs = 0) : SvrId(iSvrId), Sid(iSid), Type(iType), Args(iArgs) {};

		ThreadMsgHeader& operator = (const ThreadMsgHeader& sHeader)
		{
			if (this != &sHeader)
			{
				SvrId = sHeader.SvrId;
				Sid   = sHeader.Sid;
				Type  = sHeader.Type;
				Args  = sHeader.Args;
			}
			return *this;
		}
	};

	//���ط��͵�����ͷ
	struct GateMsgHeader
	{
		//�ỰID
		SID	 Sid;

		GateMsgHeader(SID iSid = 0) : Sid(iSid) {};

		GateMsgHeader& operator = (const GateMsgHeader& sHeader)
		{
			if (this != &sHeader)
			{
				Sid   = sHeader.Sid;
			}
			return *this;
		}
	};

	//���������͵�����ͷ
	struct SvrMsgHeader
	{
		//�ỰID
		SID	 Sid;

		SvrMsgHeader(SID iSid = 0) : Sid(iSid) {};

		SvrMsgHeader& operator = (const SvrMsgHeader& sHeader)
		{
			if (this != &sHeader)
			{
				Sid   = sHeader.Sid;
			}
			return *this;
		}
	};

	//�ڲ�֪ͨ����
	struct GateNotify
	{
		//֪ͨ���ͱ��
		enum
		{
			NOTIFY_UNKNOWN = 0,
			NOTIFY_SESSION_CONNECT,		//�½��Ự����
			NOTIFY_SESSION_DISCONN,		//�Ự���ӶϿ�
			NOTIFY_SESSION_CLOSE,		//�����رջỰ
			NOTIFY_SERVICE_EXIT,		//�˳����ط���
			NOTIFY_SERVICE_ATTACH,		//�������
			NOTIFY_SERVICE_DETACH,		//����ж��
			NOTIFY_PROTO_REGISTER,		//Э��ע��
			NOTIFY_PROTO_BROADCAST,		//Э��㲥
		};

		//Ĭ�Ϲ���
		GateNotify(UInt8 iType = NOTIFY_UNKNOWN) : Type(iType) {};

		//֪ͨ����
		UInt8  Type;

		//֪ͨ����
		union
		{
			//����������
			struct
			{
				//���Ӷ��׽���
				SOCKET	Handle;
				//�Ự��ַ
				Char	Address[IPV_LENGTH];
				//��ַ����
				UInt8	AddrLen;
				//�󶨷�����ID
				UInt32	SvrId;
			}eConnect;				

			//���ӻỰ�Ͽ�(����)
			struct
			{
				//�ỰID
				SID		Sid;
				//�Ự��ַ
				Char	Address[IPV_LENGTH];
				//��ַ����
				UInt8	AddrLen;
			}eDisConn;

			//�ر����ӻỰ(����)
			struct
			{
				//������ID
				UInt32  SvrId;
				//�ỰID
				SID		Sid;				
			}eClose;

			//�������
			struct 
			{
				//������ID
				UInt32  SvrId;
				//���������
				UInt32  ConnLimit;
			}eAttach;

			//����ж��
			struct 
			{
				//������ID
				UInt32  SvrId;
			}eDetach;
		};
	};
#pragma pack(pop)
}
#endif
