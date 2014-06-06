#ifndef C_JSON_H
#define C_JSON_H

#include "CDiskFile.h"

namespace Util
{
	/************************************************************************/
	/* JSON��ʽ���ݽ���(UTF8����)											*/
	/************************************************************************/
	class UTIL_API CJson : public CRefCounter
	{
	public:
		//����
		CJson();

		//����
		virtual ~CJson();

	public:
		//ֵ���Ͷ���
		enum
		{
			JSON_NONE = 0,
			JSON_BOOL,
			JSON_INT,
			JSON_FLOAT,
			JSON_STRING,
			JSON_OBJECT,
			JSON_ARRAY,				
		};

		//�������ͺͶ������Ͷ���
		struct  JsonVal;
		typedef vector<JsonVal*>		JsonArr;
		typedef map<UString, JsonVal*>	JsonObj;

		//ֵ
		struct JsonVal
		{
			UInt8	 Type;
			Bool	 BoolVal;
			Int32	 IntVal;
			Float	 FloatVal;
			UString	 StrVal;
			JsonObj  ObjVal;
			JsonArr	 ArrVal;

			JsonVal() : Type(JSON_NONE), BoolVal(false), IntVal(0), FloatVal(0) {};
		};

	public:
		//��������
		virtual Bool   Parse(const AString& sFile);

		//��������
		virtual Bool   Parse(const UChar* pData, Int32 iSize);

		//���浽�ļ�
		virtual Bool   SaveToFile(const AString& sFile);

		//�������
		virtual Bool   Clear();

		//��ȡ������
		const JsonObj* GetRoot() const;

	protected:
		//�ͷ�JsonVal����
		virtual Bool   FreeJsonVal(JsonVal* pVal);

	protected:
		//������
		JsonObj  m_sRoot;
	};
}
#endif
