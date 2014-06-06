#include "ProtocolManager.h"
#include "LoggerManager.h"
#include "OSOperator.h"
#include "SysProtocol.h"

namespace Util
{
	SINGLETON_IMPL(ProtocolManager);

	CProtocolManager::Scope::Scope(CProtocol** ppProto) : m_ppProto(ppProto) 
	{
	};

	CProtocolManager::Scope::~Scope() 
	{ 
		CProtocolManager::GetInstance()->ReleaseProto(*m_ppProto); 
	}

	CProtocolManager::CProtocolManager()
	{
		m_bAutoDecode = true;
		m_iSizeLimit  = 0;
		m_iProtoSize  = sizeof(ProtoType) + sizeof(ProtoSize) + sizeof(ProtoCrc);		
	}

	CProtocolManager::~CProtocolManager()
	{		
		ProtocolMap::iterator it = m_mRegister.begin();
		for (; it != m_mRegister.end(); ++it)
		{
			DEF_RELEASE(it->second);
		}
		m_mRegister.clear();
	}	

	Bool CProtocolManager::RegSysProtocol()
	{
		//注册系统内部协议
		REGISTER_PROTO(SysProtocol::Sys_MsgNotify);
		REGISTER_PROTO(SysProtocol::Sys_ErrNotify);
		REGISTER_PROTO(SysProtocol::Sys_HeartBeat);
		REGISTER_PROTO(SysProtocol::Sys_HeartBreak);
		REGISTER_PROTO(SysProtocol::Sys_SessionStart);
		REGISTER_PROTO(SysProtocol::Sys_SessionBreak);
		REGISTER_PROTO(SysProtocol::Sys_CloseSession);
		REGISTER_PROTO(SysProtocol::Sys_LogMsg);
		REGISTER_PROTO(SysProtocol::Sys_ProfReq);
		REGISTER_PROTO(SysProtocol::Sys_ProfInfo);
		REGISTER_PROTO(SysProtocol::Sys_RefuseConn);
		REGISTER_PROTO(SysProtocol::Sys_CltPing);
		REGISTER_PROTO(SysProtocol::Sys_SvrPong);
		REGISTER_PROTO(SysProtocol::Sys_Octets);

		return true;
	}

	Bool CProtocolManager::CheckDecodeProtocol(const COctetsStream& xOS, UInt32* pChunkSize)
	{
		if (xOS.AvailableSize() >= m_iProtoSize)
		{
			ProtoSize iSize = *((ProtoSize*)((Char*)xOS.AvailableData() + sizeof(ProtoType)));
			if (xOS.AvailableSize() >= m_iProtoSize + iSize)
			{
				if (pChunkSize)
					*pChunkSize = m_iProtoSize + iSize;

				return true;
			}
		}
		return false;
	}

	Bool CProtocolManager::SetSizeLimit(UInt32 iSize)
	{
		m_iSizeLimit = iSize;
		return true;
	}

	Bool CProtocolManager::CheckSizeLimit(UInt32 iSize) const
	{
		if (!m_iSizeLimit || m_iSizeLimit >= iSize)
			return true;

		return false;
	}

	UInt32 CProtocolManager::GetProtoHeaderSize() const
	{
		return m_iProtoSize;
	}

	Bool CProtocolManager::AnalyseProtocolHeader(const COctetsStream& xOS, ProtoType& iType, ProtoSize& iSize, ProtoCrc& iCrc)
	{
		if (xOS.AvailableSize() >= m_iProtoSize)
		{
			iType = *((ProtoType*)xOS.AvailableData());
			iSize = *((ProtoSize*)((Char*)xOS.AvailableData() + sizeof(ProtoType)));
			iCrc  = *((ProtoCrc*)((Char*)xOS.AvailableData() + sizeof(ProtoType) + sizeof(ProtoSize)));
			return true;			
		}
		return false;
	}

	Bool CProtocolManager::ReadProtocolHeader(COctetsStream& xOS, ProtoType& iType, ProtoSize& iSize, ProtoCrc& iCrc)
	{
		xOS.Pop(iType);
		xOS.Pop(iSize);
		xOS.Pop(iCrc);
		return true;
	}

	Bool CProtocolManager::WriteProtocolHeader(COctetsStream& xOS, ProtoType iType, ProtoSize iSize, ProtoCrc iCrc)
	{
		xOS.Push(iType);
		xOS.Push(iSize);
		xOS.Push(iCrc);
		return true;
	}

	CProtocol*  CProtocolManager::Decode(COctetsStream& rhsOS)
	{
		if (!CheckDecodeProtocol(rhsOS))
			return 0;

		//创建协议,开始解析
		ProtoType iType	= *((ProtoType*)rhsOS.AvailableData());
		Protocol* pProto = CreateProtocol(iType);
		if (pProto && !pProto->Decode(rhsOS))
		{
			ReleaseProto(pProto);
		}
		return pProto;
	}

	Bool CProtocolManager::Register(ProtoType iType, CProtocol* pProto)
	{		
		ProtocolMap::iterator it = m_mRegister.find(iType);
		Assert(it == m_mRegister.end());
		if (pProto && it == m_mRegister.end())
		{
			pProto->AddRef();
			m_mRegister[iType] = pProto;	
			return true;
		}
		return false;
	}

	Bool CProtocolManager::CheckProtocolLegal(ProtoType iType)
	{
		return m_mRegister.find(iType) != m_mRegister.end();
	}

	UInt32 CProtocolManager::GetRegProtoIds(vector<ProtoType>& vProtoIds, vector<UInt8>* pProtoDrive)
	{
		vProtoIds.clear();

		ProtocolMap::iterator it = m_mRegister.begin();
		for (;it!=m_mRegister.end();it++)
		{
			vProtoIds.push_back(it->first);
			if (pProtoDrive)
				pProtoDrive->push_back(it->second->GetDrive());
		}

		return (UInt32)vProtoIds.size();
	}

	Bool  CProtocolManager::IsAutoDecode() const
	{
		return m_bAutoDecode;
	}

	void  CProtocolManager::SetAutoDecode(Bool bAuto)
	{
		m_bAutoDecode = bAuto;
	}

	CProtocol*  CProtocolManager::CreateProtocol(ProtoType iType)
	{
		ProtocolMap::iterator it = m_mRegister.find(iType);
		if (it == m_mRegister.end())
		{
			FmtError("Protocol Unregister, Type: %d", iType);
			T_Exception("Protocol Unregister.");
			return 0;
		}

		CProtocol* pProto = m_mRegister[iType]->Clone();
		return pProto;
	}

	Bool CProtocolManager::ReleaseProto(CProtocol*& pProto)
	{
		if (pProto)
		{
			DEF_RELEASE(pProto);
			return true;
		}
		return false;
	}



}
