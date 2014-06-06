#ifndef _SOCKETADDR_H
#define _SOCKETADDR_H

#include "IPAddr.h"

namespace Util
{
	/************************************************************************/
	/* Socket��ַ��װ                                                        */
	/************************************************************************/
	class CSocketAddr : public CRefCounter
	{
	public:
		//IP�˿��ַ�������(֧������)
		CSocketAddr(const AString& sIpPort = "");

		//IP��ַ�Ͷ˿ڹ���
		CSocketAddr(const IPAddr& sHost, UInt16 iPort);
		
		//��ַ����
		CSocketAddr(const struct sockaddr* pAddr, Int32 iLen);
		
		//��������
		CSocketAddr(const CSocketAddr& sAddr);
		
		//����
		virtual ~CSocketAddr();
		
		//��ֵ����
		CSocketAddr& operator = (const CSocketAddr& sAddr);

	public:
		//��ȡIP��ַ
		virtual Bool GetIPAddr(IPAddr& sAddr) const;

		//��ȡ�˿�
		virtual UInt16 GetPort() const;

		//��ȡ��ַ�ֽڳ���
		virtual Int32 GetLength() const;

		//��ȡsockaddrָ��
		const sockaddr* GetAddress() const;

		//��ȡ��ַ��AF
		virtual Int32 GetAF() const;

		//ת��Ϊ�ַ�����ʽ
		virtual AString ToString() const;

		//��ȡ��ַ����
		virtual Int32 GetFamily() const;

		//���Ƶ�ַ��Ϣ
		virtual UInt8 CopyAddr(Char* pData) const;

		//��ַ�Ƿ���Ч
		virtual Bool IsValid() const;

	public:
		//С�ڲ���������
		Bool  operator < (const CSocketAddr& sAddr) const;

		//��Ȳ���������
		Bool  operator == (const CSocketAddr& sAddr) const;

	protected:
		//�ڲ���ʼ��
		void  Init(const IPAddr& sHost, UInt16 iPort);

	private:
		//��ַ��
		IPAddr::Family m_eFamily;

		//socket��ַ��Ϣ
		union
		{
			struct sockaddr_in  m_sIPv4Addr;
			struct sockaddr_in6 m_sIPv6Addr;
		};
	};

	//socket��ַ���ͼ�Ҫ����
	typedef CSocketAddr SocketAddr;
}
#endif //_SOCKETADDR_H
