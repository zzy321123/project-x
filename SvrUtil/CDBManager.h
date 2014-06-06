#ifndef _DBMANAGER_H_
#define _DBMANAGER_H_

#include "ManagerBase.h"
#include "CObjManager.h"
#include "CDatabase.h"

namespace Util
{
	/************************************************************************/
	/* ���ݿ������                                                         */
	/************************************************************************/
	class UTIL_API CDBManager : public CManagerBase
	{
	protected:
		//����
		CDBManager();

		//����
		virtual ~CDBManager();

		//����������
		SINGLETON_DECL(DBManager);				

	public:
		//���ݿ������������
		typedef CObjManager<AString, CDatabase> DBManager;	

		//��ȫDB������װ
		class UTIL_API SafeDB : public DBManager::SafeObj
		{
		public:
			SafeDB(const AString& sDBName = "");

			virtual ~SafeDB();
		};

		//������Ա����
		friend class SafeDB;

	public:
		//�����ݿ�
		virtual CDatabase* OpenDatabase(const AString& sDBName,const CDatabase::DBConn& sDBConn);				

		//�ر����ݿ�
		virtual Bool	   CloseDatabase(const AString& sDBName);

	protected:
		//�������ݿ����
		virtual CDatabase* InnerCreate(UInt32 iType);

	protected:
		//���ݿ������
		DBManager* m_pDBMan;
	};

	//���ݿ�ȫ�ֹ���������
	#define g_DBManager CDBManager::GetInstance()
}
#endif //_DBMANAGER_H_
