#ifndef BASE_GATESECURITY_H
#define BASE_GATESECURITY_H

#include "Security.h"

namespace Util
{
	/************************************************************************/
	/* �������ݰ�ȫ���                                                     */
	/************************************************************************/
	class GATE_API BaseGateSecurity : public CSecurity
	{
	public:
		//��ѹ����ѹ����
		BaseGateSecurity(UInt8 iType = SECURITY_NONE);

		//��ѹ����ѹ����
		virtual ~BaseGateSecurity();

	public:
		//���ܽ���
		enum
		{
			SECURITY_NONE = 0,
			SECURITY_ENCODE,
			SECURITY_DECODE,
		};

	public:
		//���ܸ����ֽ���
		virtual Bool  Update(COctets& xOctets);

	protected:
		//��ѹ���ӿ�
		virtual Bool  Encode(COctets& xOctets);

		//����ѹ�ӿ�
		virtual Bool  Decode(COctets& xOctets);

	protected:
		//����
		UInt8		m_iType;		
		//�м�����������
		COctets		m_sOctets;
		//������ָ��
		PVoid		m_pZStream;
	};
}
#endif
