#include "BaseHeader.h"

#ifdef UTIL_MYSQL
#include "mysql.h"
#endif

#include "CMysql.h"
#include "CDBManager.h"
#include "CScope.h"
#include "StringUtil.h"

namespace Util
{
#ifdef UTIL_MYSQL
	//////////////////////////////////////////////////////////////////////////
	//mysql连接在 wait-timeout (s) 内没有任何操作执行，服务器主动断开连接   //
	//可以在my.ini里面的mysqld 下设置wai-timeout值来控制					//
	//////////////////////////////////////////////////////////////////////////
	/*my.ini配置
	[mysql]
	default-character-set=utf8

	[mysqld]
	character-set-server=utf8
	default-storage-engine=MYISAM
	wait_timeout=604800
	interactive_timeout=604800
	max_connections=1024
	*/

	CMysql::CMysql() : CDatabase()
	{
		m_iDBType = CDatabase::HDB_MYSQL;
	}

	CMysql::~CMysql()
	{
		Close();
	}

	Bool CMysql::Open(const CDatabase::DBConn& sConn)
	{
		ClearError();

		MYSQL* pSql = mysql_init((MYSQL*)m_pDBHandle);
		if(!pSql) 
			return false;
		
		m_pDBHandle = pSql;

		Int32 iTimeout = 60;
		Int32 iRes	   = 0;
		
		iRes = mysql_options(pSql, MYSQL_OPT_CONNECT_TIMEOUT, (Char *)&iTimeout);
		if(iRes) 
		{
			GenError();
			return false;
		}

		UInt32 iCltFlag = CLIENT_FOUND_ROWS;

		AString sHost = "localhost";
		if (sConn.GetHost().size())
			sHost = sConn.GetHost();

		Int32 iPort = 3306;
		if (sConn.GetPort())
			iPort = sConn.GetPort();

		AString sUser = "root";
		if (sConn.GetUser().size())
			sUser = sConn.GetUser();

		AString sPwd = "";
		if (sConn.GetPwd().size())
			sPwd = sConn.GetPwd();

		pSql = mysql_real_connect(pSql,sHost.c_str(),sUser.c_str(),sPwd.c_str(),(const Char*)sConn.GetDBName().c_str(),iPort,0,iCltFlag);
		if(!pSql) 
		{
			GenError();
			return false;
		}

		iRes = mysql_set_server_option(pSql, MYSQL_OPTION_MULTI_STATEMENTS_ON);
		if(iRes) 
		{
			GenError();
			return false;
		}

		if (sConn.GetDBName().size() && !SelectDB(sConn.GetDBName()))
		{
			GenError();
			return false;
		}

		return true;
	}

	Bool CMysql::Close()
	{
		if (m_pDBHandle)
		{
			mysql_close((MYSQL *)m_pDBHandle);
			m_pDBHandle = 0;
			return true;
		}
		return false;
	}

	void CMysql::GenError()
	{
		if (m_pDBHandle)
		{
			m_iErrCode = mysql_errno((MYSQL *)m_pDBHandle);
			m_sErrMsg  = (Utf8*)mysql_error((MYSQL *)m_pDBHandle);
		}			
	}

	UString CMysql::AmendString(const UString& sValue)
	{
		Int32 iSize = sValue.size()*2 + 1;
		Utf8* pBuf  = (Utf8*)UtilMalloc(iSize*sizeof(Utf8));
		memset(pBuf,0,iSize * sizeof(Utf8));
		CScope::CMallocPtr scope(pBuf);

		mysql_real_escape_string((MYSQL *)m_pDBHandle,(Char*)pBuf,(const Char*)sValue.c_str(),sValue.size());
		return pBuf;
	}

	AString CMysql::AmendString(const AString& sValue)
	{
		Int32 iSize = sValue.size()*2 + 1;
		Char* pBuf  = (Char*)UtilMalloc(iSize * sizeof(Char));
		memset(pBuf,0,iSize * sizeof(Char));
		CScope::CMallocPtr scope(pBuf);

		mysql_real_escape_string((MYSQL *)m_pDBHandle,pBuf,sValue.c_str(),sValue.size());
		return pBuf;
	}

	Bool CMysql::ChangeUser(const AString& sUser, const AString& sPwd, const AString& sDBName)
	{
		if(!m_pDBHandle)
			return false;

		ClearError();

		if(mysql_change_user((MYSQL *)m_pDBHandle,sUser.c_str(),sPwd.c_str(),sDBName.c_str()))
			return true;

		GenError();
		return false;
	}

	Bool CMysql::IsExistDB(const AString& sDBName)
	{
		if(!m_pDBHandle)
			return false;

		ClearError();

		Char sCmd[SQL_DEFAULT_SIZE] = {0};
		sprintf(sCmd,"SELECT * FROM information_schema.SCHEMATA WHERE SCHEMA_NAME = '%s';",sDBName.c_str());

		Int32 iRes = mysql_query((MYSQL *)m_pDBHandle,(const Char*)sCmd);
		if(iRes) 
		{
			GenError();
			return false;
		}

		MYSQL_RES* pRes = mysql_store_result((MYSQL *)m_pDBHandle);
		if (pRes)
		{
			UInt64 iRow = mysql_num_rows(pRes);
			UInt64 iCol = mysql_num_fields(pRes);
			mysql_free_result(pRes);
			return iRow > 0 && iCol > 0;
		}

		return false;
	}

	Bool CMysql::CreateDB(const AString& sDBName)
	{
		if(!m_pDBHandle)
			return false;

		ClearError();

		Char sCmd[SQL_DEFAULT_SIZE] = {0};
		sprintf(sCmd,"CREATE DATABASE '%s';",sDBName.c_str());

		Int32 iRes = mysql_query((MYSQL *)m_pDBHandle,(const Char*)sCmd);
		if(iRes) 
		{
			GenError();
			return false;
		}

		ClearResult();

		return true;
	}

	Bool CMysql::SelectDB(const AString& sDBName)
	{
		if(!m_pDBHandle)
			return false;

		ClearError();

		Int32 iRes = mysql_select_db((MYSQL *)m_pDBHandle, sDBName.c_str());
		if(iRes)
		{
			GenError();
			return false;
		}

		ClearResult();

		return true;
	}

	Bool CMysql::IsExistTable(const AString& sDBName,const AString& sTblName)
	{
		if(!m_pDBHandle)
			return false;

		ClearError();

		if (sDBName.size() && sTblName.size())
		{
			Char sCmd[SQL_DEFAULT_SIZE] = {0};
			sprintf(sCmd,"SELECT * FROM information_schema.TABLES WHERE TABLE_SCHEMA = '%s' and TABLE_NAME = '%s';",sDBName.c_str(),sTblName.c_str());

			Int32 iRes = mysql_query((MYSQL *)m_pDBHandle,(const Char*)sCmd);
			if(iRes) 
			{
				GenError();
				return false;
			}

			MYSQL_RES* pRes = mysql_store_result((MYSQL *)m_pDBHandle);
			if (pRes)
			{
				UInt64 iRow = mysql_num_rows(pRes);
				UInt64 iCol = mysql_num_fields(pRes);
				mysql_free_result(pRes);
				return iRow > 0 && iCol > 0;
			}			
		}

		return false;
	}

	Int32 CMysql::GetInsertId()
	{
		return (Int32)mysql_insert_id((MYSQL*)m_pDBHandle);
	}

	void CMysql::ClearResult()
	{
		//取出结果并清空
		MYSQL_RES* pRes = mysql_store_result((MYSQL *)m_pDBHandle);
		if(pRes) 
			mysql_free_result(pRes);

		while (!mysql_next_result((MYSQL *)m_pDBHandle))
		{
			pRes = mysql_store_result((MYSQL *)m_pDBHandle);
			if(pRes) 
				mysql_free_result(pRes);
		}
	}

	Int32 CMysql::MysqlQuery(const UString& sSql)
	{
		if (m_pDBHandle == 0)
			return UTIL_ERROR;

		return mysql_query((MYSQL *)m_pDBHandle,(const Char*)sSql.c_str());
	}

	Bool CMysql::Execute(const UString& sSql)
	{
		if (m_pDBHandle && sSql.size())
		{
			ClearError();

			Int32 iRes = MysqlQuery(sSql);
			if(iRes) 
			{
				GenError();
				return false;
			}
			
			ClearResult();

			return true;
		}		

		return false;
	}

	Bool CMysql::Select(const UString& sSql, QueryResult& sResult)
	{
		if (m_pDBHandle && sSql.size())
		{			
			ClearError();

			sResult.SetDBHdl(this);

			Int32 iRes = MysqlQuery(sSql);
			if(iRes) 
			{
				GenError();
				return false;
			}

			MYSQL_RES* pRes = mysql_store_result((MYSQL *)m_pDBHandle);
			if (pRes)
			{
				sResult.SetResHdl((void*)pRes);
				sResult.SetRow(mysql_num_rows(pRes));
				sResult.SetCol(mysql_num_fields(pRes));
			}

			ClearResult();
			return true;
		}
		return false;
	}

	Bool CMysql::Insert(const UString& sSql, QueryResult& sResult)
	{
		if (m_pDBHandle && sSql.size())
		{			
			ClearError();

			sResult.SetDBHdl(this);

			Int32 iRes = MysqlQuery(sSql);
			if(iRes) 
			{
				GenError();
				return false;
			}			

			sResult.SetAffectRows(mysql_affected_rows((MYSQL *)m_pDBHandle));

			MYSQL_RES* pRes = mysql_store_result((MYSQL *)m_pDBHandle);
			if (pRes)
			{
				sResult.SetResHdl((void*)pRes);
				sResult.SetRow(mysql_num_rows(pRes));
				sResult.SetCol(mysql_num_fields(pRes));		
			}

			ClearResult();
			return true;
		}
		return false;
	}

	Bool CMysql::Update(const UString& sSql, QueryResult& sResult)
	{
		if (m_pDBHandle && sSql.size())
		{			
			ClearError();

			sResult.SetDBHdl(this);

			Int32 iRes = MysqlQuery(sSql);
			if(iRes) 
			{
				GenError();
				return false;
			}

			sResult.SetAffectRows(mysql_affected_rows((MYSQL *)m_pDBHandle));

			MYSQL_RES* pRes = mysql_store_result((MYSQL *)m_pDBHandle);
			if (pRes)
			{
				sResult.SetResHdl((void*)pRes);
				sResult.SetRow(mysql_num_rows(pRes));
				sResult.SetCol(mysql_num_fields(pRes));			
			}

			ClearResult();
			return true;
		}
		return false;
	}

	Bool CMysql::Delete(const UString& sSql, QueryResult& sResult)
	{
		if (m_pDBHandle && sSql.size())
		{			
			ClearError();

			sResult.SetDBHdl(this);

			Int32 iRes = MysqlQuery(sSql);
			if(iRes) 
			{
				GenError();
				return false;
			}

			sResult.SetAffectRows(mysql_affected_rows((MYSQL *)m_pDBHandle));

			MYSQL_RES* pRes = mysql_store_result((MYSQL *)m_pDBHandle);
			if (pRes)
			{
				sResult.SetResHdl((void*)pRes);
				sResult.SetRow(mysql_num_rows(pRes));
				sResult.SetCol(mysql_num_fields(pRes));
			}

			ClearResult();

			return true;
		}
		return false;
	}

	Bool CMysql::FreeResult(QueryResult* pResult)
	{
		if (m_pDBHandle && pResult)
		{
			MYSQL_RES* pRes = (MYSQL_RES*)pResult->GetResHdl();
			if (pRes)
				mysql_free_result(pRes);
			return true;
		}
		return false;
	}

	Utf8* CMysql::GetResult(const QueryResult* pResult, UInt64 iRow, UInt64 iCol, UInt32* pResLen)
	{		
		if (!pResult || !pResult->GetDBHdl())
			return 0;

		MYSQL_RES* pRes = (MYSQL_RES*)pResult->GetResHdl();
		if (!pRes)
			return 0;

		UInt64 iTotalRow = pResult->GetRow();
		UInt64 iTotalCol = pResult->GetCol();
		if (iTotalRow <= 0 || iTotalCol <= 0 )
			return 0;

		A_Exception(iRow >= 0 && iRow < iTotalRow);
		A_Exception(iCol >= 0 && iCol < iTotalCol);
		
		mysql_data_seek(pRes,iRow);

		MYSQL_ROW ppRow = mysql_fetch_row(pRes);
		UInt32* ppLen   = (UInt32*)mysql_fetch_lengths(pRes);
		if (!ppRow || !ppLen)
			return 0;

		if (pResLen)
			*pResLen = (Int32)ppLen[iCol];

		return (Utf8*)ppRow[iCol];
	}
#endif
}
