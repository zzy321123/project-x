#include "MarshalData.h"

namespace Util
{
	CMarshalData* CMarshalData::Create() const
	{
		T_Exception("CMarshalData::Create Must Implement.");
		return 0;
	}

	CMarshalData& CMarshalData::operator = (const CMarshalData& rhs)
	{		
		T_Exception("CMarshalData::operator =  Must Implement.");
		return *this;
	}	
}
