#include "Marshal.h"

namespace Util
{
	COctetsStream& CMarshal::Marshal(COctetsStream& rhsOS)
	{
		T_Exception("CMarshal::Marshal Must Implement.");
		return rhsOS;
	}

	COctetsStream& CMarshal::Unmarshal(COctetsStream& rhsOS)
	{
		T_Exception("CMarshal::Unmarshal Must Implement.");
		return rhsOS;
	}

	Bool CMarshal::Clear()
	{
		return true;
	}
}
