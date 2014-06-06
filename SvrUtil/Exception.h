#ifndef _EXCEPTION_H_
#define _EXCEPTION_H_

#include "BaseHeader.h"
#include "UtilMalloc.h"

namespace Util
{
	/************************************************************************/
	/* �쳣������,����ϵͳ�쳣�Ĵ���ͼ�¼									*/
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
		//�����쳣��Ϣ
		CException(const AString& sMsg,const AString& sFile, const AString& sFunc, Int32 iLine);

		//��������
		virtual ~CException() throw();

		//�쳣����
		Bool CException::Throw() const;
	};

	//�׳��쳣
	#define T_Exception(msg)	throw CException(msg,__FILE__,__FUNCTION__,__LINE__)
	//�ж�valΪfalseʱ�׳��쳣
	#define A_Exception(cond)	if(!(cond)) throw CException(#cond,__FILE__,__FUNCTION__,__LINE__)	
	//�׳���ָ�����
	#define NullException()		{ Char* pPtr = 0; *pPtr = 0; }
#ifdef _DEBUG
	//ֵ�ǿն���
	#define Assert(cond)	{ if(!(cond)) CException::ShowFileLine sSFL("Assert", __FILE__,__FUNCTION__,__LINE__); assert(cond);}
#else
	//ֵ�ǿն���
	#define Assert(cond)
#endif

}

#endif //_EXCEPTION_H_