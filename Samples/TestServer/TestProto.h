#ifndef _TESTPROTO_H_
#define _TESTPROTO_H_

#include "SvrUtil.h"

namespace Util
{
	enum
	{
		PROTO_A = 1000,
		PROTO_B,
		PROTO_C,
		PROTO_CNT
	};

	class ProtoA : public CProtocol
	{
	public:
		ProtoA() : CProtocol(PROTO_A)
		{
			m_sMsg = CStringUtil::RandomString<AString>(CRand::RandInt(32,256));
		}

	public:
		virtual CProtocol* Clone() const
		{
			return new ProtoA;
		}

		virtual COctetsStream& Marshal(COctetsStream& rhsOS)
		{
			return rhsOS << m_sMsg;
		}
		
		virtual COctetsStream& Unmarshal(COctetsStream& rhsOS)
		{
			return rhsOS >> m_sMsg;
		}

	public:
		AString m_sMsg;
	};

	class ProtoB : public CProtocol
	{
	public:
		ProtoB() : CProtocol(PROTO_B)
		{
			m_iSize = CRand::RandInt(8,32);
			m_iTime =COSOperator::GetTickCount();
		}

	public:
		virtual CProtocol* Clone() const
		{
			return new ProtoB;
		}

		virtual void Print()
		{
			FmtPrint("Size = %d, Time = %d",m_iSize,m_iTime);
		}

		virtual COctetsStream& Marshal(OctetsStream& rhsOS)
		{
			return rhsOS << m_iSize << m_iTime;
		}

		virtual OctetsStream& Unmarshal(OctetsStream& rhsOS)
		{
			return rhsOS >> m_iSize >> m_iTime;
		}

	public:
		Int32  m_iSize;
		Int32  m_iTime;
	};

	class ProtoC : public CProtocol
	{
	public:
		ProtoC() : CProtocol(PROTO_C)
		{
			m_fX = CRand::RandFloat(0,1);
			m_fY = CRand::RandFloat(0,1);
			m_fZ = CRand::RandFloat(0,1);
		}

	public:
		virtual CProtocol* Clone() const
		{
			return new ProtoC;
		}

		virtual COctetsStream& Marshal(COctetsStream& rhsOS)
		{
			return rhsOS << m_fX << m_fY << m_fZ;
		}

		virtual COctetsStream& Unmarshal(COctetsStream& rhsOS)
		{
			return rhsOS >> m_fX >> m_fY >> m_fZ;
		}

	public:
		Float m_fX;
		Float m_fY;
		Float m_fZ;
	};
}
#endif 
