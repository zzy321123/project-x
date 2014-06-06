#ifndef _LOGGERMANAGER_H
#define _LOGGERMANAGER_H

#include "ManagerBase.h"
#include "Logger.h"

namespace Util
{
	/************************************************************************/
	/* ��־��������װ														*/
	/************************************************************************/
	class UTIL_API CLoggerManager : public CManagerBase
	{
	protected:
		//��־����������
		CLoggerManager();

		//��־����������
		virtual ~CLoggerManager();

		//����������
	protected:
		static CLoggerManager* m_pLoggerManager;
	public:											
		static Bool InitInstance();			
		static CLoggerManager* GetInstance();					
		static Bool ExitInstance();
	
	public:
		//��־�����Ͷ���
		typedef map<AString, CLogger*> LoggerMap;	

	public:
		//����һ����־��¼��
		CLogger*   CreateLogger(const AString& sFile, const AString& sName = "", Bool bAppend = false);	

		//��ȡһ����־��¼��
		CLogger*	GetLogger(const AString& sName);

	public:
		//������־�ļ�·��(��'/'��β)
		virtual Bool  SetupLogs(const AString& sDir = "Logs/");

		//Print��Ϣ
		virtual void  Print(const AString& sMsg);

		//���ÿ���̨��ӡ���
		virtual void  EnableConsole(Bool bEnable);

		//���ô�ӡ��Ϣ������־
		virtual void  EnablePrintLog(Bool bEnable = true);

	protected:
		//��־�ļ�·��
		AString		m_sLogDir;
		//��־ӳ���
		LoggerMap	m_mLoggerMap;
		//�Ƿ������ӡ���
		Bool		m_bConsole;
		//����̨�����־
		CLogger*	m_pPrintLog;
	};

	//����̨��ӡ��Ϣ
	void  UTIL_API Print(const AString& sMsg, Bool bNewLine = true);
	void  UTIL_API FmtPrint(const Char* sFmt, ...);
	void  UTIL_API FmtPrintLine(const Char* sFmt, ...);

	//ϵͳĬ����־��¼
	void  UTIL_API Log(const AString& sMsg, CLogger::LogLevel eLevel = CLogger::LEVEL_INFO, const AString& sName = "Util");
	void  UTIL_API FmtLog(const Char* sFmt,  ...);

	//ϵͳĬ�ϴ����¼
	void  UTIL_API Error(const AString& sMsg, const AString& sName = "Error");
	void  UTIL_API FmtError(const Char* sFmt, ...);
	
	//��־ȫ�ֹ���������
	#define g_LoggerManager CLoggerManager::GetInstance()
}
#endif //MY_LOGGERMANAGER_H
