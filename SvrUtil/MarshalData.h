#ifndef _MARSHALDATA_H_
#define _MARSHALDATA_H_

#include "Marshal.h"

namespace Util
{
	/************************************************************************/
	/* �����л����ݶ���,��Ҫ����Protocol�е����ݽṹ							*/
	/************************************************************************/
	class UTIL_API CMarshalData : public CMarshal
	{
	public:
		//�����л������¡����
		virtual CMarshalData* Create() const;

		//�����л�����ֵ����
		virtual CMarshalData& operator = (const CMarshalData& rhsMD);
	};

	//�����Ͷ���
	typedef CMarshalData MarshalData;
}
#endif //_MARSHALDATA_H_
