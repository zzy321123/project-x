#ifndef C_MSGMANAGER_H
#define C_MSGMANAGER_H

#include "CMsg.h"
#include "ManagerBase.h"

namespace Util
{
	/************************************************************************/
	/* 消息管理器,在创建和释放消息之前,必须注册作用的消息类型               */
	/************************************************************************/
	class UTIL_API CMsgManager : public CManagerBase
	{
	protected:
		//构造
		CMsgManager();

		//析构
		virtual ~CMsgManager();

		//管理器单例申明
		SINGLETON_DECL(MsgManager);

		//注册列表
		typedef map<Int32, Int32> MsgRegMap;

	public:
		//注册消息类型
		virtual Bool	 Register(Int32 iMsg);	

		//获取消息
		virtual CMsg*	GetMsg(Int32 iMsg);

		//释放消息
		virtual Bool     FreeMsg(CMsg*& pMsg);

		//获取注册协议ID列表
		virtual UInt32	 GetRegMsgIds(vector<Int32>& vMsgIds);

	protected:		
		//注册类型列表
		MsgRegMap m_mReg;
	};

	#define g_MsgManager  CMsgManager::GetInstance()
}
#endif
