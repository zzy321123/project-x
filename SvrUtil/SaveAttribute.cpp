#include "SaveAttribute.h"
#include "UtilMalloc.h"
#include "LoggerManager.h"

namespace Util
{
	CSaveAttrib::CSaveAttrib( )
	{
		m_nAttribMax = 0;
		m_AttribParamPool = NULL;
		m_pDirtyFlag = NULL;
		m_AttribSizePool = NULL;
	}

	CSaveAttrib::~CSaveAttrib( )
	{
		UtilFree( m_AttribParamPool );
		UtilFree( m_AttribSizePool );
		m_AttribParamPool = NULL;
		m_AttribSizePool = NULL;
	}

	void	CSaveAttrib::Create( int nDirtyMax )
	{
		if( m_nAttribMax == 0 )
		{
			m_nAttribMax = nDirtyMax;
			UtilRealloc( m_pDirtyFlag, BIT_FLAG_MAX(m_nAttribMax) );
			UtilRealloc( m_AttribParamPool, sizeof(PVOID)*m_nAttribMax );
			UtilRealloc( m_AttribSizePool, sizeof(int)*m_nAttribMax );
			memset( m_pDirtyFlag, 0, BIT_FLAG_MAX(m_nAttribMax) );
			memset( m_AttribParamPool, 0, sizeof(PVOID)*m_nAttribMax );
			memset( m_AttribSizePool, 0, sizeof(int)*m_nAttribMax );
		}
		else
		{
			int nOldAttribMax = m_nAttribMax;
			m_nAttribMax = nDirtyMax;
			UtilRealloc( m_pDirtyFlag, BIT_FLAG_MAX(m_nAttribMax) );
			UtilRealloc( m_AttribParamPool, sizeof(PVOID)*m_nAttribMax );
			UtilRealloc( m_AttribSizePool, sizeof(int)*m_nAttribMax );
			memset( m_pDirtyFlag + BIT_FLAG_MAX(nOldAttribMax), 0, BIT_FLAG_MAX(m_nAttribMax) - BIT_FLAG_MAX(nOldAttribMax) );
			memset( m_AttribParamPool + nOldAttribMax, 0, sizeof(PVOID)*( m_nAttribMax - nOldAttribMax ) );
			memset( m_AttribSizePool + nOldAttribMax, 0, sizeof(int)*( m_nAttribMax -nOldAttribMax ) );
		}

		if( NULL == m_AttribParamPool )
		{
			FmtLog( "开辟连续属性内存失败" );
		}
	}

	bool	CSaveAttrib::CheckValidAttrib( int nID )
	{
		if( NULL == m_AttribParamPool )
		{
			FmtLog( "属性池没有初始化!" );
			return false;
		}

		if( nID < 0 || nID >= m_nAttribMax )
		{
			FmtLog( "属性ID范围异常! max:%d,id:%d", m_nAttribMax, nID );
			return false;
		}

		return true;
	}

	void	CSaveAttrib::_RegistAttrib( int nID, VOID* pAttrib, int nSize )
	{
		if( CheckValidAttrib( nID ) )
		{
			if( NULL != m_AttribParamPool[nID] || NULL != m_AttribSizePool[nID] )
			{
				FmtLog( "该属性已被注册过，请查明错误原因，id:%d", nID );
				return;
			}

			m_AttribParamPool[nID] = pAttrib;
			m_AttribSizePool[nID] = nSize;

		}
	}

	void	CSaveAttrib::RegistAttrib( int nID, byte*	pAttrib )
	{
		_RegistAttrib( nID, (VOID*)pAttrib, sizeof(byte) );
	}

	void	CSaveAttrib::RegistAttrib( int nID, short*	pAttrib )
	{
		_RegistAttrib( nID, (VOID*)pAttrib, sizeof(short) );
	}

	void	CSaveAttrib::RegistAttrib( int nID, int*	pAttrib )
	{
		_RegistAttrib( nID, (VOID*)pAttrib, sizeof(int) );
	}

	void	CSaveAttrib::RegistAttrib( int nID, Int64*	pAttrib )
	{
		_RegistAttrib( nID, (VOID*)pAttrib, sizeof(Int64) );
	}

	void	CSaveAttrib::RegistAttrib( int nID, float*	pAttrib )
	{
		_RegistAttrib( nID, (VOID*)pAttrib, sizeof(float) );
	}

	void	CSaveAttrib::RegistAttrib( int nID, double*	pAttrib )
	{
		_RegistAttrib( nID, (VOID*)pAttrib, sizeof(double) );
	}

	void	CSaveAttrib::RegistAttrib( int nID, char*	pAttrib, int nSize )
	{
		_RegistAttrib( nID, (VOID*)pAttrib, nSize );
	}

	void	CSaveAttrib::RegistAttrib( int nID, UINT*	pAttrib )
	{
		_RegistAttrib( nID, (VOID*)pAttrib, sizeof(UINT) );
	}

	void	CSaveAttrib::RegistAttrib( int nID, UINT64*	pAttrib )
	{
		_RegistAttrib( nID, (VOID*)pAttrib, sizeof(UINT64) );
	}

	//设置属性
	void	CSaveAttrib::_SetAttrib( int nID, VOID* pAttribValue, int nType )
	{
		if( NULL == pAttribValue )
		{
			FmtLog( "无效属性设置,ID:%d", nID );
			return;
		}

		if( CheckValidAttrib( nID ) )
		{
			if( NULL == m_AttribParamPool[nID] || NULL == m_AttribSizePool[nID] )
			{
				FmtLog( "该属性未被注册，请查明错误原因，id:%d", nID );
				return;
			}

			switch( nType )
			{
			case type_byte:
				{
					if( m_AttribSizePool[nID] != sizeof(byte) )
					{
						FmtLog( "错误的属性设置，ID:%d,请检查注册时的属性类型和传入时的属性类型，注册类型大小:%d,设置类型大小:%d", nID,  m_AttribSizePool[nID], sizeof(byte) );
						return;
					}
					*((byte*)(m_AttribParamPool[nID])) = *(byte*)pAttribValue;
					break;
				}
			case type_short:
				{
					if( m_AttribSizePool[nID] != sizeof(short) )
					{
						FmtLog( "错误的属性设置，ID:%d,请检查注册时的属性类型和传入时的属性类型，注册类型大小:%d,设置类型大小:%d",
							nID,  m_AttribSizePool[nID], sizeof(short) );
						return;
					}
					*((short*)(m_AttribParamPool[nID])) = *(short*)pAttribValue;
					break;
				}
			case type_UINT:
				{
					if( m_AttribSizePool[nID] != sizeof(UINT) )
					{
						FmtLog( "错误的属性设置，ID:%d,请检查注册时的属性类型和传入时的属性类型，注册类型大小:%d,设置类型大小:%d",
							nID,  m_AttribSizePool[nID], sizeof(UINT) );
						return;
					}
					*((UINT*)(m_AttribParamPool[nID])) = *(UINT*)pAttribValue;
					break;
				}
			case type_int:
				{
					if( m_AttribSizePool[nID] != sizeof(int) )
					{
						FmtLog( "错误的属性设置，ID:%d,请检查注册时的属性类型和传入时的属性类型，注册类型大小:%d,设置类型大小:%d",
							nID, m_AttribSizePool[nID], sizeof(int) );
						return;
					}
					*((int*)(m_AttribParamPool[nID])) = *(int*)pAttribValue;
					break;
				}
			case type_int64:
				{
					if( m_AttribSizePool[nID] != sizeof(Int64) )
					{
						FmtLog( "错误的属性设置，ID:%d,请检查注册时的属性类型和传入时的属性类型，注册类型大小:%d,设置类型大小:%d", 
							nID, m_AttribSizePool[nID], sizeof(Int64) );
						return;
					}
					*((Int64*)(m_AttribParamPool[nID])) = *(Int64*)pAttribValue;
					break;
				}
			case type_Uint64:
				{
					if( m_AttribSizePool[nID] != sizeof(UINT64) )
					{
						FmtLog( "错误的属性设置，ID:%d,请检查注册时的属性类型和传入时的属性类型，注册类型大小:%d,设置类型大小:%d", 
							nID, m_AttribSizePool[nID], sizeof(UINT64) );
						return;
					}
					*((UINT64*)(m_AttribParamPool[nID])) = *(UINT64*)pAttribValue;
					break;
				}
			case type_float:
				{
					if( m_AttribSizePool[nID] != sizeof(float) )
					{
						FmtLog( "错误的属性设置，ID:%d,请检查注册时的属性类型和传入时的属性类型，注册类型大小:%d,设置类型大小:%d", 
							nID, m_AttribSizePool[nID], sizeof(float) );
						return;
					}
					*((float*)(m_AttribParamPool[nID])) = *(float*)pAttribValue;
					break;
				}
			case type_double:
				{
					if( m_AttribSizePool[nID] != sizeof(double) )
					{
						FmtLog( "错误的属性设置，ID:%d,请检查注册时的属性类型和传入时的属性类型，注册类型大小:%d,设置类型大小:%d", 
							nID, m_AttribSizePool[nID], sizeof(double) );
						return;
					}
					*((double*)(m_AttribParamPool[nID])) = *(double*)pAttribValue;
					break;
				}
			default:
				{
					FmtLog( "错误的属性类型!ID:%d,",nID );
					return;
				}
			}

			SetDirtyAttrib( nID, TRUE );
		
		}
	}

	//设置字符串属性
	void	CSaveAttrib::_SetAttribString( int nID, char* szSource, int nSize )
	{
		if( NULL == szSource )
		{
			FmtLog( "无效属性设置ID:%d," ,nID );
			return;
		}

		if( CheckValidAttrib( nID ) )
		{
			if( NULL == m_AttribParamPool[nID] || NULL == m_AttribSizePool[nID] )
			{
				FmtLog( "该属性未被注册，请查明错误原因，id:%d", nID );
				return;
			}

			if( m_AttribSizePool[nID] < nSize )
			{
				FmtLog( "错误的属性设置，ID:%d,请检查注册时的属性类型和传入时的属性类型，注册大小:%d,设置大小:%d", 
					nID, m_AttribSizePool[nID], nSize );
				return;
			}

			char* szDes = (char*)(m_AttribParamPool[nID]);
			memset( szDes, 0 , m_AttribSizePool[nID] );
			strncpy( szDes, szSource, nSize );

			SetDirtyAttrib( nID, TRUE );
		}
	}

	//获得属性
	VOID*	CSaveAttrib::_GetAttrib( int nID, int nSize)
	{
		if( CheckValidAttrib( nID ) )
		{
			if( NULL == m_AttribParamPool[nID] || NULL == m_AttribSizePool[nID] )
			{
				FmtLog( "该属性未被注册，请查明错误原因，id:%d", nID );
				return NULL;
			}

			if( m_AttribSizePool[nID] != nSize )
			{
				FmtLog( "错误的属性获得，ID:%d 请检查注册时的属性类型和传入时的属性类型，注册大小:%d,获得大小:%d", nID, m_AttribSizePool[nID], nSize );
				return NULL;
			}

			VOID* pAttrib = m_AttribParamPool[nID];

			return pAttrib;
		}

		return NULL;
	}

	//获得字符串属性
	char*	CSaveAttrib::_GetAttribString( int nID )
	{
		if( CheckValidAttrib( nID ) )
		{
			if( NULL == m_AttribParamPool[nID] || NULL == m_AttribSizePool[nID] )
			{
				FmtLog( "该属性未被注册，请查明错误原因，id:%d", nID );
				return NULL;
			}

			return (char*)(m_AttribParamPool[nID]);
		}

		return NULL;
	}

	//////////////////////////////////////////////////////////////////////////
	//设置属性接口
	//////////////////////////////////////////////////////////////////////////
	void	CSaveAttrib::SetAttribByte( int nID, byte bValue )
	{
		_SetAttrib( nID, &bValue, type_byte );
	}

	void	CSaveAttrib::SetAttribShort( int nID, short tValue )
	{
		_SetAttrib( nID, &tValue, type_short );
	}

	void	CSaveAttrib::SetAttribInt( int nID, int nValue )
	{
		_SetAttrib( nID, &nValue, type_int );
	}

	void	CSaveAttrib::SetAttribInt64( int nID, Int64 lValue )
	{
		_SetAttrib( nID, &lValue, type_int64 );
	}

	void	CSaveAttrib::SetAttribUINT64( int nID,UINT64 lValue )
	{
		_SetAttrib( nID, &lValue, type_Uint64 );
	}

	void	CSaveAttrib::SetAttribFloat( int nID, float fValue )
	{
		_SetAttrib( nID, &fValue, type_float );
	}

	void	CSaveAttrib::SetAttribDouble( int nID,	double FValue )
	{
		_SetAttrib( nID, &FValue, type_double );
	}

	void	CSaveAttrib::SetAttribString( int nID, char* szValue )
	{
		_SetAttribString( nID, szValue, strlen( szValue ) );
	}

	void	CSaveAttrib::SetAttribUINT( int nID, UINT uValue )
	{
		_SetAttrib( nID, &uValue, type_UINT );
	}

	//////////////////////////////////////////////////////////////////////////
	//获得属性接口
	//////////////////////////////////////////////////////////////////////////
	byte	CSaveAttrib::GetAttribByte( int nID )
	{
		return *( (byte*)_GetAttrib( nID, sizeof(byte) ) );
	}

	short	CSaveAttrib::GetAttribShort( int nID )
	{
		return *( (short*)_GetAttrib( nID, sizeof(short) ) );
	}

	int		CSaveAttrib::GetAttribInt( int nID )
	{
		return *( (int*)_GetAttrib( nID, sizeof(int) ) );
	}

	Int64	CSaveAttrib::GetAttribInt64( int nID )
	{
		return *( (Int64*)_GetAttrib( nID, sizeof(Int64) ) );
	}

	UINT64	CSaveAttrib::GetAttribUINT64( int nID )
	{
		return *( (UINT64*)_GetAttrib( nID, sizeof(UINT64) ) );
	}
	float	CSaveAttrib::GetAttribFloat( int nID )
	{
		return *( (float*)_GetAttrib( nID, sizeof(float) ) );
	}

	double	CSaveAttrib::GetAttribDouble( int nID )
	{
		return *( (double*)_GetAttrib( nID, sizeof(double) ) );
	}

	char*	CSaveAttrib::GetAttribString( int nID )
	{
		return _GetAttribString( nID );
	}
	UINT	CSaveAttrib::GetAttribUINT( int nID )
	{
		return *( (UINT*)_GetAttrib( nID, sizeof(UINT) ) );
	}

	//是否是脏数据,脏数据不用写盘 返回TRUE为脏数据，FALSE为新鲜数据
	BOOL	CSaveAttrib::IsDirtyAttrib( int nAttribID )
	{
		if( MAKE_BIT_FLAG(nAttribID) < 0 || MAKE_BIT_FLAG( nAttribID ) >= BIT_FLAG_MAX( m_nAttribMax ) )
			return FALSE;

		if( m_pDirtyFlag[MAKE_BIT_FLAG( nAttribID )] & ( 1 << ( MAKE_BIT_PARAM( nAttribID ) ) ) )
			return FALSE;
		return TRUE;
	}

	//是否是新鲜数据,脏数据不用写盘 返回FALSE为脏数据，TRUE为新鲜数据
	BOOL	CSaveAttrib::IsFreshAttrib( int nAttribID )
	{
		if( MAKE_BIT_FLAG( nAttribID ) < 0 || MAKE_BIT_FLAG( nAttribID ) >= BIT_FLAG_MAX( m_nAttribMax ) )
			return FALSE;

		if( m_pDirtyFlag[MAKE_BIT_FLAG( nAttribID )] & ( 1 << ( MAKE_BIT_PARAM( nAttribID ) ) ) )
			return TRUE;

		return FALSE;
	}

	//设置脏数据标记,FALSE为脏数据，TRUE为新鲜数据
	void	CSaveAttrib::SetDirtyAttrib( int nAttribID, BOOL bValue )
	{
		if( MAKE_BIT_FLAG( nAttribID )<0 || MAKE_BIT_FLAG( nAttribID ) >= BIT_FLAG_MAX( m_nAttribMax ) )
			return;

		if ( bValue )
			m_pDirtyFlag[MAKE_BIT_FLAG( nAttribID )] |= ( 1 << ( MAKE_BIT_PARAM( nAttribID ) ) );
		else
			m_pDirtyFlag[MAKE_BIT_FLAG( nAttribID )] &= ( ~( 1 << ( MAKE_BIT_PARAM( nAttribID ) ) ) );
	}

	//设置所有标记
	void CSaveAttrib::SetDirtyAllFlag(BOOL bFlag)
	{
		if(bFlag)
		{
			for( int i=0; i < BIT_FLAG_MAX( m_nAttribMax ); i++ )
				m_pDirtyFlag[i]=(char)0xFF;
		}
		else
		{	
			memset( m_pDirtyFlag, 0, BIT_FLAG_MAX( m_nAttribMax ) );
		}
	}
}