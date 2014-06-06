#ifndef C_PROFILER_H
#define C_PROFILER_H

#include "SvrUtil.h"

#ifndef PRO_API
#define PRO_API
#endif

namespace Util
{
	/************************************************************************/
	/* �������ܼ����                                                       */
	/************************************************************************/
	class PRO_API CProfiler : public CRefCounter
	{
	public:
		//����
		CProfiler(const AString& sAddr = "");

		//����
		virtual ~CProfiler();

	public:
		//��ʼ������
		virtual Bool   Start(const AString& sAddr = "");

		//ֹͣ���
		virtual Bool   Stop();

	public:
		//�Ǽǽ���Э��
		virtual Bool   RegRecvProto(ProtoType iType, ProtoSize iSize);

		//�ǼǷ���Э��
		virtual Bool   RegSendProto(ProtoType iType, ProtoSize iSize);

		//�Ǽ�����
		virtual Bool   RegConnect(Bool bConn);

		//��ȡCPU����
		virtual UInt32 GetProcessorNumber();

		//��ȡ�����ڴ���
		virtual UInt64 GetTotalMem();

		//��ȡCPUʹ����
		virtual UInt32 GetCpuUsage();

		//��ȡռ���ڴ��С
		virtual UInt64 GetMemUsage();

	public:
		//ִ�з����߳�ѭ��
		virtual Bool   RunLoop();

	protected:
		//�Ƿ�������
		volatile Bool m_bRunning;
		//�����ַ
		AString		  m_sAddr;
		//�����׽���(UDPģʽ)
		CSocket	  m_sSocket;
		//�����߳�
		CThread*	  m_pThread;
		//CPU����
		UInt32		  m_iCpuCount;
		//�������ڴ�
		UInt64		  m_iTotalMem;
		//CPUʹ����
		UInt32		  m_iCpuUsage;
		//�ڴ�ʹ����
		UInt64		  m_iMemUsage;
		//��Ч������
		UInt32		  m_iConnect;
		//Э����ո���
		UInt64		  m_iRecvProto;
		//Э������ֽ�
		UInt64		  m_iRecvSize;
		//Э�鷢�͸���
		UInt64		  m_iSendProto;
		//Э�鷢���ֽ�
		UInt64		  m_iSendSize;
		//UDP���ݽ���Buffer
		OctetsStream* m_pRecvBuf;
	};
}
#endif
