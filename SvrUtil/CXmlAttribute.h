#ifndef C_XMLATTRIBUTE_H
#define C_XMLATTRIBUTE_H

#include "StringUtil.h"

namespace Util
{
	/************************************************************************/
	/* XML属性KV类型值定义                                                  */
	/************************************************************************/
	template<class StringType> class CXmlAttribute : public CRefCounter
	{
	public:
		//KV键值默认构造
		CXmlAttribute()
		{
			Key.clear();
			Value.clear();
		}

		//键值数据构造
		CXmlAttribute(const StringType& sKey,const StringType& sValue)
		{
			Key   = sKey;
			Value = sValue;
		}

		//赋值操作符
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
		//属性值转换为整数
		Int32 IntValue()
		{
			return CStringUtil::StringToInt<StringType>(Value);
		}

		//属性值转换为浮点数
		Float FloatValue()
		{
			return CStringUtil::StringToFloat<StringType>(Value);
		}

		//属性值转换为字符串
		StringType StringValue()
		{
			return Value;
		}

		//属性值转换为布尔值
		Bool BoolValue()
		{
			return CStringUtil::StringToBool<StringType>(Value);
		}

		//XML格式化字符串
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
		//属性键
		StringType Key;
		//属性值
		StringType Value;
	};

	//XML属性不同编码的定义
	typedef CXmlAttribute<AString>	AXmlAttribute;
	typedef CXmlAttribute<UString>	UXmlAttribute;
	typedef CXmlAttribute<WString>	WXmlAttribute;
}
#endif
