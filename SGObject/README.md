# Adding functionality using CRTP
Suppose we have a class representing a human:
```C++
class Human {
    void breathe();
    void eat();
    void drink();
    virtual ~Human();
};
```
If we want to model a football player, we might do the following:
```C++
class Footballer : public Human {
    void score_goal();
    void pass();
    void shoot();
};
```
And if we want to model a scientist, we might do something like:
```C++
class Scientist : public Human {
    void observe();
    void conjecture();
    void experiment();
    void verify();
};
```
Now what if we want to model a footballer, who is also a scientist. Multiple inheritance in this case is problematic, since both `Scientist` and `Footballer` have the same parent (the diamond problem). Mixzins can be used in this case as follows:
```C++
template <template<class> class Func1, template<class> class Func2>
class Human : public Func1<Human<Func1, Func2>>, public Func2<Human<Func1, Func2>> 
{
    void breathe();
    void eat();
    void drink();
    virtual ~Human();
};

template <class T>
class Footballer
{
    void score_goal();
    void pass();
    void shoot();
    virtual ~Footballer();
};

template <class T>
class Scientist
{
    void observe();
    void conjecture();
    void experiment();
    void verify();
    virtual ~Scientist();
};
```
Now we can do something like this to achieve what we want:
```C++
using FootballerScientist = Human<Footballer, Scientist>;
```

This can be extended using variadic templates to mix any number of functionalities.

## Problems
1. Polymorphism is out of the window.
2. Order matters: `Human<Footballer, Scientist>` is not the same as `Human<Scientist, Footballer>`.
3. For generic use, a lot of the functions that use it have to be template functions decorated with SFINAE type checks (to make sure that a specific functionality is available), which might cause the compilation process to be slower.
4. Classes down the inheritance hierarchy can't add functionalities to the base class.

# Adding functionality using the Visitor pattern
Suppose you have the following integer array class
```C++
class Array {
public:
    int* data;
    int len;
};
```
and you want to add some specific functionality that acts on the array elements. This can be done using a Visitor that visits the array and performs its operation, as follows:
```C++
class Array {
public:
    void accept(ArrayVisitor& visitor) {
        visitor.visit(*this);
    }
    int* data;
    int len;
};

class ArrayVisitor {
public:
    virtual void visit(Array& arr) = 0;
};
```
So to calculate the sum for example, you can have:
```C++
class ArraySum : public ArrayVisitor {
public:
    void visit(Array& arr) override {
        result = 0;
        for(int i = 0; i < arr.len; ++i)
            result += arr.data[i];
    }
    int result;
};
```