#include "CDatabase.h"

namespace Util
{
	//////////////////////////////////////////////////////////////////////////
	//��ѯ�����
	CDatabase::QueryResult::QueryResult()
	{
		m_iRow	  = 0;
		m_iCol    = 0;
		m_iAffect = 0;
		m_pResHdl = 0;
		m_pDBHdl  = 0;
	}

	CDatabase::QueryResult::~QueryResult()
	{
		if (m_pDBHdl && m_pResHdl)
		{
			//��������
			((CDatabase*)m_pDBHdl)->FreeResult(this);

			//��������
			m_iRow	  = 0;
			m_iCol	  = 0;
			m_iAffect = 0;
		}
	}	

	const Utf8* CDatabase::QueryResult::GetResult(UInt64 iRow,UInt64 iCol,UInt32* pResLen) const
	{
		if (m_pDBHdl && m_pResHdl && m_iRow && m_iCol)
		{
			A_Exception(iRow >= 0 && iRow < m_iRow);
			A_Exception(iCol >= 0 && iCol < m_iCol);

			return ((CDatabase*)m_pDBHdl)->GetResult(this,iRow,iCol,pResLen);
		}
		return 0;
	}

	const Utf8* CDatabase::QueryResult::operator[] (UInt64 iIdx) const
	{		
		if (m_pDBHdl && m_pResHdl && m_iRow && m_iCol)
		{
			UInt64 iRow = iIdx / m_iCol;
			UInt64 iCol = iIdx % m_iCol;
			return GetResult(iRow, iCol);
		}
		return 0;
	}

	const Utf8* CDatabase::QueryResult::operator()(UInt64 iRow, UInt64 iCol) const
	{
		return GetResult(iRow,iCol);
	}

	//////////////////////////////////////////////////////////////////////////
	//���ݿ�����
	CDatabase::DBConn::DBConn(UInt32 iType)
	{
		Reset();

		m_iType = iType;
	}

	//����
	CDatabase::DBConn::~DBConn()
	{
		Reset();
	}

	//��ֵ������
	CDatabase::DBConn& CDatabase::DBConn::operator = (const CDatabase::DBConn& sConn)
	{
		if (this != &sConn)
		{
			m_iType   = sConn.m_iType;
			m_iPort   = sConn.m_iPort;
			m_sHost   = sConn.m_sHost;				
			m_sUser   = sConn.m_sUser;
			m_sPwd	  = sConn.m_sPwd;
			m_sDBName = sConn.m_sDBName;
		}
		return *this;
	}

	Bool CDatabase::DBConn::IsValid() const
	{
		if ((m_iType == HDB_MYSQL && m_sHost.size() && m_iPort > 0 && m_sUser.size()))
			return true;

		return false;
	}

	void CDatabase::DBConn::Reset()
	{
		m_iType = 0;
		m_iPort = 0;
		m_sHost.clear();
		m_sUser.clear();
		m_sPwd.clear();
		m_sDBName.clear();
	}

	//////////////////////////////////////////////////////////////////////////

	CDatabase::CDatabase()
	{
		m_iDBType   = 0;
		m_pDBHandle = 0;		
		m_iErrCode  = 0;		
		m_bTrans    = false;
		m_sDBName.clear();
		m_sErrMsg.clear();
	}

	CDatabase::~CDatabase()
	{
		Close();
	}

	Bool CDatabase::Open(const DBConn& sConn)
	{
		m_sDBConn = sConn;
		return true;
	}

	Bool CDatabase::Close()
	{
		if (m_pDBHandle)
		{
			m_pDBHandle = 0;
			return true;
		}

		return false;
	}	

	Bool CDatabase::Execute(const UString& sSql)
	{
		if (m_pDBHandle && sSql.size())
			return true;
		
		return false;
	}

	void CDatabase::GenError()
	{
	}

	void CDatabase::ClearError()
	{
		m_sErrMsg.clear();
		m_iErrCode = 0;
	}	

	Bool CDatabase::Select(const UString& sSql, QueryResult& sResult)
	{
		if (m_pDBHandle && sSql.size())
		{
			sResult.m_pDBHdl = this;
			return true;
		}

		return false;
	}

	Bool CDatabase::Insert(const UString& sSql, QueryResult& sResult)
	{
		if (m_pDBHandle && sSql.size())
		{
			sResult.m_pDBHdl = this;
			return true;
		}

		return false;
	}

	Bool CDatabase::Update(const UString& sSql, QueryResult& sResult)
	{
		if (m_pDBHandle && sSql.size())
		{
			sResult.m_pDBHdl = this;
			return true;
		}

		return false;
	}

	Bool CDatabase::Delete(const UString& sSql, QueryResult& sResult)
	{
		if (m_pDBHandle && sSql.size())
		{
			sResult.m_pDBHdl = this;
			return true;
		}

		return false;
	}	

	Bool CDatabase::BeginTrans()
	{
		if (m_pDBHandle && !m_bTrans)
		{
			m_bTrans = true;
			return true;
		}

		return false;		
	}

	Bool CDatabase::RollbackTrans()
	{
		if (m_pDBHandle && m_bTrans)
		{
			m_bTrans = false;
			return true;
		}

		return false;
	}

	Bool CDatabase::CommitTrans()
	{
		if (m_pDBHandle && m_bTrans)
		{
			m_bTrans = false;
			return true;
		}
		
		return false;
	}

	UString CDatabase::AmendString(const UString& sValue) const
	{
		T_Exception("CDatabase::AmendString Must Implement.");
		return sValue;
	}

	AString CDatabase::AmendString(const AString& sValue) const
	{
		T_Exception("CDatabase::AmendString Must Implement.");
		return sValue;
	}

	Utf8* CDatabase::GetResult(const QueryResult* pResult,UInt64 iRow,UInt64 iCol,UInt32* pResLen)
	{
		return 0;
	}

	Bool CDatabase::FreeResult(QueryResult* pResult)
	{
		return true;
	}
}
