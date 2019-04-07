#ifndef __SGVECTOR_H__
#define __SGVECTOR_H__

#include "common.h"
#include "SGReferencedData.h"

#include <algorithm>
#include <atomic>
#include <memory>

template<class T> class SGVector : public SGReferencedData
{

//-----------------------------------------------------------------------------
// Constructors & Destructor
public:
	SGVector(T* vector, index_t vlen, bool is_cow=false) :
		SGReferencedData(true), m_vector(vector), m_vlen(vlen), m_is_cow(is_cow)
	{   }

	SGVector(const SGVector &orig) : SGReferencedData(orig)
	{
		copy_data(orig);
	}

	~SGVector()
	{
		unref();
	}

//-----------------------------------------------------------------------------
// Overloaded operators
public:
	SGVector<T> operator+(const SGVector<T> x)
	{
		SGVector<T> result = clone();
		result.add(x);
		return result;
	}

	SGVector<T>& operator=(const SGVector<T>& other)
	{
		if(&other == this) 
			return *this;
		unref();
		copy_data(other);
		copy_refcount(other);
		ref();
		return *this;
	}

	const T& operator[](uint32_t index) const
	{
		return get_vector()[index];
	}

	T& operator[](uint32_t index)
	{
		return get_vector()[index];
	}

//-----------------------------------------------------------------------------
// Vector operations
public:
	void add(const SGVector<T>& x)
	{
		auto x_vector = x.get_vector();
		auto vector = get_vector();
		for(int32_t i = 0; i < m_vlen; i++)
			vector[i] += x_vector[i];
	}

	const T& get_element(index_t index) const
	{
		return get_vector()[index];
	}

//-----------------------------------------------------------------------------
// Utility
public:
	SGVector<T> clone() const
	{
		return SGVector<T>(clone_vector(m_vector, m_vlen), m_vlen);
	}

	T* clone_vector(const T* vec, int32_t len) const
	{
		if(!vec)
			return nullptr;
		
		T* result = new T[m_vlen];
		memcpy(result, vec, sizeof(T)*m_vlen);
		return result;
	}

	void set_const(T const_elem)
	{
		auto vector = get_vector();
		for (index_t i=0; i<m_vlen; i++)
			vector[i]=const_elem;
	}

//-----------------------------------------------------------------------------
// SGReferencedData interface
protected:
	void copy_data(const SGReferencedData &orig) override
	{
		m_vector=((SGVector*)(&orig))->m_vector;
		m_vlen=((SGVector*)(&orig))->m_vlen;
		m_is_cow = ((SGVector*)(&orig))->m_is_cow;
	}

	void clone_data() override
	{
		m_vector = clone_vector(m_vector, m_vlen);
	}

	void init_data() override
	{
		m_vector = nullptr;
		m_vlen = 0;
	}

	void free_data() override
	{
		if(m_vector)
		{
			delete m_vector;
			m_vector = nullptr;
		}
		m_vlen = 0;
	}

//-----------------------------------------------------------------------------
// Getters
public:
	const T* get_vector() const {   return m_vector;    }

	T* get_vector()
	{
		if(m_is_cow)
			detach();
		return m_vector;
	}

	int get_vlen() const {  return m_vlen;  }

//-----------------------------------------------------------------------------
// Data
private:
	T* m_vector;
	index_t m_vlen;
	bool m_is_cow;
};

#endif