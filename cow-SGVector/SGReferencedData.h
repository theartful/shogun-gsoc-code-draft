#ifndef __SGREFERENCED_DATA_H__
#define __SGREFERENCED_DATA_H__

#include <iostream>
#include "common.h"
#include "RefCount.h"

class SGReferencedData
{
	public:
		/** default constructor */
		SGReferencedData(bool ref_counting=true) : m_refcount(NULL)
		{
			if (ref_counting)
			{
				m_refcount = new RefCount(0);
			}

			ref();
		}

		/** copy constructor */
		SGReferencedData(const SGReferencedData &orig)
		{
			copy_refcount(orig);
			ref();
		}

		/** override assignment operator to increase refcount on assignments */
		SGReferencedData& operator= (const SGReferencedData &orig);

		/** empty destructor
		 *
		 * NOTE: unref() has to be called in derived classes
		 * to avoid memory leaks.
		 */
		virtual ~SGReferencedData() {}

		/** display reference counter
		 *
		 * @return reference count
		 */
		int32_t ref_count()
		{
			if (m_refcount == nullptr)
				return -1;
			int32_t c = m_refcount->ref_count();
			printf("ref_count(): refcount %d, data %p\n", c, this);
			return c;
		}

		// NEW
		void detach()
		{
			// if the data is shared with another vector
			if(m_refcount->ref_count() > 1)
			{
				printf("detach(): detaching from data and creating new copy %p\n", this);
				unref(false);
				m_refcount = new RefCount(0);
				ref();
				// like copy_data but creates a deep copy
				clone_data();
			}
			else
			{
				printf("detach(): no need to detach %p\n", this);
			}
		}

	protected:
		/** copy refcount */
		void copy_refcount(const SGReferencedData &orig)
		{
			m_refcount =  orig.m_refcount;
		}

		/** increase reference counter
		 *
		 * @return reference count
		 */
		int32_t ref()
		{
			if (m_refcount == nullptr)
				return -1;
			int32_t c = m_refcount->ref();
			printf("ref() refcount %d data %p increased\n", c, this);
			return c;
		}

		/** decrement reference counter and deallocate object if refcount is zero
		 * before or after decrementing it
		 *
		 * @return reference count
		 */
		int32_t unref(bool update_data=true)
		{
			if (m_refcount == NULL)
			{
				if(update_data) init_data();
				m_refcount=NULL;
				return -1;
			}
			int32_t c = m_refcount->unref();
			if (c<=0)
			{
				printf("unref() refcount %d data %p destroying\n", c, this);
				if(update_data) free_data();
				delete m_refcount;
				m_refcount=NULL;
				return 0;
			}
			else
			{
				printf("unref() refcount %d data %p decreased\n", c, this);
				if(update_data) init_data();
				m_refcount=NULL;
				return c;
			}
		}

		// NEW
		virtual void clone_data()=0;

		/** needs to be overridden to copy data */
		virtual void copy_data(const SGReferencedData &orig)=0;

		/** needs to be overridden to initialize empty data */
		virtual void init_data()=0;

		/** needs to be overridden to free data */
		virtual void free_data()=0;

	private:

		/** reference counter */
		RefCount* m_refcount;
};
#endif // __SGREFERENCED_DATA_H__
