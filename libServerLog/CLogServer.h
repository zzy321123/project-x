#ifndef C_LOGSERVER_H
#define C_LOGSERVER_H

#include "SvrUtil.h"

namespace Util
{
	/************************************************************************/
	/* ��־��������װ, ���ݿ�ģʽ��Ĭ��дlogs��                             */
	/************************************************************************/
	class UTIL_API CLogServer : public CRefCounter
	{
	public:
		//����
		CLogServer();

		//����
		virtual ~CLogServer();

	public:
		//��ʼ���ļ���־
		virtual Bool Init(const AString& sSvrAddr, const AString& sLogFile, Int32 iCacheSize = MEGABYTE, Int32 iCacheTime = 60000);

		//��ʼ�����ݿ���־
		virtual Bool Init(const AString& sSvrAddr, const CDBConn& sConn, Int32 iCacheSize = MEGABYTE, Int32 iCacheTime = 60000);

		//������־������
		virtual Bool Run();

		//�ر���־������
		virtual Bool Stop();

		//������־��ӡ���
		virtual Bool EnableConsole(Bool bEnable);

	protected:
		//������־
		virtual Bool CacheLogs(Int32 iSvrId, UInt32 iUsrId, const UString& sUsrName, Int32	iType, const UString& sKey, const UString& sMsg);

		//��־���
		virtual Bool FlushLogs();

	protected:
		//����״̬
		volatile Bool m_bRunning;
		//��ӡ����
		Bool		  m_bConsole;
		//UDPģʽ��־����
		CSocket		  m_sSocket;					
		//UDP���ݽ���Buffer
		OctetsStream* m_pRecvBuf;
		//��־����Buffer
		COctets*	  m_pLogCache;
		//��־��ʽ��Buffer
		Utf8*		  m_pFmtBuf;
		//��־�洢���ݿ�
		CDatabase*	  m_pLogDB;
		//��־�洢�ļ�
		CDiskFile*	  m_pLogFile;
		//����ˢ���¼�
		Int32		  m_iCacheTime;
	};
}
#endif
