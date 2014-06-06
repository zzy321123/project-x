#include "SocketPair.h"

namespace Util
{
#ifdef PLATFORM_WINDOWS
	Int32 win32_socketpair(Int32 iAF, Int32 iType,SOCKET hSocket[2])
	{
		struct sockaddr_in listen_addr;
		struct sockaddr_in connect_addr;

		SOCKET hListener = socket(iAF,iType,0);
		if (hListener == INVALID_SOCKET)
		{
			return UTIL_ERROR;
		}

		memset(&listen_addr, 0, sizeof(listen_addr));
		listen_addr.sin_family		= iAF;
		listen_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
		listen_addr.sin_port		= 0;
		if(::bind(hListener,(struct sockaddr*)&listen_addr, sizeof(listen_addr)) != UTIL_OK)
		{
			closesocket(hListener);
			return UTIL_ERROR;
		}

		if(listen(hListener,SOMAXCONN) != UTIL_OK)
		{
			closesocket(hListener);
			return UTIL_ERROR;
		}

		Int32 iSize = sizeof(connect_addr);
		if (getsockname(hListener, (struct sockaddr*) &connect_addr, &iSize) != UTIL_OK || iSize != sizeof(connect_addr))
		{
			closesocket(hListener);
			return UTIL_ERROR;
		}

		SOCKET hClient = socket(iAF,SOCK_STREAM,0);
		if (hClient == INVALID_SOCKET)
		{
			closesocket(hListener);
			return UTIL_ERROR;
		}

		if (connect(hClient,(struct sockaddr*)&connect_addr, sizeof(connect_addr)) != UTIL_OK)
		{
			closesocket(hClient);
			closesocket(hListener);
			return UTIL_ERROR;
		}

		SOCKET hServer = accept(hListener,(struct sockaddr*)&listen_addr, &iSize);
		if (hServer == INVALID_SOCKET || iSize != sizeof(listen_addr))
		{
			closesocket(hClient);
			closesocket(hListener);
			return UTIL_ERROR;
		}

		closesocket(hListener);

		if (getsockname(hClient, (struct sockaddr*)&connect_addr, &iSize) != UTIL_OK)
		{
			closesocket(hServer);
			closesocket(hClient);
			closesocket(hListener);
			return UTIL_ERROR;
		}

		if (iSize != sizeof (connect_addr)
			|| listen_addr.sin_family != connect_addr.sin_family
			|| listen_addr.sin_addr.s_addr != connect_addr.sin_addr.s_addr
			|| listen_addr.sin_port != connect_addr.sin_port)
		{
			closesocket(hServer);
			closesocket(hClient);
			closesocket(hListener);
			return UTIL_ERROR;
		}

		hSocket[0] = hClient, hSocket[1] = hServer;
		return UTIL_OK;
	}
#endif

	CSocketPair::CSocketPair()
	{
		m_hSocket[0] = INVALID_SOCKET;
		m_hSocket[1] = INVALID_SOCKET;
		m_iSend		 = 0;
		m_iRecv		 = 0;
		m_pMutex	 = 0;
	}

	CSocketPair::~CSocketPair()
	{
		Close();
	}
	
	Bool CSocketPair::Create(Bool bThreadSafe)
	{
#ifdef PLATFORM_WINDOWS
		 if (win32_socketpair(AF_INET,SOCK_STREAM,m_hSocket) != UTIL_OK)
			 return false;
#else
		if (socketpair(AF_UNIX,SOCK_STREAM,0,m_hSocket) != UTIL_OK)
			return false;
		
#endif

		Int32 iBufSize = CSocket::MaximizeSndBuf(GetWriteFd());
		A_Exception(iBufSize > 0);

		if (bThreadSafe)
			m_pMutex = new CMutex;

		return true;
	}

	Bool CSocketPair::Close()
	{
		SOCKET hSocket = GetWriteFd();
		if (hSocket != INVALID_SOCKET)
			closesocket(hSocket);

		hSocket = GetReadFd();
		if (hSocket != INVALID_SOCKET)
			closesocket(hSocket);

		m_iSend = 0;
		m_iRecv = 0;

		m_hSocket[0] = INVALID_SOCKET;
		m_hSocket[1] = INVALID_SOCKET;
		
		DEF_RELEASE(m_pMutex);
		return true;
	}

	Bool CSocketPair::Clear()
	{
		if (IsValid())
		{
			SOCKET hSocket = GetReadFd();
			if (CSocket::SetBlocking(hSocket, false))
			{
				Char iFlag  = 0;
				Int32 iSize = sizeof(Char);
				while(recv(hSocket, &iFlag, iSize,0) > 0);

				return CSocket::SetBlocking(hSocket, true);
			}
		}
		return false;
	}

	Bool CSocketPair::IsValid() const
	{
		return m_hSocket[0] != INVALID_SOCKET && m_hSocket[1] != INVALID_SOCKET;
	}

	SOCKET CSocketPair::GetReadFd() const
	{
		return m_hSocket[0];
	}

	SOCKET CSocketPair::GetWriteFd() const
	{
		return m_hSocket[1];
	}	

	Bool CSocketPair::Send(const void* pData, Size_t& iSize, Int32 iFlags)
	{
		SOCKET hSocket = GetWriteFd();
		if (hSocket != INVALID_SOCKET)
		{
			if (m_pMutex)
				m_pMutex->Lock(__FILE__, __LINE__);

			iSize = send(hSocket, (const Char*)pData, iSize, iFlags);
			
			if (m_pMutex)
				m_pMutex->Unlock();

			if (iSize != Size_t(-1))
			{
				m_iSend += iSize;
				return true;
			}
		}
		return false;		
	}

	Bool CSocketPair::Receive(void* pData, Size_t& iSize, Int32 iFlags)
	{
		SOCKET hSocket = GetReadFd();
		if (hSocket != INVALID_SOCKET)
		{
			iSize = recv(hSocket,(Char*)pData, iSize, iFlags);
			if (iSize != Size_t(-1))
			{
				m_iRecv += iSize;
				return true;
			}
		}
		return false;
	}
}
