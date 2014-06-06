#ifndef _PROTOCOL_H_
#define _PROTOCOL_H_

#include "MarshalData.h"
#include "Socket.h"

namespace Util
{
	/************************************************************************/
	/* Э���ʽ���������װ													*/
	/************************************************************************/
	class UTIL_API CProtocol : public CMarshal
	{
	public:
		//��������
		enum DriveType
		{
			DRIVE_EVENT = 0,	//�¼�(ÿ�η���)
			DRIVE_STATE,		//״̬(�ı䷢��)
		};

	public:
		//Э�鹹��
		CProtocol(ProtoType iType = 0, UInt8 iDrive = DRIVE_EVENT);

		//Э������
		virtual ~CProtocol();		

	public:
		//��¡Э��(ʵ��Э�����ʵ��)
		virtual CProtocol* Clone() const = 0;

	public:		
		//Э����: Type + Size + Crc + Data
		virtual Bool  Encode(COctetsStream& rhsOS);

		//Э����
		virtual Bool  Decode(COctetsStream& rhsOS);		

		//����Э��
		//virtual Bool  Send(SID iSid, const SocketAddr* pAddr = 0);

		//��ȡ�Խ�Buffer
		virtual Bool  GetDecodeOS(OctetsStream*& pOS);
		
	public:
		//��ȡЭ������
		ProtoType	  GetType() const;

		//��ȡ�ֽ���
		ProtoSize     GetSize() const;

		//��ȡЭ������У��CRC
		ProtoCrc	  GetCrc() const;

		//��ȡ����ģʽ
		UInt8		  GetDrive() const;

	protected:
		//�Խ���,Ĭ������»��Խ���
		virtual Bool  DecodeSelf();

	protected:
		//����
		ProtoType	 m_iType;
		//�ֽ���
		ProtoSize	 m_iSize;
		//У����
		ProtoCrc	 m_iCrc;
		//����
		UInt8		 m_iDrive;
		//���ݴ洢��
		OctetsStream m_sDecode;
	};

	//Э�����ͼ�㶨��
	typedef CProtocol Protocol;
}
#endif //_PROTOCOL_H_
