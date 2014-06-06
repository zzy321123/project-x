#ifndef BASE_GATEPROXY_H
#define BASE_GATEPROXY_H

#include "BaseGateDef.h"

namespace Util
{
	/************************************************************************/
	/* 和网关匹配的连接端封装                                               */
	/************************************************************************/
	class GATE_API BaseGateProxy : public CRefCounter
	{
	public:
		//构造
		BaseGateProxy();

		//析构
		virtual ~BaseGateProxy();

		//线程私有句柄
		typedef map<UInt32, CZmq*>	ThreadProxyMap;

		//注册协议类型
		typedef map<ProtoType, UInt8>   ProtoTypeDriveMap;

	public:
		//初始化
		virtual Bool  Init(const AString& sAddr, UInt32 iSvrId, UInt32 iConnLimit = 0);

		//更新消息
		virtual Bool  Update(Int32 iTimeout = 0);

	public:
		//注册协议类型
		virtual Bool  RegisterProtoType(const ProtoTypeDriveMap& mTypeDrive);

		//注册可用线程
		virtual Bool  RegisterThreads(const vector<UInt32>& vThreads);

		//发送协议
		virtual Bool  SendProtocol(SID iSid, CProtocol* pProto);

		//发送协议
		virtual Bool  SendProtocol(const SvrMsgHeader& sHeader, CProtocol* pProto);		

		//发送数据
		virtual Bool  SendRawData(SID iSid, const OctetsStream* pData);

		//发送数据
		virtual Bool  SendRawData(const SvrMsgHeader& sHeader, const OctetsStream* pData);	

		//广播协议
		virtual Bool  BroadcastProto(const vector<SID>& vSid, CProtocol* pProto);

		//接收协议(只单纯接收协议, 一般不建议使用)
		virtual Bool  RecvProtocol(GateMsgHeader& sHeader, CProtocol*& pProto, Int32 iTimeout = -1);

		//关闭会话
		virtual Bool  CloseSession(SID iSid);

	protected:
		//连接到网关回调
		virtual Bool  OnProxyConnected();

		//接收协议回调(线程模式下,线程调用), 返回true即表示协议被应用接收, 应用负责释放, 否则立即释放
		virtual Bool  OnRecvProtocol(const GateMsgHeader& sHeader, CProtocol* pProto);

		//和网关断开连接回调
		virtual Bool  OnProxyDisconnect();

	protected:
		//检测网关连接状态(非线程模式下需手动调用)
		virtual Bool  CheckProxyState();

		//发送通知(SID为目标对象ID, 为0即表示对系统发)
		virtual Bool  SendNotify(const GateNotify& sNotify, const OctetsStream* pExtData = 0);

		//向网关发送默认注册消息
		virtual Bool  SendRegsterMsg();

		//获取线程私有通信对象
		CZmq*		  GetThreadProxy();		

	protected:
		//通用缓冲区
		OctetsStream*		m_pOctets;
		//网关地址
		AString				m_sGateAddr;
		//服务标识ID
		UInt32				m_iSvrId;
		//最大连接数限制
		UInt32				m_iConnLimit;
		//消息通道
		CZmq*				m_pProxyZmq;
		//是否和网关连接
		UInt32				m_iProxyState;
		//线程数据接收器
		CZmq*				m_pCollectZmq;
		//线程私有通信对象
		ThreadProxyMap		m_mThreadProxy;
		//注册协议列表
		ProtoTypeDriveMap	m_mProtoTypeDrive;
	};
}
#endif
