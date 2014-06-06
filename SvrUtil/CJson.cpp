#include "CJson.h"
#include "CScope.h"
#include "StringUtil.h"
#include "json/json.h"

namespace Util
{
	//解析Json值
	Bool  ParseJsonVal(const Json::Value* pJsonVal, CJson::JsonVal* pVal)
	{
		if (!pJsonVal || !pVal)
			return false;

		if (pJsonVal->isBool())
		{
			pVal->Type    = CJson::JSON_BOOL;
			pVal->BoolVal = pJsonVal->asBool();
		}
		else if (pJsonVal->isInt() || pJsonVal->isUInt())
		{
			pVal->Type   = CJson::JSON_INT;
			pVal->IntVal = pJsonVal->asInt();
		}
		else if (pJsonVal->isDouble())
		{
			pVal->Type     = CJson::JSON_FLOAT;
			pVal->FloatVal = (Float)pJsonVal->asDouble();
		}
		else if (pJsonVal->isString())
		{
			pVal->Type   = CJson::JSON_STRING;
			pVal->StrVal = (Utf8*)pJsonVal->asString().c_str();
		}
		else if (pJsonVal->isArray())
		{
			pVal->Type = CJson::JSON_ARRAY;
			for (Size_t i=0;i<pJsonVal->size();i++)
			{
				const Json::Value& sJsonVal = pJsonVal->get((UInt32)i, Json::Value::null);
				if (sJsonVal.isNull())
					return false;

				CJson::JsonVal* pSubVal = new CJson::JsonVal;
				if (ParseJsonVal(&sJsonVal, pSubVal))
				{
					pVal->ArrVal.push_back(pSubVal);
				}
				else
				{
					DEF_DELETE(pSubVal);
				}
			}
		}
		else if (pJsonVal->isObject())
		{
			pVal->Type = CJson::JSON_OBJECT;
			for(Json::Value::iterator it = pJsonVal->begin(); it !=  pJsonVal->end(); it++)
			{
				const Json::Value& sJsonKey = it.key();
				if (sJsonKey.isNull())
					return false;

				UString sKey = (Utf8*)sJsonKey.asString().c_str();
				const Json::Value& sJsonVal = pJsonVal->get((const Char*)sKey.c_str(),Json::Value::null);
				if (sJsonVal.isNull())
					return false;

				CJson::JsonVal* pSubVal = new CJson::JsonVal;
				if (ParseJsonVal(&sJsonVal, pSubVal))
				{
					pVal->ObjVal[sKey] = pSubVal;
				}
				else
				{
					DEF_DELETE(pSubVal);
					return false;
				}
			}
		}

		return true;
	}

	//转换为Json值
	Bool  ConvertToJsonVal(const CJson::JsonVal* pVal, Json::Value* pJsonVal)
	{
		if (!pJsonVal || !pVal)
			return false;

		if (!pJsonVal || !pVal)
			return false;

		if (pVal->Type == CJson::JSON_BOOL)
		{
			*pJsonVal = Json::Value(pVal->BoolVal);
		}
		else if (pVal->Type == CJson::JSON_INT)
		{
			*pJsonVal = Json::Value(pVal->IntVal);
		}
		else if (pVal->Type == CJson::JSON_FLOAT)
		{
			*pJsonVal = Json::Value(pVal->FloatVal);
		}
		else if (pVal->Type == CJson::JSON_STRING)
		{
			*pJsonVal = Json::Value((const Char*)pVal->StrVal.c_str());
		}
		else if (pVal->Type == CJson::JSON_ARRAY)
		{
			for (Size_t i=0;i<pVal->ArrVal.size();i++)
			{
				Json::Value jItem;
				if (ConvertToJsonVal(pVal->ArrVal[i], &jItem))
					pJsonVal->append(jItem);
			}
		}
		else if (pVal->Type == CJson::JSON_OBJECT)
		{
			for (CJson::JsonObj::const_iterator it = pVal->ObjVal.begin(); it != pVal->ObjVal.end(); ++it)
			{
				Json::Value jItem;
				if (ConvertToJsonVal(it->second, &jItem))
					(*pJsonVal)[(const Char*)it->first.c_str()] = jItem;
			}
		}

		return true;
	}

	//////////////////////////////////////////////////////////////////////////

	CJson::CJson()
	{
	}

	CJson::~CJson()
	{
		Clear();
	}

	Bool CJson::Parse(const AString& sFile)
	{
		CDiskFile sDiskFile;
		if (sDiskFile.Open(sFile, CFile::OPEN_READ))
		{
			Int32 iSize  = (Int32)sDiskFile.GetFileSize();
			UChar* pData = new UChar[iSize];
			memset(pData, 0, iSize);
			CScope::CDataArrayPtr scope(pData);
			iSize = (Int32)sDiskFile.Read(pData, iSize);
			return Parse(pData, iSize);
		}
		return false;
	}

	Bool CJson::Parse(const UChar* pData, Int32 iSize)
	{
		Json::Reader jReader;
		Json::Value  sRoot;
		if (jReader.parse((const Char*)pData, sRoot,false))
		{
			for(Json::Value::iterator it = sRoot.begin(); it != sRoot.end(); it++)
			{
				const Json::Value& sJsonKey = it.key();
				if (sJsonKey.isNull())
					return false;

				UString sKey = (Utf8*)sJsonKey.asString().c_str();
				const Json::Value& sJsonVal = sRoot[(const Char*)sKey.c_str()];
				if (sJsonVal.isNull())
					return false;

				JsonVal* pVal = new JsonVal;
				if (ParseJsonVal(&sJsonVal, pVal))
				{
					m_sRoot[sKey] = pVal;
				}
				else
				{
					DEF_DELETE(pVal);
					return false;
				}
			}
			return true;
		}
		return false;
	}

	Bool CJson::SaveToFile(const AString& sFile)
	{		
		std::ofstream sStream(sFile.c_str(), ios::out | ios::binary);
		if (sStream.is_open())
		{
			Json::StyledStreamWriter jWriter;
			Json::Value jVal;

			for (JsonObj::iterator it = m_sRoot.begin(); it != m_sRoot.end(); it++)
			{
				Json::Value jItem;
				if (ConvertToJsonVal(it->second, &jItem))
					jVal[(const Char*)it->first.c_str()] = jItem;
			}

			jWriter.write(sStream, jVal);
			sStream.close();
			return true;
		}
		return false;
	}

	Bool CJson::Clear()
	{
		JsonObj::iterator it = m_sRoot.begin();
		for (;it!=m_sRoot.end();it++)
		{
			FreeJsonVal(it->second);
		}
		m_sRoot.clear();
		return true;
	}

	Bool CJson::FreeJsonVal(JsonVal* pVal)
	{
		if (pVal->Type == JSON_OBJECT)
		{
			JsonObj::iterator it = m_sRoot.begin();
			for (;it!=m_sRoot.end();it++)
			{
				FreeJsonVal(it->second);
			}
			pVal->ObjVal.clear();
		}
		else if (pVal->Type == JSON_ARRAY && pVal->ArrVal.size())
		{
			for (Size_t i=0;i<pVal->ArrVal.size();i++)
			{
				FreeJsonVal(pVal->ArrVal[i]);
			}
			pVal->ArrVal.clear();
		}		
		DEF_DELETE(pVal);
		return true;
	}

	const CJson::JsonObj* CJson::GetRoot() const
	{
		return &m_sRoot;
	}
}
