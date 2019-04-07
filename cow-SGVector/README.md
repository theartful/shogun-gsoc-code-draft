# Copy-on-write SGVector

This is a minimal working example of a copy-on-write SGVector.

## Key Changes
Add detach function in SGReferencedData:
```C++
void detach()
{
  // if the data is shared with another vector
  if(m_refcount->ref_count() > 1) 
  {
    printf("detach(): detaching from data and creating new copy %p\n", this);
    unref(false);
    m_refcount = new RefCount(0);
    ref();
    clone_data();
  }
  else
  {
    printf("detach(): no need to detach %p\n", this);
  }
}
```

Use getters instead of accessing the member variable `SGVector::vector` directly:
```C++
const T* get_vector() const {   return m_vector;    }

T* get_vector()
{
  if(m_is_cow)
    detach();
  return m_vector;
}
```
Change every direct access inside the SGVector class to use the getter instead. For example:
```C++
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
```

## Tests
*Test Case:* If an `SGVector` is the only reference to the data, it shouldn't copy on write.

*Code:*
```C++
void cow_singleref_write()
{
	int len = 100;
	int* data = new int[len];
	SGVector<int> x(data, len, true);
	x.set_const(25);
}
```
*Output:*
```
ref() refcount 1 data 0x7ffcb9b8ec00 increased
detach(): no need to detach 0x7ffcb9b8ec00
unref() refcount 0 data 0x7ffcb9b8ec00 destroying
```

*Test Case:* If an `SGVector` is not the only reference to the data, it should copy on write.

*Code:*
```C++
void cow_multiref_write()
{
	int len = 100;
	int* data = new int[len];
	SGVector<int> x(data, len, true);
	x.set_const(24);

	SGVector<int> y = x;
	printf("x[0] = %d\n", x.get_element(0));
	printf("y[0] = %d\n", y.get_element(0));
	printf("data[0] = %d\n", data[0]);
	
	y.set_const(5);
	printf("x[0] = %d\n", x.get_element(0));
	printf("y[0] = %d\n", y.get_element(0));
	printf("data[0] = %d\n", data[0]);
	
	x.set_const(3);
	y.set_const(4);
	printf("x[0] = %d\n", x.get_element(0));
	printf("y[0] = %d\n", y.get_element(0));
	printf("data[0] = %d\n", data[0]);
}
```
*Output:*
```
ref() refcount 1 data 0x7ffd8bf1d850 increased
detach(): no need to detach 0x7ffd8bf1d850
ref() refcount 2 data 0x7ffd8bf1d870 increased
x[0] = 24
y[0] = 24
data[0] = 24
detach(): detaching from data and creating new copy 0x7ffd8bf1d870
unref() refcount 1 data 0x7ffd8bf1d870 decreased
ref() refcount 1 data 0x7ffd8bf1d870 increased
x[0] = 24
y[0] = 5
data[0] = 24
detach(): no need to detach 0x7ffd8bf1d850
detach(): no need to detach 0x7ffd8bf1d870
x[0] = 3
y[0] = 4
data[0] = 3
unref() refcount 0 data 0x7ffd8bf1d870 destroying
unref() refcount 0 data 0x7ffd8bf1d850 destroying
```

*Test Case:* A constant `SGVector` implies constant members

*Code:*
```C++
void cow_const_effect()
{
	int len = 100;
	int* data = new int[len];
	const SGVector<int> x(data, len, true);
	auto x_vector = x.get_vector();
	// x_vector[0] = 2; // won't compile; read-only location

	int len2 = 100;
	int* data2 = new int[len2];
	SGVector<int> y(data, len, true);
	auto y_vector = y.get_vector();
	// compiles fine
	y_vector[0] = 3;
}
```

*Test Case:* A cow `SGVector` should not detach when const functions are called

*Code:*
```C++
void cow_const_effect2()
{
	int len = 100;
	int* data = new int[len];
	SGVector<int> x(data, len, true);
	
	int len2 = 100;
	int* data2 = new int[len2];
	SGVector<int> y(data, len, true);
	// tries to detach since add is non-const
	y.add(x);

	// doesn't try to detach since get_element is const
	auto val = y.get_element(2);
}
```
*Output:*
```C++
ref() refcount 1 data 0x7ffcb9b8ebe0 increased
ref() refcount 1 data 0x7ffcb9b8ec00 increased
detach(): no need to detach 0x7ffcb9b8ec00          // y.add(x) tries to detach
unref() refcount 0 data 0x7ffcb9b8ec00 destroying
unref() refcount 0 data 0x7ffcb9b8ebe0 destroying
```
*Test Case:* A side effect of this is when using the `[]` operator with a non-const `SGVector` it detaches itself

*Code:*
```C++
void cow_side_effect()
{
	int len = 100;
	int* data = new int[len];
	SGVector<int> x(data, len, true);
	x.set_const(0);
	SGVector<int> y = x;
	printf("y[0] = %d\n", y[0]);
}
```
*Output:*
```
ref() refcount 1 data 0x7fffd0223ee0 increased
detach(): no need to detach 0x7fffd0223ee0
ref() refcount 2 data 0x7fffd0223f00 increased
detach(): detaching from data and creating new copy 0x7fffd0223f00
unref() refcount 1 data 0x7fffd0223f00 decreased
ref() refcount 1 data 0x7fffd0223f00 increased
y[0] = 0
unref() refcount 0 data 0x7fffd0223f00 destroying
unref() refcount 0 data 0x7fffd0223ee0 destroying
```
