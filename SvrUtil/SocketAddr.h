#ifndef _SOCKETADDR_H
#define _SOCKETADDR_H

#include "IPAddr.h"

namespace Util
{
	/************************************************************************/
	/* Socket地址封装                                                        */
	/************************************************************************/
	class CSocketAddr : public CRefCounter
	{
	public:
		//IP端口字符串构造(支持域名)
		CSocketAddr(const AString& sIpPort = "");

		//IP地址和端口构造
		CSocketAddr(const IPAddr& sHost, UInt16 iPort);
		
		//地址构造
		CSocketAddr(const struct sockaddr* pAddr, Int32 iLen);
		
		//拷贝构造
		CSocketAddr(const CSocketAddr& sAddr);
		
		//析构
		virtual ~CSocketAddr();
		
		//赋值函数
		CSocketAddr& operator = (const CSocketAddr& sAddr);

	public:
		//获取IP地址
		virtual Bool GetIPAddr(IPAddr& sAddr) const;

		//获取端口
		virtual UInt16 GetPort() const;

		//获取地址字节长度
		virtual Int32 GetLength() const;

		//获取sockaddr指针
		const sockaddr* GetAddress() const;

		//获取地址族AF
		virtual Int32 GetAF() const;

		//转换为字符串格式
		virtual AString ToString() const;

		//获取地址类型
		virtual Int32 GetFamily() const;

		//复制地址信息
		virtual UInt8 CopyAddr(Char* pData) const;

		//地址是否有效
		virtual Bool IsValid() const;

	public:
		//小于操作符重载
		Bool  operator < (const CSocketAddr& sAddr) const;

		//相等操作符重载
		Bool  operator == (const CSocketAddr& sAddr) const;

	protected:
		//内部初始化
		void  Init(const IPAddr& sHost, UInt16 iPort);

	private:
		//地址族
		IPAddr::Family m_eFamily;

		//socket地址信息
		union
		{
			struct sockaddr_in  m_sIPv4Addr;
			struct sockaddr_in6 m_sIPv6Addr;
		};
	};

	//socket地址类型简要定义
	typedef CSocketAddr SocketAddr;
}
#endif //_SOCKETADDR_H
