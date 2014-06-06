#ifndef _MARSHAL_H_
#define _MARSHAL_H_

#include "OctetsStream.h"

namespace Util
{
	/************************************************************************/
	/* 数据流的序列化和反序列化基类封装										*/
	/************************************************************************/
	class UTIL_API CMarshal : public CRefCounter
	{
	public:
		//数据打包(子类须重载)
		virtual COctetsStream& Marshal(COctetsStream& rhsOS) ;

		//数据解包(子类须重载)
		virtual COctetsStream& Unmarshal(COctetsStream& rhsOS) ;

		//清理数据
		virtual Bool Clear();
	};
}
#endif //_MARSHAL_H_
