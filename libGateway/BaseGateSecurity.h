#ifndef BASE_GATESECURITY_H
#define BASE_GATESECURITY_H

#include "Security.h"

namespace Util
{
	/************************************************************************/
	/* 网关数据安全组件                                                     */
	/************************************************************************/
	class GATE_API BaseGateSecurity : public CSecurity
	{
	public:
		//流压缩解压构造
		BaseGateSecurity(UInt8 iType = SECURITY_NONE);

		//流压缩解压析构
		virtual ~BaseGateSecurity();

	public:
		//加密解密
		enum
		{
			SECURITY_NONE = 0,
			SECURITY_ENCODE,
			SECURITY_DECODE,
		};

	public:
		//加密更新字节流
		virtual Bool  Update(COctets& xOctets);

	protected:
		//流压缩接口
		virtual Bool  Encode(COctets& xOctets);

		//流解压接口
		virtual Bool  Decode(COctets& xOctets);

	protected:
		//类型
		UInt8		m_iType;		
		//中间数据流参数
		COctets		m_sOctets;
		//流对象指针
		PVoid		m_pZStream;
	};
}
#endif
