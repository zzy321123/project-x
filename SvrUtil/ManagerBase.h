#ifndef _MANAGERBASE_H_
#define _MANAGERBASE_H_

#include "RefCounter.h"

namespace Util
{
	/************************************************************************/
	/* 管理器对象的基类定义													*/
	/************************************************************************/
	class UTIL_API CManagerBase : public CRefCounter
	{
	public:
		//构造
		CManagerBase();

		//析构
		virtual ~CManagerBase();

	public:
		//开启管理器
		virtual Bool Start();

		//更新管理器
		virtual Bool Tick(UInt32 iPeriod);

		//判断管理器是否运行
		virtual Bool IsRunning() const;
		
		//停止管理器运行,尽量在Stop里进行资源释放
		virtual Bool Stop();

	protected:
		volatile Bool m_bRunning;
	};
}

#endif //_MANAGERBASE_H_
