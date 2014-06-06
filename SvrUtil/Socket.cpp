#include "Socket.h"
#include "StringUtil.h"

namespace Util
{
	CSocket::CSocket(SOCKET hSocket)
	{
		m_hSocket    = hSocket;
		m_bBlocking  = true;
		m_iErrCode   = 0;
		m_iTransport = TYPE_UNKNOWN;
	}

	CSocket::~CSocket()
	{
	}	

	CSocket& CSocket::operator = (const CSocket& sSocket)
	{
		if (&sSocket != this)
		{
			m_hSocket	 = sSocket.m_hSocket;
			m_iTransport = sSocket.m_iTransport;
			m_bBlocking	 = sSocket.m_bBlocking;
			m_iErrCode	 = sSocket.m_iErrCode;
		}
		return *this;
	}

	Bool CSocket::Create(Int32 iAF, Int32 iType, Int32 iProtocol)
	{
		Reset();

		m_hSocket = socket(iAF,iType,iProtocol);

		if (m_hSocket == INVALID_SOCKET)
		{
			m_iErrCode = GetSocketErr();
			return false;
		}

		if (iType == SOCK_DGRAM)
			m_iTransport = TYPE_UDP;
		else
			m_iTransport = TYPE_TCP;

		return true;
	}

	Bool CSocket::Attach(SOCKET hSocket, Int32 iType, Bool bBlocking)
	{
		m_hSocket   = hSocket;
		m_bBlocking = bBlocking;

		if (iType == SOCK_DGRAM)
			m_iTransport = TYPE_UDP;
		else
			m_iTransport = TYPE_TCP;

		return m_hSocket != INVALID_SOCKET;
	}

	UInt8 CSocket::GetSocketType() const
	{
		if (IsTcpTransport())
			return TYPE_TCP;

		return TYPE_UDP;
	}

	Bool CSocket::IsUdpTransport() const
	{
		return m_iTransport == TYPE_UDP;
	}

	Bool CSocket::IsTcpTransport() const
	{
		return m_iTransport == TYPE_TCP;
	}

	Bool CSocket::Connect(const SocketAddr& sAddr, Int32 iTimeout)
	{
		if (!IsValid() || !sAddr.IsValid())
			return false;

		if (iTimeout <= 0)
		{
			Int32 iRet = connect(m_hSocket, sAddr.GetAddress(), sAddr.GetLength());
			if (iRet != UTIL_OK)
			{
				m_iErrCode = GetSocketErr();
				return false;
			}
		}
		else
		{
			Bool bSetBlocking = false;
			if (GetBlocking())
			{
				SetBlocking(false);
				bSetBlocking = true;
			}

			//连接超时错误处理
			if (connect(m_hSocket,sAddr.GetAddress(),sAddr.GetLength()) != UTIL_OK)
			{
				m_iErrCode = GetSocketErr();

				//发生错误
				if (!CHECK_ERROR_CONNECT_RETRIABLE(m_iErrCode))
				{
					if (bSetBlocking)
						SetBlocking(true);

					closesocket(m_hSocket);
					return false;
				}					
			}

			//连接成功等待套接字可写事件(避免出现ENOTCONN错误)
			//在非阻塞情况下的套接字连接之后,如果在可读事件之前投递Socket相关操作
			//可能引发10057的错误(未连接的套接字)
			if (!UpdateEvent(HEVENT_WRITE, iTimeout))
			{
				if (bSetBlocking)
					SetBlocking(true);

				closesocket(m_hSocket);
				return false;
			}

			if (bSetBlocking)
				SetBlocking(true);			
		}

		return true;
	}

	Bool CSocket::Bind(const SocketAddr& sAddr)
	{
		if (!IsValid() || !sAddr.IsValid())
			return false;

		if (bind(m_hSocket,sAddr.GetAddress(),sAddr.GetLength()) != UTIL_OK)
		{
			m_iErrCode = GetSocketErr();
			return false;
		}

		return true;
	}

	Bool CSocket::Listen(Int32 iBackLog)
	{
		if (!IsValid())
			return false;

		if (listen(m_hSocket,iBackLog) != UTIL_OK)
		{
			m_iErrCode = GetSocketErr();
			return false;
		}

		return true;
	}
	
	Bool CSocket::Accept(CSocket& sSocket, SocketAddr& sAddr)
	{
		if (!IsValid() || !sAddr.IsValid())
			return false;

		socklen_t iLen = sAddr.GetLength();
		SOCKET hSocket = accept(m_hSocket, const_cast<sockaddr*>(sAddr.GetAddress()), &iLen);
		if (hSocket == INVALID_SOCKET)
		{
			m_iErrCode = GetSocketErr();
			return false;			
		}

		sSocket.Attach(hSocket,true);
		return true;
	}

	Bool CSocket::Send(void* pData, Size_t& iSize, Int32 iFlags)
	{
		if (!IsValid())
			return false;

		if(!pData || !iSize)
			return true;

		iSize = send(m_hSocket, (Char*)pData, iSize, iFlags);
		if (iSize != Size_t(-1))
			return true;
		
		m_iErrCode = GetSocketErr();
		if (CHECK_ERROR_RW_RETRIABLE(m_iErrCode))
		{
			iSize = 0;
			return true;
		}

		return false;
	}

	Bool CSocket::Receive(void* pData, Size_t& iSize, Int32 iFlags)
	{
		if (!IsValid())
			return false;

		if(!pData || !iSize)
			return true;

		iSize = recv(m_hSocket,(Char*)pData,iSize,iFlags);
		
		//连接被关闭
		if (iSize == 0)
		{							
			m_iErrCode = GetSocketErr();
			return false;
		}
		//正常数据接收
		else if (iSize != Size_t(-1))
		{
			return true;
		}
		
		//发生错误
		m_iErrCode = GetSocketErr();
		if (CHECK_ERROR_RW_RETRIABLE(m_iErrCode))
		{				
			iSize = 0;
			return true;
		}
		
		return false;
	}

	Bool CSocket::SendTo(void* pData, Size_t& iSize, const SocketAddr& sAddr, Int32 iFlags)
	{
		if (!IsValid() || !sAddr.IsValid())
			return false;

		if(!pData || !iSize)
			return true;

		iSize = sendto(m_hSocket, (Char*)pData, iSize, iFlags, sAddr.GetAddress(), sAddr.GetLength());
		if (iSize != Size_t(-1))
			return true;
		
		m_iErrCode = GetSocketErr();
		if (CHECK_ERROR_RW_RETRIABLE(m_iErrCode))
		{
			iSize = 0;
			return true;
		}

		return false;
	}

	Bool CSocket::ReceiveFrom(void* pData, Size_t& iSize, SocketAddr& sAddr ,Int32 iFlags)
	{
		if (!IsValid() || !sAddr.IsValid())
			return false;

		if(!pData || !iSize)
			return true;

		socklen_t iAddrLen  = sAddr.GetLength();
		iSize = recvfrom(m_hSocket,(Char*)pData,iSize,iFlags,const_cast<sockaddr*>(sAddr.GetAddress()),&iAddrLen);

		//连接被关闭
		if (iSize == 0)
		{							
			m_iErrCode = GetSocketErr();
			return false;
		}		
		//正常接收数据
		else if (iSize != Size_t(-1))
		{
			return true;
		}	

		//发生错误
		m_iErrCode = GetSocketErr();
		if (CHECK_ERROR_RW_RETRIABLE(m_iErrCode))
		{				
			iSize = 0;
			return true;
		}

		return false;
	}

	Bool CSocket::SetOption(Int32 iOptName, const void* pOptVal, Int32 iOptLen, Int32 iLevel)
	{
		if (IsValid() && pOptVal && iOptLen > 0)
		{
			if (setsockopt(m_hSocket,iLevel,iOptName,(const char*)pOptVal,iOptLen) == UTIL_OK)
				return true;
		}
		return false;
	}

	Bool CSocket::GetOption(Int32 iOptName, void* pOptVal, Int32& iOptLen, Int32 iLevel)
	{
		if (IsValid() && pOptVal && iOptLen > 0)
		{
			socklen_t iLen = (socklen_t)iOptLen;
			if (getsockopt(m_hSocket,iLevel,iOptName,(char*)pOptVal,&iLen) == UTIL_OK)
			{
				iOptLen = iLen;
				return true;
			}
		}
		return false;
	}

	SOCKET CSocket::Handle() const
	{
		return m_hSocket;
	}

	Bool CSocket::IsValid() const
	{
		return m_hSocket != INVALID_SOCKET;
	}

	Bool CSocket::GetPeerName(SocketAddr& sAddr)
	{
		if (IsValid() && sAddr.IsValid())
		{
			socklen_t iAddrLen = sAddr.GetLength();
			if(getpeername(m_hSocket,const_cast<sockaddr*>(sAddr.GetAddress()), &iAddrLen) == UTIL_OK)
				return true;
		
			m_iErrCode = GetSocketErr();
		}
		return false;
	}

	Bool CSocket::GetSockName(SocketAddr& sAddr)
	{
		if (IsValid() && sAddr.IsValid())
		{
			socklen_t iAddrLen = sAddr.GetLength();
			if(getsockname(m_hSocket,const_cast<sockaddr*>(sAddr.GetAddress()),&iAddrLen) == UTIL_OK)
				return true;

			m_iErrCode = GetSocketErr();
		}
		return false;
	}

	Bool CSocket::Reset()
	{
		if(IsValid())
			return Close(true);
			
		return false;
	}

	Bool CSocket::Shutdown(Int32 iHow)
	{
		if(IsValid())
		{
			if (shutdown(m_hSocket,iHow) == UTIL_OK)
				return true;
		}
		return false;
	}

	Bool CSocket::Close(Bool bGraceful)
	{
		if(IsValid())
		{
			if (bGraceful)
				Shutdown(SD_BOTH);
			
			closesocket(m_hSocket);
			m_hSocket = INVALID_SOCKET;
			return true;
		}
		return false;
	}

	Bool CSocket::SetLinger(Bool bOn, Int32 iSeconds)
	{
		struct linger xLin;
		xLin.l_onoff  = bOn ? 1 : 0;
		xLin.l_linger = iSeconds;
		return SetOption(SO_LINGER, &xLin, sizeof(xLin), SOL_SOCKET);
	}

	void CSocket::GetLinger(Bool& bOn, Int32& iSeconds)
	{
		struct linger xLin;
		Int32 iLen = sizeof(xLin);
		SetOption(SO_LINGER, &xLin, iLen,SOL_SOCKET);
		bOn      = xLin.l_onoff != 0;
		iSeconds = xLin.l_linger;
	}

	Bool CSocket::SetNoDelay(Bool bFlag)
	{
		Int32 iVal = bFlag ? 1 : 0;
		return SetOption(TCP_NODELAY, &iVal, sizeof(iVal), IPPROTO_TCP);
	}

	Bool CSocket::GetNoDelay()
	{
		Int32 iVal = 0;
		Int32 iLen = sizeof(iVal);
		GetOption(TCP_NODELAY,&iVal,iLen,IPPROTO_TCP);
		return iVal != 0;
	}

	void CSocket::SetSendBufferSize(Int32 iSize)
	{
		if(IsValid()) 
			SetOption(SO_SNDBUF,&iSize,sizeof(iSize),SOL_SOCKET);
	}

	Int32 CSocket::GetSendBufferSize()
	{
		Int32 iSize = 0;
		Int32 iLen  = sizeof(iSize);
		GetOption(SO_SNDBUF, &iSize, iLen, SOL_SOCKET);
		return iSize;
	}

	void CSocket::SetRecvBufferSize(Int32 iSize)
	{
		if(IsValid()) 
			SetOption(SO_RCVBUF,&iSize,sizeof(iSize),SOL_SOCKET);
	}

	Int32 CSocket::GetRecvBufferSize()
	{
		Int32 iSize = 0;
		Int32 iLen  = sizeof(iSize);
		GetOption(SO_RCVBUF, &iSize,iLen,SOL_SOCKET);
		return iSize;
	}

	Bool   CSocket::GetBlocking() const
	{
		return m_bBlocking;
	}

	Bool CSocket::SetBlocking(Bool bBlocking)
	{
#ifdef PLATFORM_WINDOWS
		Int32 iArg = bBlocking ? 0 : 1;
		if (ioctlsocket(m_hSocket, FIONBIO, reinterpret_cast<u_long*>(&iArg)) == UTIL_OK)
		{
			m_bBlocking = bBlocking;
			return true;
		}
#else
		Int32 iFlag = fcntl(m_hSocket, F_GETFL, 0);
		if (bBlocking)
			iFlag &= ~O_NONBLOCK;
		else 
			iFlag |= O_NONBLOCK;

		if (fcntl(m_hSocket, F_SETFL, iFlag) != UTIL_ERROR)
		{
			m_bBlocking = bBlocking;
			return true;
		}
#endif
		return false;
	}

	Int32 CSocket::UpdateEvent(UInt32 iMode, Int32 iTimeout)
	{
		if(!IsValid()) 
			return 0;

		fd_set fdRead;
		fd_set fdWrite;
		fd_set fdExcept;
		FD_ZERO(&fdRead);
		FD_ZERO(&fdWrite);
		FD_ZERO(&fdExcept);

		if (iMode & HEVENT_READ)
		{
			FD_SET(m_hSocket, &fdRead);
		}

		if (iMode & HEVENT_WRITE)
		{
			FD_SET(m_hSocket, &fdWrite);
		}

		if (iMode & HEVENT_EXCEPT)
		{
			FD_SET(m_hSocket, &fdExcept);
		}

		struct timeval tv;
		tv.tv_sec  = iTimeout/1000;
		tv.tv_usec = (iTimeout % 1000) * 1000;

		Int32 iEventNum = select(m_hSocket+1, &fdRead, &fdWrite, &fdExcept, iTimeout>=0?&tv : 0);
		if(iEventNum > 0)
		{
			UInt32 iEvent = 0;
			if (FD_ISSET(m_hSocket,&fdRead))
				iEvent |= HEVENT_READ;

			if (FD_ISSET(m_hSocket,&fdWrite))
				iEvent |= HEVENT_WRITE;

			if (FD_ISSET(m_hSocket,&fdExcept))
				iEvent |= HEVENT_EXCEPT;

			return iEvent;
		} 
		return 0;
	}

	Int32 CSocket::GetSocketErr()
	{
#ifdef PLATFORM_WINDOWS
		return WSAGetLastError();
#else
		return errno;
#endif
	}

	Bool CSocket::SetBlocking(SOCKET hSocket, Bool bBlocking)
	{
#ifdef PLATFORM_WINDOWS
		Int32 iArg = bBlocking ? 0 : 1;
		if (ioctlsocket(hSocket, FIONBIO, reinterpret_cast<u_long*>(&iArg)) == UTIL_OK)
			return true;
#else
		Int32 iFlag = fcntl(hSocket, F_GETFL, 0);
		if (bBlocking)
			iFlag &= ~O_NONBLOCK;
		else 
			iFlag |= O_NONBLOCK;

		if (fcntl(hSocket, F_SETFL, iFlag) != UTIL_ERROR)
			return true;
#endif
		return false;
	}

	Int32 CSocket::UpdateEvent(SOCKET hSocket, UInt32 iMode, Int32 iTimeout)
	{
		if(hSocket == INVALID_SOCKET) 
			return 0;

		fd_set fdRead;
		fd_set fdWrite;
		fd_set fdExcept;
		FD_ZERO(&fdRead);
		FD_ZERO(&fdWrite);
		FD_ZERO(&fdExcept);

		if (iMode & HEVENT_READ)
		{
			FD_SET(hSocket, &fdRead);
		}

		if (iMode & HEVENT_WRITE)
		{
			FD_SET(hSocket, &fdWrite);
		}

		if (iMode & HEVENT_EXCEPT)
		{
			FD_SET(hSocket, &fdExcept);
		}

		struct timeval tv;
		tv.tv_sec  = iTimeout/1000;
		tv.tv_usec = (iTimeout % 1000) * 1000;

		Int32 iEventNum = select(hSocket + 1, &fdRead, &fdWrite, &fdExcept, iTimeout >= 0 ? &tv : 0);
		if(iEventNum > 0)
		{
			UInt32 iEvent = 0;
			if (FD_ISSET(hSocket,&fdRead))
				iEvent |= HEVENT_READ;

			if (FD_ISSET(hSocket,&fdWrite))
				iEvent |= HEVENT_WRITE;

			if (FD_ISSET(hSocket,&fdExcept))
				iEvent |= HEVENT_EXCEPT;

			return iEvent;
		} 
		return 0;
	}

	AString CSocket::GetHostName()
	{
		Char sHostName[DEFAULT_SIZE] = {0};
		if (gethostname(sHostName, DEFAULT_SIZE) == UTIL_OK)
			return sHostName;

		return "";
	}

	AString CSocket::GetHostIp()
	{
		Char sHostName[DEFAULT_SIZE] = {0};
		if (gethostname(sHostName, DEFAULT_SIZE) != UTIL_OK)
			return "";

		struct hostent* pHost = gethostbyname(sHostName);
		if (!pHost)
			return "";

		for (Int32 i=0; i<pHost->h_length; i++)
		{
			if (pHost->h_addr_list[i] == 0)
				continue;

			struct in_addr sAddr;
			memcpy(&sAddr, pHost->h_addr_list[i], sizeof(sAddr));
			AString sIp = inet_ntoa(sAddr);
			CStringUtil::Trim(sIp);
			if (sIp.size() && sIp != "127.0.0.1")
				return sIp;
		}

		return "127.0.0.1";
	}

	Bool  CSocket::InitTcpServer(const SocketAddr& sBindAddr, Bool bNoBlocking)
	{
		if(!Create()) 
			return false;

		if(bNoBlocking)
			SetBlocking(false);

		SetNoDelay(true);

		struct linger sLing = {0, 0};
		Int32  iFlags = 1;

		SetOption(SO_REUSEADDR,(const void *)&iFlags, sizeof(iFlags));
		SetOption(SO_KEEPALIVE,(const void *)&iFlags, sizeof(iFlags));
		SetOption(SO_LINGER,   (const void *)&sLing,  sizeof(sLing));

		if(!Bind(sBindAddr))
			return false;

		if(!Listen()) 
			return false;

		return IsValid();
	}

	Bool  CSocket::InitTcpClient(const SocketAddr& sSvrAddr, Bool bNoBlocking, Int32 iConnTimeout)
	{
		if (!Create())
			return false;

		SetNoDelay(true);

		if(bNoBlocking)
			SetBlocking(false);

		if(!Connect(sSvrAddr, iConnTimeout))
			return false;

		return IsValid();
	}

	Bool  CSocket::InitTcpPeer(Bool bNoBlocking)
	{
		SetNoDelay(true);

		if(bNoBlocking)
			SetBlocking(false);

		return IsValid();
	}

	Bool  CSocket::InitUdpServer(const SocketAddr& sBindAddr, Bool bNoBlocking)
	{
		if(!Create(AF_INET,SOCK_DGRAM,IPPROTO_UDP)) 
			return false;

		SetNoDelay(true);

		if(bNoBlocking)
			SetBlocking(false);

		Int32  iFlags = 1;
		SetOption(SO_REUSEADDR,(const void *)&iFlags, sizeof(iFlags));

		if(!Bind(sBindAddr))
			return false;

		return IsValid();
	}

	Bool  CSocket::InitUdpClient(const SocketAddr& sSvrAddr, Bool bNoBlocking, Bool bBindClient)
	{
		if(!Create(AF_INET,SOCK_DGRAM,IPPROTO_UDP)) 
			return false;

		SetNoDelay(true);

		if(bNoBlocking)
			SetBlocking(false);

		if (bBindClient)
		{
			sockaddr_in sAddr;
			sAddr.sin_family = AF_INET;
			sAddr.sin_addr.s_addr = htons(INADDR_ANY);
			sAddr.sin_port = htons(0);

			SocketAddr sClientAddr((sockaddr*)&sAddr,sizeof(sAddr));
			if(!Bind(sClientAddr))
				return false;
		}

		return IsValid();
	}

	Int32 CSocket::MaximizeSndBuf(SOCKET hSocket)
	{
		socklen_t iIntSize = sizeof(Int32);
		Int32 iLast = 0, iMin = 0, iMax = 0, iAvg = 0, iOld = 0;

		if (getsockopt(hSocket, SOL_SOCKET, SO_SNDBUF, (Char*)&iOld, &iIntSize) != 0) 
			return 0;

		iLast = iOld;
		iMin  = iOld;
		iMax  = MAX_SENDBUF_SIZE;

		while (iMin <= iMax) 
		{
			iAvg = ((UInt32)(iMin + iMax)) / 2;
			if (setsockopt(hSocket, SOL_SOCKET, SO_SNDBUF, (Char*)&iAvg, iIntSize) == 0) 
			{
				iLast = iAvg;
				iMin  = iAvg + 1;
			} 
			else 
			{
				iMax = iAvg - 1;
			}
		}
		return iLast;
	}

	Int32 CSocket::MaximizeRcvBuf(SOCKET hSocket)
	{
		socklen_t iIntSize = sizeof(Int32);
		Int32 iLast = 0, iMin = 0, iMax = 0, iAvg = 0, iOld = 0;

		if (getsockopt(hSocket, SOL_SOCKET, SO_RCVBUF, (Char*)&iOld, &iIntSize) != 0) 
			return 0;

		iLast = iOld;
		iMin  = iOld;
		iMax  = MAX_SENDBUF_SIZE;

		while (iMin <= iMax) 
		{
			iAvg = ((UInt32)(iMin + iMax)) / 2;
			if (setsockopt(hSocket, SOL_SOCKET, SO_RCVBUF, (Char*)&iAvg, iIntSize) == 0) 
			{
				iLast = iAvg;
				iMin = iAvg + 1;
			} 
			else 
			{
				iMax = iAvg - 1;
			}
		}
		return iLast;
	}

	UInt32 CSocket::Htonl(UInt32 iVal)
	{
		return htonl(iVal);
	}

	UInt16  CSocket::Htons(UInt16 iVal)
	{
		return htons(iVal);
	}

	UInt32 CSocket::Ntohl(UInt32 iVal)
	{
		return ntohl(iVal);
	}

	UInt16 CSocket::Ntohs(UInt16 iVal)
	{
		return ntohs(iVal);
	}

	UInt32 CSocket::InetAddr(const AString& sAddr)
	{
		return inet_addr(sAddr.c_str());
	}

	AString CSocket::InetNtoa(const in_addr& sAddr)
	{
		return inet_ntoa(sAddr);
	}
}
