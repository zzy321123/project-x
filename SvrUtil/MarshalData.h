#ifndef _MARSHALDATA_H_
#define _MARSHALDATA_H_

#include "Marshal.h"

namespace Util
{
	/************************************************************************/
	/* 可序列化数据对象,主要用在Protocol中的数据结构							*/
	/************************************************************************/
	class UTIL_API CMarshalData : public CMarshal
	{
	public:
		//可序列化对象克隆函数
		virtual CMarshalData* Create() const;

		//可序列化对象赋值函数
		virtual CMarshalData& operator = (const CMarshalData& rhsMD);
	};

	//简化类型定义
	typedef CMarshalData MarshalData;
}
#endif //_MARSHALDATA_H_
