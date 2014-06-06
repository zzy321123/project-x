#ifndef _COBJBASE_H_
#define _COBJBASE_H_

#include "RefCounter.h"
#include "RWLock.h"
#include "Mutex.h"

namespace Util
{
	/************************************************************************/
	/* �����������Ͷ���                                                     */
	/************************************************************************/
	template<class ObjKey,class ObjType> class CObjBase : public CRefCounter
	{
	public:
		//����
		CObjBase(): m_pImpl(0), m_iObjFlag(0)
		{
			m_pMutex = new CMutex;
		}

		//����
		virtual ~CObjBase()
		{
			DEF_RELEASE(m_pImpl);
			DEF_RELEASE(m_pMutex);
		}

		//����״̬ö�ٶ���
		enum
		{
			OBJ_ACTIVE	= 0x01,
			OBJ_LOCKED	= 0x02,
		};	

	public:
		//��ȡ�����ֵ
		virtual ObjKey GetObjKey() const
		{
			return m_tKey;
		}

		//��ȡʵ�����
		virtual ObjType* GetImpl()
		{
			return m_pImpl;
		}		

		//����ʵ�����, ��ObjManager�ɵ���
		virtual void SetImpl(const ObjKey& tKey, ObjType* pObj)
		{
			m_tKey  = tKey;
			m_pImpl = pObj;
			if(m_pImpl)
			{
				SetObjFlag(OBJ_ACTIVE);
			}
		}

		//�ͷ�ʵ�����, ��ObjManager�ɵ���
		virtual void FreeObject(Bool bRelease = true)
		{
			if (bRelease)
			{
				DEF_RELEASE(m_pImpl);
			}	
			
			ClearObjFlag(OBJ_ACTIVE);
		}

	public:
		//��������
		virtual Bool LockObj()
		{
			if (m_pMutex)
			{
				m_pMutex->Lock(__FILE__, __LINE__);
				SetObjFlag(OBJ_LOCKED);
				return true;
			}
			return false;
		}

		//�������
		virtual Bool UnlockObj()
		{
			if (m_pMutex)
			{
				ClearObjFlag(OBJ_LOCKED);
				m_pMutex->Unlock();
				return true;
			}			
			return false;
		}

	public:
		//���ö���ָ�����
		virtual void SetObjFlag(UInt32 iFlag)
		{
			m_iObjFlag |= iFlag;
		}

		//�������ָ�����
		virtual void ClearObjFlag(UInt32 iFlag)
		{
			m_iObjFlag &= ~iFlag; 
		}

		//��ѯ����ָ�����
		virtual Bool HasObjFlag(UInt32 iFlag) const
		{
			return (m_iObjFlag & iFlag) != 0;
		}

		//�ж϶����ͷż���״̬
		virtual Bool IsObjActive() const
		{
			return HasObjFlag(OBJ_ACTIVE);
		}

		//�ж϶����ͷ�����״̬
		virtual Bool IsObjLocked() const
		{
			return HasObjFlag(OBJ_LOCKED);
		}

		//�ж϶����Ƿ���Ч
		virtual Bool IsObjValid() const
		{
			if (m_pImpl)
				return IsObjActive();
			
			return false;
		}

		//�ж϶����Ƿ�Ϊ��
		virtual Bool IsObjEmpty() const
		{
			if (m_pImpl || HasObjFlag(OBJ_ACTIVE) || HasObjFlag(OBJ_LOCKED))
				return false;
			
			return true;
		}

	protected:
		//�����ֵ
		ObjKey		m_tKey;
		//����ʵ��
		ObjType*	m_pImpl;		
		//������
		CMutex*		m_pMutex;
		//������״̬
		UInt32		m_iObjFlag;
	};
}
#endif //_COBJBASE_H_
