#ifndef C_RAND_H
#define C_RAND_H

#include "Exception.h"

namespace Util
{
	/************************************************************************/
	/* �����������															*/
	/************************************************************************/
	class UTIL_API CRand
	{
	public:
		//�����������
		static Int32 RandInt();

		//���ɴ����޵��������
		static Int32 RandInt(Int32 iMax);

		//���ɴ������޵��������
		static Int32 RandInt(Int32 iLow, Int32 iHigh);

		//�������������
		static Float RandFloat();

		//���ɴ����޵����������
		static Float RandFloat(Float fMax);

		//���ɴ������޵����������
		static Float RandFloat(Float fLow, Float fHigh);
	};
}
#endif //C_RAND_H

