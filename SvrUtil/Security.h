#ifndef _SECURITY_H_
#define _SECURITY_H_

#include "Octets.h"

namespace Util
{
	/************************************************************************/
	/* ���ݼӰ���ܽӿڻ��ඨ��												*/
	/************************************************************************/
	class UTIL_API CSecurity : public CRefCounter
	{
	protected:
		//���ܶ�����
		CSecurity();

		//���ܶ�������
		virtual ~CSecurity() ;

	public:
		//���ü�����Կ
		virtual void  SetParameter(const COctets& xOctets);

		//��ȡ������Կ
		virtual void  GetParameter(COctets& xOctets);	

		//���ܸ����ֽ���
		virtual Bool  Update(COctets& xOctets);
	};
}
#endif //_SECURITY_H_
