#include "TestProto.h"
#include "SvrUtil.h"
#include "BaseGateway.h"
//#include "MemLeak.h"
//#include "../libMemLeak/src/MemLeakDetect.h"

CThread* g_DbgThread = 0;

PVoid DoKeyDownExit(void* pArg)
{
	BaseGateway* pGateway = (BaseGateway*)pArg;
	while (pGateway)
	{
		if(COSOperator::WaitKeyboardInput("",false) == 'Q')
		{
			pGateway->Stop();
			return 0;
		}
	}
	return 0;
}

int main(int argc, Char* argv[])
{
	CSvrUtil::Init();

	REGISTER_PROTO(ProtoA);
	REGISTER_PROTO(ProtoB);
	REGISTER_PROTO(ProtoC);

	if (argc >= 3)
	{
		COSOperator::ClearConsole();
		COSOperator::Gotoxy(0, 0);

		Print("==============================================================");
		Print("==========================TestServer==========================");
		Print("==============================================================");

		BaseGateway* pGateway = new BaseGateway;
		AString sAddr = argv[1];
		Int32 iThreads = CStringUtil::StringToInt<AString>(argv[2]);

		if (pGateway->Init(sAddr, "127.0.0.1:7899", iThreads, 0))
		{
#ifdef _DEBUG
			g_DbgThread = new CThread(DoKeyDownExit);
			if (g_DbgThread)
				g_DbgThread->Start(pGateway);
#endif	

			if (argc >= 4)
			{
				Bool bSecurity = CStringUtil::StringToBool<AString>(argv[3]);
				if (bSecurity)
					pGateway->TurnGateSecurity();
			}

			pGateway->Run();
		}

		DEF_RELEASE(g_DbgThread);
		DEF_RELEASE(pGateway);
	}
	else
	{
		printf("usage: testserver <ip:port> <threads> <security>.\r\n");
		COSOperator::WaitKeyboardInput();
		return -1;
	}

	CSvrUtil::Stop();
	CSvrUtil::Release();

	return 0;
}
