#include "CAppObj.h"

namespace Util
{
	CAppObj::CAppObj(const XID& sXid) : m_sXid(sXid)
	{
		m_bMsgTrack = false;
	}

	CAppObj::~CAppObj()
	{
		m_vObjMsg.clear();
	}

	XID CAppObj::GetXid() const
	{
		return m_sXid;
	}

	Bool CAppObj::OnMessage(const CMsg& sMsg)
	{
		if (m_bMsgTrack)
			TrackMsg(HOM_MSG, sMsg.Msg);

		return false;
	}

	Bool CAppObj::OnProtocol(SID iSid, Protocol* pProto)
	{
		if (m_bMsgTrack && pProto)
			TrackMsg(HOM_PROTO, pProto->GetType());

		return false;
	}

	Bool CAppObj::OnTick()
	{
		return true;
	}

	Bool CAppObj::EnableMsgTrack(Bool bEnable)
	{
		m_bMsgTrack = bEnable;
		return m_bMsgTrack;
	}

	void CAppObj::TrackMsg(UInt32 iType, UInt32 iId)
	{
		if (m_bMsgTrack)
			m_vObjMsg.push_back(ObjMsg(iType, iId));
	}
}
