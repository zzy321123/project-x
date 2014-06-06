#ifndef _SAVEATTRIBUTE_H_
#define _SAVEATTRIBUTE_H_

#include "BaseHeader.h"
#include "RefCounter.h"

#define BIT_FLAG_MAX( num )	( ( num >> 3) + 1 )
#define MAKE_BIT_FLAG( flag ) ( flag >> 3 )
#define MAKE_BIT_PARAM( flag ) ( flag % 8 )

namespace Util
{
	/************************************************************************/
	/* 需要存盘的属性管理器基类 所有需要存盘的属性管理器继承此类			*/
	/************************************************************************/
	class CSaveAttrib : public CRefCounter
	{
	public:
		CSaveAttrib( );
		~CSaveAttrib( );
	
		//属性类型
		enum
		{
			type_byte,
			type_short,
			type_int,
			type_UINT,
			type_int64,
			type_Uint64,
			type_float,
			type_double,
		};

	public:	
		// 属性开辟索引空间
		void		Create( int nDirtyMax );
		
		// ---------------------------------------------------------------------------------------
		// 注册属性的接口，所有需要存盘的属性必须注册才能使用
		void		RegistAttrib( int nID, byte*	pAttrib );
		void		RegistAttrib( int nID, short*	pAttrib );
		void		RegistAttrib( int nID, int*		pAttrib );
		void		RegistAttrib( int nID, UInt32*	pAttrib );
		void		RegistAttrib( int nID, Int64*	pAttrib );
		void		RegistAttrib( int nID, UInt64*	pAttrib );
		void		RegistAttrib( int nID, float*	pAttrib );
		void		RegistAttrib( int nID, double*	pAttrib );
		void		RegistAttrib( int nID, char*	pAttrib, int nSize );

		// ---------------------------------------------------------------------------------------
		// 设置属性接口
		void		SetAttribByte( int nID,	byte bValue );
		void		SetAttribShort( int nID, short tValue );
		void		SetAttribInt( int nID,	int nValue );
		void		SetAttribUINT( int nID, UInt32 uValue );
		void		SetAttribInt64( int nID, Int64 lValue );
		void		SetAttribUINT64( int nID,UInt64 lValue );
		void		SetAttribFloat( int nID, float fValue );
		void		SetAttribDouble( int nID, double FValue );
		void		SetAttribString( int nID, char* szValue );
	
	
		// ---------------------------------------------------------------------------------------
		// 获得属性接口
		byte		GetAttribByte( int nID );
		short		GetAttribShort( int nID );
		int			GetAttribInt( int nID );
		UINT		GetAttribUINT( int nID );
		Int64		GetAttribInt64( int nID );
		UINT64		GetAttribUINT64( int nID );
		float		GetAttribFloat( int nID );
		double		GetAttribDouble( int nID );
		char*		GetAttribString( int nID );

		// ---------------------------------------------------------------------------------------
		// 属性是否被更新的一些接口,用于数据刷新，入库
		// ---------------------------------------------------------------------------------------
		// 所有属性只有在被刷新的情况下，才会写入到数据库
		// 原理：
		/*
			0:表示脏数据，不用被写盘
				1:表示新鲜数据，可以被写盘

			   |------1------| |------2------|
		key:   1 2 3 4 5 6 7 8 9 10 ............
			   _____________________________________________________
		flag: |0|1|0|0|0|1|0|1|0|0|1|0|0|0|1|0|1|0|0|1|0|0|0|1|0|1|0  .....

			  每一位所在标记的位置游标表示属性的Key
			  每一位的值，由0,1表示此key对应的属性是否是脏数据
			  8比特表示一组，标记为所开空间为 (key的数量/8)+1  字节*/

		//是否是脏数据,脏数据不用写盘 返回TRUE为脏数据，FALSE为新鲜数据
		BOOL		IsDirtyAttrib( int nAttribID );

		//是否是新鲜数据,脏数据不用写盘 返回FALSE为脏数据，TRUE为新鲜数据
		BOOL		IsFreshAttrib( int nAttribID );

		//设置脏数据标记,FALSE为脏数据，TRUE为新鲜数据
		void		SetDirtyAttrib( int nAttribID,BOOL	bValue );

		//设置所有标记
		void		SetDirtyAllFlag( BOOL bFlag );

		//检查属性有效
		bool		CheckValidAttrib( int nID );
	protected:
		//注册属性
		void		_RegistAttrib( int nID, VOID* pAttrib, int nSize );

		//设置属性
		void		_SetAttrib( int nID, VOID* pAttribValue, int nType );

		//设置字符串属性
		void		_SetAttribString( int nID, char* szSource, int nSize );
	
		//获得属性
		VOID*		_GetAttrib( int nID, int nSize);

		//获得字符串属性
		char*		_GetAttribString( int nID );

	protected:
		PVOID*		m_AttribParamPool;	//注册的属性索引池
		int*		m_AttribSizePool;	//每个属性的大小池
		int			m_nAttribMax;		//属性数量最大值
		char*		m_pDirtyFlag;		//脏属性标记池
	};
}

#endif // _SAVEATTRIBUTE_H_