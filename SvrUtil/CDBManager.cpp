#include "CDBManager.h"
#include "CMysql.h"

namespace Util
{
	//数据库管理对象单例定义
	SINGLETON_IMPL(DBManager);	

	//////////////////////////////////////////////////////////////////////////
	//安全对象访问类实现
	CDBManager::SafeDB::SafeDB(const AString& sDBName) : DBManager::SafeObj(g_DBManager->m_pDBMan,sDBName)
	{
	}
	
	CDBManager::SafeDB::~SafeDB()
	{
	}


	//////////////////////////////////////////////////////////////////////////
	//数据库管理器
	CDBManager::CDBManager()
	{
		m_pDBMan = new DBManager;
	}

	CDBManager::~CDBManager()
	{		
		DEF_RELEASE(m_pDBMan);
	}

	CDatabase*  CDBManager::OpenDatabase(const AString& sDBName, const CDatabase::DBConn& sDBConn)
	{
		if (!m_pDBMan->QueryObject(sDBName))
		{
			CDatabase* pDB = InnerCreate(sDBConn.GetType());
			if (pDB)
			{
				if(pDB->Open(sDBConn))
				{
					if (m_pDBMan->AllocObject(sDBName,pDB))
					{
						pDB->SetDBName(sDBName);
						return pDB;
					}	
				}
				DEF_RELEASE(pDB);
			}
		}		
		return 0;
	}

	Bool CDBManager::CloseDatabase(const AString& sDBName)
	{
		if (m_pDBMan)
		{
			return m_pDBMan->FreeObject(sDBName);
		}
		return false;
	}

	CDatabase* CDBManager::InnerCreate(UInt32 iType)
	{
		CDatabase* pDB = 0;

#ifdef UTIL_MYSQL
		if (iType == CDatabase::HDB_MYSQL)
			pDB = new CMysql;
#endif

		return pDB;
	}
}
