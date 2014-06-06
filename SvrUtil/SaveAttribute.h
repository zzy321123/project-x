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
	/* ��Ҫ���̵����Թ��������� ������Ҫ���̵����Թ������̳д���			*/
	/************************************************************************/
	class CSaveAttrib : public CRefCounter
	{
	public:
		CSaveAttrib( );
		~CSaveAttrib( );
	
		//��������
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
		// ���Կ��������ռ�
		void		Create( int nDirtyMax );
		
		// ---------------------------------------------------------------------------------------
		// ע�����ԵĽӿڣ�������Ҫ���̵����Ա���ע�����ʹ��
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
		// �������Խӿ�
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
		// ������Խӿ�
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
		// �����Ƿ񱻸��µ�һЩ�ӿ�,��������ˢ�£����
		// ---------------------------------------------------------------------------------------
		// ��������ֻ���ڱ�ˢ�µ�����£��Ż�д�뵽���ݿ�
		// ԭ��
		/*
			0:��ʾ�����ݣ����ñ�д��
				1:��ʾ�������ݣ����Ա�д��

			   |------1------| |------2------|
		key:   1 2 3 4 5 6 7 8 9 10 ............
			   _____________________________________________________
		flag: |0|1|0|0|0|1|0|1|0|0|1|0|0|0|1|0|1|0|0|1|0|0|0|1|0|1|0  .....

			  ÿһλ���ڱ�ǵ�λ���α��ʾ���Ե�Key
			  ÿһλ��ֵ����0,1��ʾ��key��Ӧ�������Ƿ���������
			  8���ر�ʾһ�飬���Ϊ�����ռ�Ϊ (key������/8)+1  �ֽ�*/

		//�Ƿ���������,�����ݲ���д�� ����TRUEΪ�����ݣ�FALSEΪ��������
		BOOL		IsDirtyAttrib( int nAttribID );

		//�Ƿ�����������,�����ݲ���д�� ����FALSEΪ�����ݣ�TRUEΪ��������
		BOOL		IsFreshAttrib( int nAttribID );

		//���������ݱ��,FALSEΪ�����ݣ�TRUEΪ��������
		void		SetDirtyAttrib( int nAttribID,BOOL	bValue );

		//�������б��
		void		SetDirtyAllFlag( BOOL bFlag );

		//���������Ч
		bool		CheckValidAttrib( int nID );
	protected:
		//ע������
		void		_RegistAttrib( int nID, VOID* pAttrib, int nSize );

		//��������
		void		_SetAttrib( int nID, VOID* pAttribValue, int nType );

		//�����ַ�������
		void		_SetAttribString( int nID, char* szSource, int nSize );
	
		//�������
		VOID*		_GetAttrib( int nID, int nSize);

		//����ַ�������
		char*		_GetAttribString( int nID );

	protected:
		PVOID*		m_AttribParamPool;	//ע�������������
		int*		m_AttribSizePool;	//ÿ�����ԵĴ�С��
		int			m_nAttribMax;		//�����������ֵ
		char*		m_pDirtyFlag;		//�����Ա�ǳ�
	};
}

#endif // _SAVEATTRIBUTE_H_