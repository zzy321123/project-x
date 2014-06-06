#ifndef _SECURITY_H_
#define _SECURITY_H_

#include "Octets.h"

namespace Util
{
	/************************************************************************/
	/* 数据加班加密接口基类定义												*/
	/************************************************************************/
	class UTIL_API CSecurity : public CRefCounter
	{
	protected:
		//加密对象构造
		CSecurity();

		//加密对象析构
		virtual ~CSecurity() ;

	public:
		//设置加密密钥
		virtual void  SetParameter(const COctets& xOctets);

		//获取加密密钥
		virtual void  GetParameter(COctets& xOctets);	

		//加密更新字节流
		virtual Bool  Update(COctets& xOctets);
	};
}
#endif //_SECURITY_H_
