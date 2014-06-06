#ifndef C_LOGPROXY_H
#define C_LOGPROXY_H

#include "SvrUtil.h"

namespace Util
{
	/************************************************************************/
	/* ��־��¼����˷�װ(��־����ΪUtf8��ʽ)                               */
	/************************************************************************/
	class UTIL_API CLogProxy : public CRefCounter
	{
	public:
		//����
		CLogProxy();

		//����
		virtual ~CLogProxy();

		//��־���
		enum LogType
		{
			LT_MSG = 0,
			LT_WARN,
			LT_ERROR,
		};

	public:
		//��־�����ʼ��
		virtual Bool  Init(const AString& sAddr, UInt32 iSvrId = 0);		

	public:
		//��ͨ��־(Utf8����)
		virtual Bool  FmtMsg(const Char* pKey,const Char* pFmt, ...);

		//������־(Utf8����)
		virtual Bool  FmtWarn(const Char* pKey,const Char* pFmt, ...);

		//������־(Utf8����)
		virtual Bool  FmtError(const Char* pKey,const Char* pFmt, ...);

		//��ͨ��־(Utf8����)
		virtual Bool  FmtMsg(UInt32 iUsrId, const Char* pUsrName, const Char* pKey,const Char* pFmt, ...);
		
		//������־(Utf8����)
		virtual Bool  FmtWarn(UInt32 iUsrId, const Char* pUsrName, const Char* pKey,const Char* pFmt, ...);
		
		//������־(Utf8����)
		virtual Bool  FmtError(UInt32 iUsrId, const Char* pUsrName, const Char* pKey,const Char* pFmt, ...);

	protected:
		//��־���ͽӿ�(Utf8����)
		virtual Bool  SendLog(UInt8 iType, const Char* pKey, const Char* pMsg);

		//��־���ͽӿ�(Utf8����)
		virtual Bool  SendLog(UInt32 iUsrId, const Char* pUsrName, UInt8 iType, const Char* pKey, const Char* pMsg);

	public:
		//���ñ��ش�ӡ
		virtual void  EnableConsole(Bool bEnable);

		//���ô�ӡ�߳���Ϣ
		virtual void  SetShowThread(Bool bShow);

	protected:
		//�̰߳�ȫ������
		CMutex*	m_pLock;
		//��־�����׽���
		CSocket	m_sSocket;
		//��־��¼Զ�̵�ַ
		SocketAddr	m_sAddr;
		//��־����ID��ʶ
		UInt32		m_iSvrId;
		//�Ƿ񱾵ش�ӡ
		Bool		m_bConsole;
		//�Դ�ӡ��ʾ�߳�ID
		Bool		m_bShowThread;
	};
}
#endif
