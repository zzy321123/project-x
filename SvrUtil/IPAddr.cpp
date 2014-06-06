#include "IPAddr.h"
#include "Exception.h"

namespace Util
{
	CIPAddr::CIPAddr(Family eFamile) : m_eFamily(eFamile)
	{
		if (m_eFamily == IPv4)
		{
			memset(&m_sIPv4Addr,0,sizeof(m_sIPv4Addr));
		}
		else
		{
			memset(&m_sIPv6Addr,0,sizeof(m_sIPv6Addr));
		}
	}

	CIPAddr::CIPAddr(const CIPAddr& sAddr)
	{
		m_eFamily = sAddr.GetFamily();
		if (m_eFamily == IPv4)
		{
			m_sIPv4Addr = sAddr.m_sIPv4Addr;
		}
		else
		{
			m_sIPv6Addr = sAddr.m_sIPv6Addr;
		}
	}

	CIPAddr::CIPAddr(const AString& sIpAddr)
	{
		if (sIpAddr.size())
		{
			if(!ParseIpAddr(sIpAddr))
			{
				T_Exception("ParseIpAddr Error.");
			}
		}
		else
		{
			m_eFamily = IPv4;
			m_sIPv4Addr.s_addr = htonl(INADDR_ANY);
		}
	}

	CIPAddr::CIPAddr(const void* pAddr, Int32 iLen)
	{
		if (iLen == sizeof(struct in_addr))
		{
			memcpy(&m_sIPv4Addr,pAddr,iLen);
			m_eFamily = IPv4;
		}
		else if (iLen == sizeof(struct in6_addr))
		{
			memcpy(&m_sIPv6Addr,pAddr,iLen);
			m_eFamily = IPv6;
		}
	}

	CIPAddr::~CIPAddr()
	{
	}

	CIPAddr& CIPAddr::operator = (const CIPAddr& sAddr)
	{
		m_eFamily = sAddr.GetFamily();

		if (m_eFamily == IPv4)
			m_sIPv4Addr = sAddr.m_sIPv4Addr;
		else
			m_sIPv6Addr = sAddr.m_sIPv6Addr;
		
		return *this;
	}

	CIPAddr::Family CIPAddr::GetFamily() const
	{
		return m_eFamily;
	}	

	Int32 CIPAddr::GetLength() const
	{
		if (m_eFamily == IPv4)
			return sizeof(m_sIPv4Addr);
		
		return sizeof(m_sIPv6Addr);
	}
		
	const void* CIPAddr::GetAddress() const
	{
		if (m_eFamily == IPv4)
			return &m_sIPv4Addr;
		
		return &m_sIPv6Addr;
	}

	Int32 CIPAddr::GetAF() const
	{
		if (m_eFamily == IPv4)
			return AF_INET;

		return AF_INET6;
	}

	AString CIPAddr::ToString() const
	{
		AString sRet = "";		
		if (m_eFamily == IPv4)
		{
			Char sVal[256] = {0};
			const UInt8* pBytes = reinterpret_cast<const UInt8*>(&m_sIPv4Addr);
			sprintf(sVal,"%d.%d.%d.%d",pBytes[0],pBytes[1],pBytes[2],pBytes[3]);
			sRet = sVal;
		}
		else
		{			
			Assert(false && "Cannot Support IPv6 Addr.");
		}
		return sRet;
	}	

	Bool CIPAddr::ParseIpAddr(const AString& sIpAddr)
	{
		if (sIpAddr.empty()) return false;

		//”Ú√˚Ω‚Œˆ
		if (sIpAddr != "*" && (sIpAddr[0] < '0' || sIpAddr[0] > '9'))
		{
			return InitByHostName(sIpAddr);
		}

		//IPΩ‚Œˆ
		if (sIpAddr.size() <= IPV4_MAX_LENGTH)
		{
			m_eFamily = IPv4;

			if (sIpAddr == "*")
				m_sIPv4Addr.s_addr = INADDR_ANY;
			else
				m_sIPv4Addr.s_addr = inet_addr(sIpAddr.c_str());

			if (m_sIPv4Addr.s_addr != INADDR_NONE)
				return true;
		}
		else
		{
			m_eFamily = IPv6;
			Assert(false && "Cannot Support IPv6 Addr.");		
		}

		return false;
	}

	Bool CIPAddr::InitByHostName(const AString& sHostName)
	{
		if (sHostName.size())
		{
			struct hostent* pHost = gethostbyname(sHostName.c_str());
			if (pHost)
			{
				if (pHost->h_addrtype == AF_INET)
				{
					m_eFamily   = IPv4;
					m_sIPv4Addr = *(struct in_addr*)pHost->h_addr_list[0];
					return true;
				}
				else if (pHost->h_addrtype == AF_INET6)
				{
					m_eFamily   = IPv6;
					m_sIPv6Addr = *(struct in6_addr*)pHost->h_addr_list[0];
					return true;
				}				 
			}
		}
		return false;
	}
}
