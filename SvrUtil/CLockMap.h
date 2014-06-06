#ifndef C_LOCKMAP_H
#define C_LOCKMAP_H

#include "Mutex.h"

namespace Util
{
	template<class TKey, class TVal> class CLockMap : protected std::map<TKey,TVal>
	{
	public:
		CLockMap() : std::map<TKey,TVal>()
		{
			m_pLock = new CMutex();
		}

		~CLockMap()
		{
			DEF_RELEASE(m_pLock);
		}

	public:
		typedef typename std::map<TKey,TVal>::iterator		 iterator;
		typedef typename std::map<TKey,TVal>::const_iterator const_iterator;

	public:
		//��С
		Size_t Size() const
		{
			return this->size();
		}

		//����
		Bool Lock()
		{
			if (m_pLock)
			{
				m_pLock->Lock(__FILE__, __LINE__);
				return true;
			}
			return false;
		}

		//����
		Bool Unlock()
		{
			if (m_pLock)
			{
				m_pLock->Unlock();
				return true;
			}
			return false;
		}

		//��ʼ
		iterator Begin()
		{
			return this->begin();
		}

		//��ʼ
		const_iterator Begin() const
		{
			return this->begin();
		}

		//����
		iterator End()
		{
			return this->end();
		}

		//����
		const_iterator End() const
		{
			return this->end();
		}

		//����
		Bool Find(const TKey& tKey, TVal* pVal = 0)
		{
			AutoMutex(lock, m_pLock);

			iterator it = this->find(tKey);
			if (it != this->end())
			{
				if(pVal)
				{
					*pVal = it->second;	
				}
				return true;
			}

			return false;
		}

		//ɾ��
		Bool Erase(const TKey& tKey,TVal* pVal = 0)
		{
			AutoMutex(lock, m_pLock);

			iterator it = this->find(tKey);
			if (it != this->end())
			{
				if(pVal)
				{
					*pVal = it->second;
				}
				this->erase(it);
				return true;
			}

			return false;
		}

		//����
		void Set(const TKey& tKey,const TVal& tVal)
		{
			AutoMutex(lock, m_pLock);

			(*this)[tKey] = tVal;
		}

		void Clear()
		{
			AutoMutex(lock, m_pLock);

			this->clear();
		}		

	public:
		//�����ռ���������
		void CollectKey(vector<TKey>& vKey)
		{
			AutoMutex(lock, m_pLock);

			iterator it = this->begin();
			for (;it != this->end();it++)
			{
				vKey.push_back(it->first);
			}
		}

		void CollectValue(vector<TVal>& vVal)
		{
			AutoMutex(lock, m_pLock);

			iterator it = this->begin();
			for (;it != this->end();it++)
			{
				vVal.push_back(it->second);
			}
		}

		void CollectKeyValue(vector<TKey>& vKey,vector<TVal>& vVal)
		{
			AutoMutex(lock, m_pLock);

			iterator it = this->begin();
			for (;it != this->end();it++)
			{
				vKey.push_back(it->first);
				vVal.push_back(it->second);
			}
		}

		void Copy(map<TKey,TVal>& mCpy)
		{
			AutoMutex(lock, m_pLock);

			iterator it = this->begin();
			for (;it != this->end();it++)
			{
				mCpy[it->first] = it->second;
			}
		}

	protected:
		CMutex*    m_pLock;
	};
}
#endif