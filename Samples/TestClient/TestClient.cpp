#include "TestProto.h"

#define PRINT_PROTO 1
#define PROTO_TIME	200

volatile Bool		g_Running = true;
AString				g_IpAddr  = "";
vector<CThread*>	g_Threads;

PVoid fun_TestClientRoutine(void* pArg)
{
	CSession session;
	if (session.Init(g_IpAddr))
	{
		while (g_Running)
		{
			//更新
			if (!session.Tick(PROTO_TIME))
				break;

			//发送协议
			Int32 iType = CRand::RandInt(PROTO_A, PROTO_C);
			if (iType == PROTO_A)
			{
				ProtoA sCmd;
				session.SendProtocol(&sCmd);
			}
			else if (iType == PROTO_B)
			{
				ProtoB sCmd;
				session.SendProtocol(&sCmd);
			}
			else if (iType == PROTO_C)
			{
				ProtoC sCmd;
				session.SendProtocol(&sCmd);
			}			

			//接收协议
			Protocol* pProto = 0;
			while (session.DecodeProtocol(pProto) && pProto)
			{
#if (PRINT_PROTO == 1)
				ProtoType iType = pProto->GetType();
				if (iType == PROTO_A)
				{
					ProtoA* pA = (ProtoA*)pProto;
					FmtPrint("Msg: %s",pA->m_sMsg.c_str());
				}
				else if (iType == PROTO_B)
				{
					ProtoB* pB = (ProtoB*)pProto;
					FmtPrint("Size: %d, Time: %d",pB->m_iSize, pB->m_iTime);
				}
				else if (iType == PROTO_C)
				{
					ProtoC* pC = (ProtoC*)pProto;
					FmtPrint("X: %f, Y: %f, Z: %f",pC->m_fX, pC->m_fY, pC->m_fZ);
				}
#endif
				g_ProtocolManager->ReleaseProto(pProto);
			}

			Sleep(PROTO_TIME);
		}
	}

	return 0;
}

bool IsValidNickNameChar(char ch)
{
	if( ch==32 || ch==33 || ch==34 || ch==39 || ch==42 || ch==45 || ch==47 || ch==96 || ch==61 || ch==59 ) // !"'*-/`=;
		return false;
	return true;
}

int main(int argc, Char* argv[])
{
	{
		std::string strIn = "法师";
		std::vector< std::string > vecFilterTable;
		vecFilterTable.push_back(std::string("*法*轮*功*"));
		wchar_t wsz_temp[256] = {0};
		CStringUtil::AsciiToUnicode(strIn.c_str(), strIn.length(), wsz_temp, 256);
		std::wstring wstr_In = wsz_temp;
		size_t allsize = vecFilterTable.size();
		for ( size_t i = 0; i < allsize; ++i )
		{
			memset(wsz_temp, 0, 256);
			CStringUtil::AsciiToUnicode(vecFilterTable[i].data(), vecFilterTable[i].length(), wsz_temp, 256);
			if ( std::wstring::npos != wstr_In.find(wsz_temp) )
				return FALSE; //发现非法字符串
		}
	}


	CSvrUtil::Init();
	
	REGISTER_PROTO(ProtoA);
	REGISTER_PROTO(ProtoB);
	REGISTER_PROTO(ProtoC);

	if (argc >= 2) 
		g_IpAddr = argv[1];

	if (!g_IpAddr.size())
		g_IpAddr = COSOperator::ConsoleInput();

	Int32 iCount = 1;
	if (argc >= 3)
		iCount = CStringUtil::StringToInt<AString>(argv[2]);

	for (Int32 i=0;i<iCount;i++)
	{
		CThread* pThread = new CThread(fun_TestClientRoutine);
		pThread->Start();
		g_Threads.push_back(pThread);

		CSleep(10);
	}	

	while( COSOperator::WaitKeyboardInput("",false) != 'Q' );
	
	g_Running = false;

	for (Size_t i=0;i<g_Threads.size();i++)
	{
		g_Threads[i]->Close();
		DEF_RELEASE(g_Threads[i]);
	}

	CSvrUtil::Stop();
	CSvrUtil::Release();
	return 0;
}

