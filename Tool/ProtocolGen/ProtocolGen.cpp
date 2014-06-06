#include "SvrUtil.h"

int main(int argc, Char* argv[])
{
	CSvrUtil::Init();

	Char sFile[256]   = {0};
	AString sFileHead = "SvrUtil.h";

	if (argc < 2)
	{
		if (COSOperator::ExistFile("Protocol.xml"))
		{
			strcpy(sFile, "BaseProtocol.xml");
		}
		else if (COSOperator::ExistFile("GameProtocol.xml"))
		{
			strcpy(sFile, "GameProtocol.xml");
		}
		else
		{
			Print("Input Protocol XmlFile: ");
			scanf("%s",sFile);
		}
	}
	else
	{
		//读取协议文件
		if (argc >= 2)
			memcpy(sFile, argv[1], strlen(argv[1]));

		//读取通用头文件
		if (argc >= 3)
			sFileHead = argv[2];
	}
	
	if (!g_ProtocolManager->GenProtocols(sFile, sFileHead))
	{
		Print("\r\nGen Protocols Failed.");
	}
	else
	{
		Print("\r\nGen Protocols Success.");
#ifdef PLATFORM_WINDOWS
		if (argc != 2)
		{
			COSOperator::WaitKeyboardInput();
		}
#endif
	}

	CSvrUtil::Stop();
	CSvrUtil::Release();

	return 0;
}
