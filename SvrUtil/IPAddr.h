#ifndef _IPADDR_H
#define _IPADDR_H

#include "RefCounter.h"

namespace Util
{
	/************************************************************************/
	/* IP��ַ��װ                                                            */
	/************************************************************************/
	class CIPAddr : public CRefCounter
	{
	public:
		//IP��ַ����
		enum Family
		{
			IPv4,
			IPv6
		};

	public:		
		//Ĭ�Ϲ���
		CIPAddr(Family eFamile = IPv4);

		//��������
		CIPAddr(const CIPAddr& sAddr);
		
		//IP��ַ��ʼ��(֧������)
		CIPAddr(const AString& sIpAddr);

		//pAddr��ʾIPv4��in_addr ���� IPv6��in6_addrָ��
		CIPAddr(const void* pAddr, Int32 iLen);

		//����
		virtual ~CIPAddr();
		
		//��ֵ�����
		CIPAddr& operator = (const CIPAddr& sAddr);
		
	public:
		//����IP��ַ������
		virtual Family GetFamily() const;
			
		//IP��ַת��Ϊ�ַ���
		virtual AString ToString() const;
			
		//��ȡIP��ַ����
		virtual Int32 GetLength() const;
			
		//����IP��ַ��Ϣ,IPv4: in_addr IPv6: in6_addr
		const void* GetAddress() const;
		
		//��ȡIP��ַ��,IPv4: AF_INET IPv6: AF_INET6
		virtual Int32 GetAF() const;		

	protected:
		//�����ַ�����IP��Ϣ
		virtual Bool ParseIpAddr(const AString& sIpAddr);

		//������������ʼ��
		virtual Bool InitByHostName(const AString& sHostName);

	private:
		//IP��ַ������
		Family m_eFamily;

		//IP��ַ��Ϣ
		union
		{
			struct in_addr  m_sIPv4Addr;
			struct in6_addr m_sIPv6Addr;
		};
	};

	//IP��ַ��Ҫ���Ͷ���
	typedef CIPAddr IPAddr;
}
#endif //_IPADDR_H
