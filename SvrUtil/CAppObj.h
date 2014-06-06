#ifndef _APPOBJ_H_
#define _APPOBJ_H_

#include "CMsgManager.h"
#include "ProtocolManager.h"
#include "OSOperator.h"

namespace Util
{
	/************************************************************************/
	/* Ӧ�ö����װ,��Ҫ������Ϣ��Э��                                      */
	/************************************************************************/
	class UTIL_API CAppObj : public CRefCounter
	{
	public:
		//����
		CAppObj(const XID& sXid = XID());

		//����
		virtual ~CAppObj();

		//Ӧ�û��������Ա����
		friend class CApp;

		//������Ϣ����
		enum
		{
			HOM_NONE = 0,
			HOM_MSG,
			HOM_PROTO,
		};

		//��Ϣ��¼����
		struct ObjMsg
		{
			UInt32	Type;
			UInt32	Id;
			UInt32  Time;

			ObjMsg(UInt32 iType = 0, UInt32 iId = 0) : Type(iType),Id(iId)
			{
				Time = COSOperator::GetTickCount();
			}
		};
		typedef vector<ObjMsg> MsgTrack;

	public:
		//����Ψһ��ʶID
		virtual XID    GetXid() const;

		//������Ϣ��¼
		virtual Bool   EnableMsgTrack(Bool bEnable);

	protected:
		//��Ϣ��Ӧ
		virtual Bool   OnMessage(const CMsg& sMsg);

		//Э����Ӧ
		virtual Bool   OnProtocol(SID iSid, Protocol* pProto);

		//����
		virtual Bool   OnTick();

	protected:
		//������Ϣ׷�ټ�¼
		virtual void   TrackMsg(UInt32 iType, UInt32 iId);

	protected:
		//����ΨһID
		XID		 m_sXid;
		//�Ƿ�����Ϣ׷��
		Bool	 m_bMsgTrack;
		//������Ϣ�б�
		MsgTrack m_vObjMsg;
	};
}
#endif
