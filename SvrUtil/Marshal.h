#ifndef _MARSHAL_H_
#define _MARSHAL_H_

#include "OctetsStream.h"

namespace Util
{
	/************************************************************************/
	/* �����������л��ͷ����л������װ										*/
	/************************************************************************/
	class UTIL_API CMarshal : public CRefCounter
	{
	public:
		//���ݴ��(����������)
		virtual COctetsStream& Marshal(COctetsStream& rhsOS) ;

		//���ݽ��(����������)
		virtual COctetsStream& Unmarshal(COctetsStream& rhsOS) ;

		//��������
		virtual Bool Clear();
	};
}
#endif //_MARSHAL_H_
