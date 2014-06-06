#ifndef C_XMLDOCUMENT_H
#define C_XMLDOCUMENT_H

#include "CXmlElement.h"

namespace Util
{
	/************************************************************************/
	/* XML文档对象类                                                        */
	/************************************************************************/
	template<class StringType> class CXmlDocument : public CRefCounter
	{
	public:
		//XML文档默认构造
		CXmlDocument()
		{
		}

		//XML文档析构
		virtual ~CXmlDocument()
		{
		}

	protected:
		//XML文档跟节点
		CXmlElement<StringType> Root;

	public:		
		//获取文档根节点
		CXmlElement<StringType>* GetRoot()
		{
			return &Root;
		}

		//XML格式化字符串
		StringType ToString() const
		{
			StringType sRet = Root.ToString();
			CStringUtil::Append<StringType,AString>(sRet,"\r\n");
			return sRet;
		}

		//存储到文件
		Bool Save(const AString& sFile, CFile::CodeType eCode)
		{
			CDiskFile xFile;
			if (!xFile.Open(sFile,CFile::OPEN_WRITE))
				return false;

			//重建树形父子结构
			RebuildRelation();
			//根据编码不同存储
			if (eCode == CFile::CODE_ASCII)
			{				
				AString sTmp = (const Char*)ToString().c_str();
				xFile.Write(sTmp.c_str(),sTmp.size()*sizeof(Char));
			}
			else if (eCode == CFile::CODE_UTF8)
			{
				static UChar UTF8_TAG[] = {0xEF,0xBB,0xBF};
				xFile.Write(UTF8_TAG,3);
				UString sTmp = (const Utf8*)ToString().c_str();
				xFile.Write(sTmp.c_str(),sTmp.size()*sizeof(Utf8));
			}
			else
			{
				static UChar UNICODE_TAG[] = {0xFF,0xFE};
				xFile.Write(UNICODE_TAG,2);
				WString sTmp = (const WChar*)ToString().c_str();
				xFile.Write(sTmp.c_str(),sTmp.size()*sizeof(WChar));
			}
			xFile.Close();
			return true;
		}

		//XML文档节点清理
		void Clear()
		{
			Root.Clear();
		}

		//重建节点父子关系
		void RebuildRelation()
		{
			Root.Parent = 0;
			Root.Level  = 0;

			vector< CXmlElement<StringType>* > ptr_list;
			ptr_list.push_back(&Root);

			UInt32 iIdx = 0;
			//循环方式规避递归调用
			while (iIdx < ptr_list.size())
			{
				CXmlElement<StringType>* pEle = ptr_list[iIdx++];
				for (Size_t i = 0;i<pEle->Children.size();i++)
				{
					pEle->Children[i].Parent = pEle;
					pEle->Children[i].Level  = pEle->Level+1;
					ptr_list.push_back(&pEle->Children[i]);
				}
			}
		}		
	};

	//XML文档不同编码的定义
	typedef CXmlDocument<AString>	AXmlDocument;
	typedef CXmlDocument<UString>	UXmlDocument;
	typedef CXmlDocument<WString>	WXmlDocument;
}
#endif
