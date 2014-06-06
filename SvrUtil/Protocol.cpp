#include "Protocol.h"
#include "OSOperator.h"
#include "LoggerManager.h"
#include "ProtocolManager.h"

namespace Util
{
	CProtocol::CProtocol(ProtoType iType, UInt8 iDrive) : m_iType(iType), m_iSize(0), m_iCrc(0), m_iDrive(iDrive)
	{
	}

	CProtocol::~CProtocol()
	{
	}

	ProtoType CProtocol::GetType() const
	{
		return m_iType;
	}	

	ProtoSize CProtocol::GetSize() const
	{
		return m_iSize;
	}

	ProtoCrc CProtocol::GetCrc() const
	{
		return m_iCrc;
	}

	UInt8 CProtocol::GetDrive() const
	{
		return m_iDrive;
	}

	Bool CProtocol::GetDecodeOS(OctetsStream*& pOS)
	{
		pOS = &m_sDecode;
		return true;
	}

	Bool CProtocol::Encode(COctetsStream& rhsOS)
	{
		try
		{
			COctetsStream sEncode;
			sEncode << *this;

			m_iSize = sEncode.Size();
			m_iCrc  = COSOperator::CalcCrc((UChar*)sEncode.Begin(), m_iSize);

			g_ProtocolManager->WriteProtocolHeader(rhsOS, m_iType, m_iSize, m_iCrc);

			rhsOS.Push(sEncode.Begin(),m_iSize);
		}
		catch (CException& rhsExcep)
		{
			FmtError(rhsExcep.GetMsg().c_str());
			return false;
		}

		return true;
	}

	Bool CProtocol::Decode(COctetsStream& rhsOS)
	{
		//检测协议完整性
		if (!g_ProtocolManager->CheckDecodeProtocol(rhsOS))
			return false;

		m_iSize = 0;
		m_iCrc  = 0;
		m_sDecode.Clear();

		ProtoType iType = 0;
		ProtoCrc  iCrc  = 0;
		try
		{		
			//读协议头
			rhsOS >> COctetsStream::TransBegin;

			g_ProtocolManager->ReadProtocolHeader(rhsOS, iType, m_iSize, m_iCrc);
			
			if ((ProtoSize)rhsOS.AvailableSize() >= m_iSize)
			{
				//读协议数据
				if (m_iSize)
				{
					m_sDecode.Reserve(m_iSize);
					rhsOS.Pop((void*)m_sDecode.Begin(), m_iSize);
					m_sDecode.Resize(m_iSize);
					iCrc = COSOperator::CalcCrc((UChar*)m_sDecode.Begin(),m_iSize);
				}

				//读取完全,提交
				rhsOS >> COctetsStream::TransCommit;
			}
			else
			{
				//数据包不完全,回滚
				rhsOS >> COctetsStream::TransRollback;
				return false;
			}
		}
		catch (CException& rhsExcep)
		{
			rhsOS >> COctetsStream::TransRollback;
			FmtError(rhsExcep.GetMsg().c_str());
			return false;
		}

		//数据校验
		if(m_iType != iType)
		{
			FmtError("Protocol Type Inconformity, Type: %u, Crc: %u", iType, m_iCrc);
			T_Exception("Protocol Type Inconformity.");
			return false;
		}
		else if(m_iCrc != iCrc)
		{
			FmtError("Protocol Crc Inconformity, Type: %u, Crc: %u", iType, m_iCrc);
			T_Exception("Protocol Crc Inconformity.");
			return false;
		}

		if (g_ProtocolManager->IsAutoDecode())
		{
			return DecodeSelf();
		}

		return true;
	}

	Bool CProtocol::DecodeSelf()
	{
		try
		{
			m_sDecode >> *this;
		}
		catch (CException& rhsExcep)
		{
			FmtError(rhsExcep.GetMsg().c_str());		
			return false;
		}

		return true;
	}
}
