#ifndef _DBMANAGER_H_
#define _DBMANAGER_H_

#include "ManagerBase.h"
#include "CObjManager.h"
#include "CDatabase.h"

namespace Util
{
	/************************************************************************/
	/* 数据库管理类                                                         */
	/************************************************************************/
	class UTIL_API CDBManager : public CManagerBase
	{
	protected:
		//构造
		CDBManager();

		//析构
		virtual ~CDBManager();

		//管理单例申明
		SINGLETON_DECL(DBManager);				

	public:
		//数据库管理容器类型
		typedef CObjManager<AString, CDatabase> DBManager;	

		//安全DB操作封装
		class UTIL_API SafeDB : public DBManager::SafeObj
		{
		public:
			SafeDB(const AString& sDBName = "");

			virtual ~SafeDB();
		};

		//申明友员访问
		friend class SafeDB;

	public:
		//打开数据库
		virtual CDatabase* OpenDatabase(const AString& sDBName,const CDatabase::DBConn& sDBConn);				

		//关闭数据库
		virtual Bool	   CloseDatabase(const AString& sDBName);

	protected:
		//创建数据库对象
		virtual CDatabase* InnerCreate(UInt32 iType);

	protected:
		//数据库管理器
		DBManager* m_pDBMan;
	};

	//数据库全局管理器对象
	#define g_DBManager CDBManager::GetInstance()
}
#endif //_DBMANAGER_H_
