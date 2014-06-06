#ifndef C_XMLATTRIBUTE_H
#define C_XMLATTRIBUTE_H

#include "StringUtil.h"

namespace Util
{
	/************************************************************************/
	/* XML����KV����ֵ����                                                  */
	/************************************************************************/
	template<class StringType> class CXmlAttribute : public CRefCounter
	{
	public:
		//KV��ֵĬ�Ϲ���
		CXmlAttribute()
		{
			Key.clear();
			Value.clear();
		}

		//��ֵ���ݹ���
		CXmlAttribute(const StringType& sKey,const StringType& sValue)
		{
			Key   = sKey;
			Value = sValue;
		}

		//��ֵ������
		CXmlAttribute<StringType>& operator = (const CXmlAttribute<StringType>& sRhs)
		{
			if (this != &sRhs)
			{
				Key   = sRhs.Key;
				Value = sRhs.Value;
			}

			return *this;
		}

	public:
		//����ֵת��Ϊ����
		Int32 IntValue()
		{
			return CStringUtil::StringToInt<StringType>(Value);
		}

		//����ֵת��Ϊ������
		Float FloatValue()
		{
			return CStringUtil::StringToFloat<StringType>(Value);
		}

		//����ֵת��Ϊ�ַ���
		StringType StringValue()
		{
			return Value;
		}

		//����ֵת��Ϊ����ֵ
		Bool BoolValue()
		{
			return CStringUtil::StringToBool<StringType>(Value);
		}

		//XML��ʽ���ַ���
		StringType ToString()
		{
			StringType sTmp;
			sTmp = Key;
			CStringUtil::Append<StringType,AString>(sTmp,"=\"");
			sTmp = sTmp + Value;
			CStringUtil::Append<StringType,AString>(sTmp,"\"");
			return sTmp;
		}

	public:
		//���Լ�
		StringType Key;
		//����ֵ
		StringType Value;
	};

	//XML���Բ�ͬ����Ķ���
	typedef CXmlAttribute<AString>	AXmlAttribute;
	typedef CXmlAttribute<UString>	UXmlAttribute;
	typedef CXmlAttribute<WString>	WXmlAttribute;
}
#endif
