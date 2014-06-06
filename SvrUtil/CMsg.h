#ifndef C_MSG_H
#define C_MSG_H

#include "XID.h"
#include "CParamVector.h"

namespace Util
{
	/************************************************************************/
	/* 对象消息封装处理                                                     */
	/************************************************************************/
	class UTIL_API CMsg : public CRefCounter
	{
	public:
		//构造
		CMsg(Int32 iMsg = 0, const XID& sTarId = XID(), const XID& sSrcId = XID());

		//析构
		virtual ~CMsg();

		//赋值操作符(不建议使用赋值和拷贝构造,会引发ParamVector的拷贝操作)
		CMsg& operator = (const CMsg& sMsg);

		//参数队列类型定义
		typedef  CParamVector ParamVector;

		//创建标记
		enum
		{			
			SYS_CREATE = 0x01,
		};

	public:
		//参数队列拷贝
		virtual Int32 CopyParams(const ParamVector& vParams);

		//判断消息是否有效
		virtual Bool  IsValid() const;

		//清理参数
		virtual Bool  Clear();
		
	public:
		//消息数据信息
		Int32		Msg;
		//消息发送时间
		UInt32		Time;
		//消息目标ID
		XID			Target;
		//消息来源ID
		XID			Source;
		//消息参数列表
		ParamVector	Params;
		//消息系统标记(应用不可操作)
		UInt32		SysFlag;
		//消息用户标记
		UInt32		UsrFlag;
	};	
}
#endif
