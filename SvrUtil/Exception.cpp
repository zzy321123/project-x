#include "Exception.h"
#include "LoggerManager.h"

namespace Util
{
	CException::ShowFileLine::ShowFileLine(const AString& sTag, const AString& sFile, const AString& sFunc, Int32 iLine)
	{
		//计算除路径外的文件名
		Size_t iPos = sFile.find_last_of('\\');
		if (iPos == AString::npos)
			iPos = sFile.find_last_of('/');

		AString sFileName = "";
		if (iPos != AString::npos)
			sFileName = sFile.substr(iPos+1,sFile.size()-iPos-1);

		ostringstream oas;
		oas << sTag << " : File(" << sFileName << ")---Function(" << sFunc << ")---Line(" << iLine << ")\r\n";

		Print(oas.str());
	}

	//////////////////////////////////////////////////////////////////////////

	CException::CException(const AString& sMsg,const AString& sFile, const AString& sFunc,Int32 iLine)
#ifdef PLATFORM_WINDOWS
		: std::exception(sMsg.c_str())
#else
		: std::exception()
#endif
	{
		//计算除路径外的文件名
		Size_t iPos = sFile.find_last_of('\\');
		if (iPos == AString::npos)
			iPos = sFile.find_last_of('/');

		AString sFileName = "";
		if (iPos != AString::npos)
			sFileName = sFile.substr(iPos+1,sFile.size()-iPos-1);

		ostringstream oas;
		oas << "Exception(" << sMsg << ") : ";
		oas << "File(" << sFileName << ")---Function(" << sFunc << ")---Line(" << iLine << ")\r\n";

		Print(oas.str());
	}

	CException::~CException() throw()
	{
	}

	Bool CException::Throw() const
	{
		return true;
	}
}
