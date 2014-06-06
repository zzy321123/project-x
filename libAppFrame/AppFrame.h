#ifndef _APPFRAME_H_
#define _APPFRAME_H_

#include "SvrUtil.h"
#include "CLogProxy.h"
#include "AppGateway.h"
#include "AppGateProxy.h"

namespace Util
{
	/************************************************************************/
	/* �������������									                    */
	/************************************************************************/
	class FRAME_API CAppFrame : public CApp
	{
	public:
		//����
		CAppFrame(const XID& sXid = XID());

		//����
		virtual ~CAppFrame();

		//�߳�ʹ�����ݿ����
		typedef map<UInt32, CMysql*>   DBHdlMap;
		
		//��ȡ�Ự�ĵ�ַ����
		typedef CLockMap<SID, AString> SidAddrMap;

		//��Ա����
		friend class CAppGateway;
		friend class CAppGateProxy;

		//App��ʼ���ṹ��
		struct AppCfg
		{
			//Ӧ��ID, ������ID, ��ID
			UInt32		AppId;
			//�Ƿ�������̨��ӡ
			Bool		Console;
			//�߼��߳���, ����IO�߳���
			UInt32		Threads;			
			//��־��������ַ
			AString		LogAddr;			
			//mysql���ݿ����ӵ�ַ
			CDBConn		DBConn;

			//���ش���������
			struct  
			{
				//���ش�������ַ, ����: tcp://
				AString		Address;
				//����������
				UInt32		ConnLimit;
			}Gateproxy;
			

			//��Ƕ����ģʽ��������
			struct 
			{
				//��Ƕ���ط�������ַ(ip:port)
				AString		Address;
				//�����߳���
				UInt32		Threads;
				//����ģʽ
				Int32		Mode;
				//ƽ̨
				Int32		Platform;
				//���ؼ���ģʽ
				Bool		Security;
				//���ػỰ��ʱʱ��
				Int32		Timeout;
				//�������ܼ�����
				AString		Profiler;
			}Gateway;

			AppCfg()
			{
				AppId	= 0;
				Threads = 0;
				Console = false;

				Gateproxy.Address	= "";
				Gateproxy.ConnLimit = 0;

				Gateway.Address		= "";
				Gateway.Threads		= 0;
				Gateway.Mode		= 0;
				Gateway.Platform	= 0;
				Gateway.Security	= false;
				Gateway.Timeout		= 0;
				Gateway.Profiler	= "";
			}
		};

	public:
		//��ʼ��
		virtual Bool		Init(const AppCfg& sCfg);

		//����
		virtual Bool		Run();

		//ֹͣ
		virtual Bool		Stop();

	public:
		//��ȡӦ��ID
		virtual UInt32		GetAppId() const;

		//��ȡ��ǰ����
		virtual UInt32		GetSysTime() const;
		
		//��ȡ����ʱ��
		virtual UInt32		GetTickTime() const;

		//��ȡ֡����
		virtual UInt32      GetElapseTime() const;

		//��ȡ�Ự��IP��ַ
		virtual AString		GetSidAddr(SID iSid);

		//�˳�ѭ��
		virtual Bool		BreakLoop();

		//��ȡ��ǰ�̰߳�ȫ�����ݿ�����
		CMysql*				GetSafeDbHdl();

		//��ȡӦ������
		const AppCfg*		GetAppCfg() const;

	public:
		//����Э��
		virtual Bool		SendProtocol(SID iSid, Protocol* pProto);
		
		//�رջỰ
		virtual Bool		CloseSession(SID iSid);
		
		//�㲥��Ϣ
		virtual Bool		BroadcastMsg(ObjMan* pObjMan, CMsg* pMsg);
		
		//�㲥Э��
		virtual Bool		BroadcastProto(Protocol* pProto, const vector<SID>& vSid = vector<SID>());	

	public:
		//��־ģ��ӿ�(��Ϣ)
		virtual Bool		FormatMsg(const Char* pKey, const Char* pFmt, ...);

		//��־ģ��ӿ�(����)
		virtual Bool		FormatWarn(const Char* pKey, const Char* pFmt, ...);

		//��־ģ��ӿ�(����)
		virtual Bool		FormatError(const Char* pKey, const Char* pFmt, ...);

		//��־ģ��ӿ�(��Ϣ)
		virtual Bool		FormatMsg(UInt32 iUsrId, const Char* pUsrName, const Char* pKey, const Char* pFmt, ...);

		//��־ģ��ӿ�(����)
		virtual Bool		FormatWarn(UInt32 iUsrId, const Char* pUsrName, const Char* pKey, const Char* pFmt, ...);

		//��־ģ��ӿ�(����)
		virtual Bool		FormatError(UInt32 iUsrId, const Char* pUsrName, const Char* pKey, const Char* pFmt, ...);

	protected:
		//ÿ֡����
		virtual Bool		OnTick(UInt32 iPeriod);

		//�������ӳɹ�(����ģʽ�ص�)
		virtual Bool		OnGateProxyConnected();

		//�Ự����(����ģʽ�ص�)
		virtual Bool		OnSessionStart(SID iSid, const AString& sAddr);

		//�ỰЭ��֪ͨ(����ģʽ�ص�)
		virtual Bool		OnSessionProtocol(SID iSid, Protocol* pProto);

		//�Ự����֪ͨ(����ģʽ�ص�)
		virtual Bool		OnSessionClose(SID iSid, const AString& sAddr);
		
		//�������ӹر�(����ģʽ�ص�)
		virtual Bool		OnGateProxyDisconnect();

	protected:
		//��������(������ģʽ, ����True���������ӣ�����ֱ�ӹر�)
		virtual Bool		OnAcceptConnect(const CSocket& sSocket, const SocketAddr& sAddr);

	protected:
		//Ӧ�ò㴴������(Ӧ�ò����ʵ��)
		virtual CAppObj*	AppCreateObj(const XID& sXid);

	protected:
		//Ӧ������
		AppCfg				m_sAppCfg;
		//Ӧ��ID
		UInt32				m_iAppId;
		//��ǰϵͳʱ����
		UInt32				m_iSysTime;
		//��ǰϵͳʱ�����
		UInt32				m_iTickTime;
		//�߼�֡ʱ��
		UInt32				m_iElapseTime;
		//�Ự��ַ��
		SidAddrMap			m_mSidAddr;
		//���ݿ�������Ϣ
		CDBConn				m_sDbConn;
		//���ݿ���ӳ��
		DBHdlMap			m_mDbHdl;
		//֪ͨ�˳����
		volatile Bool		m_bBreakLoop;

	protected:
		//��־����
		CLogProxy*			m_pLogProxy;
		//��Ƕ���ط�����
		CAppGateway*		m_pGateway;
		//���ض���
		CAppGateProxy*		m_pGateProxy;
	};
}
#endif
