#ifndef _PROTOCOLMANAGER_H_
#define _PROTOCOLMANAGER_H_

#include "ManagerBase.h"
#include "Protocol.h"

namespace Util
{
	/************************************************************************/
	/* Э���������װ														*/
	/************************************************************************/
	class UTIL_API CProtocolManager : public CManagerBase
	{
	protected:
		//����
		CProtocolManager();

		//����
		virtual ~CProtocolManager();

		//��������������
		SINGLETON_DECL(ProtocolManager);

	public:
		//Э��ע���
		typedef map<ProtoType, Protocol*>  ProtocolMap;

		//�Զ��ͷŷ�װ
		class UTIL_API Scope : public CRefCounter
		{
		public:
			Scope(CProtocol** ppProto = 0);

			virtual ~Scope();

		protected:
			CProtocol**  m_ppProto;
		};

	public:
		//����Э�����
		virtual CProtocol*		CreateProtocol(ProtoType iType);

		//����Э������
		virtual CProtocol*		Decode(COctetsStream& rhsOS);

		//ע��Э������
		virtual Bool		   Register(ProtoType iType, CProtocol* pProto);		

		//�ж�Э���Ƿ�ע��
		virtual Bool		   CheckProtocolLegal(ProtoType iType);

		//�ͷ�Э�����
		virtual Bool		   ReleaseProto(CProtocol*& pProto);

		//���������ļ�����Э�����
		virtual Bool		   GenProtocols(const AString& sCfgFile, const AString& sProtocolsHead = "SvrUtil.h");

		//ֱ�ӽ���Э������(��������DecodeStream����)
		virtual Bool		   IsAutoDecode() const;

		//�����Զ��������
		virtual void		   SetAutoDecode(Bool bAuto = true);

	public:
		//����Э���ͷ
		virtual Bool		   AnalyseProtocolHeader(const COctetsStream& xOS, ProtoType& iType, ProtoSize& iSize, ProtoCrc& iCrc);

		//�Ƿ��пɶ�Э���ͷ
		virtual Bool		   CheckDecodeProtocol(const COctetsStream& xOS, UInt32* pBodySize = 0);

		//��ȡЭ��ͷ�ֽڴ�С
		virtual UInt32		   GetProtoHeaderSize() const;

		//��ȡע��Э��ID�б�
		virtual UInt32		   GetRegProtoIds(vector<ProtoType>& vProtoIds, vector<UInt8>* pProtoDrive = 0);

		//ע��ϵͳ�ڲ�Э��
		virtual Bool		   RegSysProtocol();

		//����Э���ֽ�������(Ĭ��0Ϊ����)
		virtual Bool		   SetSizeLimit(UInt32 iSize);

		//���Э���ֽ����Ƿ�Ϸ�
		virtual Bool		   CheckSizeLimit(UInt32 iSize) const;

		//дЭ���ͷ
		virtual Bool		   ReadProtocolHeader(COctetsStream& xOS, ProtoType& iType, ProtoSize& iSize, ProtoCrc& iCrc);

		//��Э���ͷ
		virtual Bool		   WriteProtocolHeader(COctetsStream& xOS,ProtoType iType, ProtoSize iSize, ProtoCrc iCrc);

	protected:		
		//ע���Э���
		ProtocolMap				m_mRegister;
		//�ͷ��Զ�Э�����
		Bool					m_bAutoDecode;
		//Э���ͷ�ֽڴ�С
		UInt32					m_iProtoSize;
		//Э����ֽ�������
		UInt32					m_iSizeLimit;
	};

	#define g_ProtocolManager  CProtocolManager::GetInstance()

	//����ϵͳ����֧��Э���
	#define REGISTER_PROTO(protocol_class)\
	{\
		protocol_class* _ptr_ = new protocol_class;\
		g_ProtocolManager->Register(_ptr_->GetType(),_ptr_);\
		DEF_RELEASE(_ptr_);\
	}
}
#endif //_PROTOCOLMANAGER_H_
