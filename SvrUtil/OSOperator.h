#ifndef _OSOPERATOR_H
#define _OSOPERATOR_H

#include "RefCounter.h"

namespace Util
{
	/************************************************************************/
	/* �Ͳ���ϵͳ��غ�����װ													*/
	/************************************************************************/
	class UTIL_API COSOperator : public CRefCounter
	{
	public:
		//���ù���·��
		static Bool SetWorkDir(const AString& sDir);

		//��ù���·��
		static AString GetWorkDir();

		//���ϵͳTick������(CPU)
		static UInt32 GetTickCount();

		//��ȡϵͳʱ��
		static tm GetSysClock();
	
		//��ȡϵͳʱ������
		static time_t GetSysTime();

		//���ʱ���ַ���:2008-08-08 08:08:08
		static AString GetTimeString();

		//Sleep��װ
		static void OSSleep(Int32 iTime);		

		//���ļ�·����ȡ�ļ���
		static AString SplitFileName(const AString& sFile);

		//���ļ�·����ȡ�ļ�·��
		static AString SplitFilePath(const AString& sFile);

		//��������
		static AString SetLocale(Int32 iCategory, const AString& sLocal);

		//�Ƿ����ĳ�ļ�
		static Bool ExistFile(const AString& sFile);

		//�Ƿ����ĳĿ¼
		static Bool ExistFolder(const AString& sFolder);

		//����ļ���С
		static Int64 GetFileSize(const AString& sFile);

		//����CRC
		static UInt32  CalcCrc(const UChar* pData, UInt32 iSize, UInt32* pCrc = 0);

		//�����ļ�CRC
		static UInt32  CalcFileCrc(const AString& sFile);

		// CPU��Little endian(1)����Big endian(0)ģʽ 
		static Int32   CheckCpuEndian();

		//�����ļ�
		static Bool OsCopyFile(const AString& sFile, const AString& sNewFile);

		//ɾ���ļ�
		static void OsDeleteFile(const AString& sFile);

		//�������ļ�
		static Bool RenameFile(const AString& sOldName, const AString& sNewName);

		//�����ļ���д
		static Bool SetFileWritable(const AString& sFile);

		//����Ŀ¼��
		static Bool CreateDir(const AString& sDir);		

		//��֤�ļ�Ŀ¼������
		static void MakeSureFilePath(const AString& sFolderPath);

		//��֤�ļ���ǰ׺Ŀ¼������
		static void MakeSureFileName(const AString& sFilePath);

		//��������
		static AString ConsoleInput();

		//getch��װ
		static Int32 WaitKeyboardInput(const AString& sInfo = "", Bool bPrint = true);

		//�ػ���������
		static Bool  Daemonize(Bool bChdir = true, Bool bClose = true);

		//��ȡ�ں��߳�ID
		static UInt32 GetThreadId();

		//��ȡ����ID
		static UInt32 GetProcessId();

		//��ӡ�������λ��
		static void Gotoxy(Short iX, Short iY);

		//���ؿ��ƵĹ��
		static void HideCursor();

		//��տ���̨
		static void ClearConsole();

		static UInt32 GetCPUCount();

#ifdef PLATFORM_WINDOWS
	public:
		//CtrDebug
		static void SetCrtDebugMode();

		//ִ��Dos����
		static void ExecuteDosCmd(const AString& sDosCmd);

		//Socket����(���̼乲��,Copyһ��ֻ����һ��)
		static Bool CopySocket(SOCKET hSock, UInt32 iProcessId, WSAPROTOCOL_INFO* pInfo);

		//Socketճ��(���̹������)
		static SOCKET PasteSocket(WSAPROTOCOL_INFO* pInfo);
#endif
	};

	//�����Ϊ���׺���
	#define CSleep(ms)   COSOperator::OSSleep(ms)
	#define CSysTime()   COSOperator::GetSysTime()
	#define CTickCount() COSOperator::GetTickCount()
}
#endif //MY_OSOPERATOR_H
