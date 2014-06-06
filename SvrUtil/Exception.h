#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#include "BaseHeader.h"
#include "UtilMalloc.h"

namespace Util
{
	/************************************************************************/
	/* 异常定义类,负责系统异常的处理和记录									*/
	/************************************************************************/
	class UTIL_API CException : public std::exception
	{
	protected:												
		AString m_sMsg;							
	public:													
		inline const AString& GetMsg() const		
		{											
			return m_sMsg;									
		}											
		inline void SetMsg(const AString& value)	
		{											
			m_sMsg = value;								
		}

		struct ShowFileLine 
		{
			ShowFileLine(const AString& sTag, const AString& sFile, const AString& sFunc, Int32 iLine);
		};

	public:
		//构造异常信息
		CException(const AString& sMsg,const AString& sFile, const AString& sFunc, Int32 iLine);

		//对象析构
		virtual ~CException() throw();

		//异常抛弃
		Bool CException::Throw() const;
	};

	//抛出异常
	#define T_Exception(msg)	throw CException(msg,__FILE__,__FUNCTION__,__LINE__)
	//判断val为false时抛出异常
	#define A_Exception(cond)	if(!(cond)) throw CException(#cond,__FILE__,__FUNCTION__,__LINE__)	
	//抛出空指针错误
	#define NullException()		{ Char* pPtr = 0; *pPtr = 0; }
#ifdef _DEBUG
	//值非空断言
	#define Assert(cond)	{ if(!(cond)) CException::ShowFileLine sSFL("Assert", __FILE__,__FUNCTION__,__LINE__); assert(cond);}
#else
	//值非空断言
	#define Assert(cond)
#endif

}

#endif //_EXCEPTION_H_