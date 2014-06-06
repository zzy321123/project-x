#ifndef _LOGGER_H
#define _LOGGER_H

#include "Mutex.h"

namespace Util
{
	/************************************************************************/
	/* �̰߳�ȫ��־��,����������־��¼											*/
	/************************************************************************/
	class UTIL_API CLogger : public CRefCounter
	{
	protected:
		//����
		CLogger(const AString& sFile, Bool bAppend = false);

		//����
		virtual ~CLogger();
	
		//��������Ա����
		friend class CLoggerManager;

	public:
		//��־�ȼ�
		enum LogLevel
		{
			//��Ϣ
			LEVEL_INFO,
			//����
			LEVEL_WARNING,
			//����
			LEVEL_ERROR
		};
		
	public:
		//��¼��Ϣ
		virtual void LogMsg(const AString& sMsg, LogLevel eLevel = LEVEL_INFO);

		//��¼��ʽ����Ϣ
		virtual void LogFmtMsg(const Char* pFmt, ...);

		//��¼��ʽ������
		virtual void LogFmtErr(const Char* pFmt, ...);

	protected:
		//��¼ʱ����Ϣ
		virtual AString GetLogTimeInfo(LogLevel eLevel) const;

		//��ȡ��־���������
		virtual AString GetLogLevelDesc(LogLevel eLevel) const;

	private:
		//��־����Ӧ���ļ����
		FILE* m_pFile;
		//��־�ļ���
		AString m_sFileName;
		//�Ƿ�Ϊ׷��ģʽ
		Bool m_bAppend;
		//��־�̰߳�ȫ����
		CMutex* m_pMutex;
	};
}
#endif //_LOGGER_H
