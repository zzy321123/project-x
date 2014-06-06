#ifndef C_MSG_H
#define C_MSG_H

#include "XID.h"
#include "CParamVector.h"

namespace Util
{
	/************************************************************************/
	/* ������Ϣ��װ����                                                     */
	/************************************************************************/
	class UTIL_API CMsg : public CRefCounter
	{
	public:
		//����
		CMsg(Int32 iMsg = 0, const XID& sTarId = XID(), const XID& sSrcId = XID());

		//����
		virtual ~CMsg();

		//��ֵ������(������ʹ�ø�ֵ�Ϳ�������,������ParamVector�Ŀ�������)
		CMsg& operator = (const CMsg& sMsg);

		//�����������Ͷ���
		typedef  CParamVector ParamVector;

		//�������
		enum
		{			
			SYS_CREATE = 0x01,
		};

	public:
		//�������п���
		virtual Int32 CopyParams(const ParamVector& vParams);

		//�ж���Ϣ�Ƿ���Ч
		virtual Bool  IsValid() const;

		//�������
		virtual Bool  Clear();
		
	public:
		//��Ϣ������Ϣ
		Int32		Msg;
		//��Ϣ����ʱ��
		UInt32		Time;
		//��ϢĿ��ID
		XID			Target;
		//��Ϣ��ԴID
		XID			Source;
		//��Ϣ�����б�
		ParamVector	Params;
		//��Ϣϵͳ���(Ӧ�ò��ɲ���)
		UInt32		SysFlag;
		//��Ϣ�û����
		UInt32		UsrFlag;
	};	
}
#endif
