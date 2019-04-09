#include <memory>
#include "SGVector.h"

void cow_singleref_write()
{
	int len = 100;
	int* data = new int[len];
	SGVector<int> x(data, len, true);
	x.set_const(25);
}

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

void cow_side_effect()
{
	int len = 100;
	int* data = new int[len];
	SGVector<int> x(data, len, true);
	x.set_const(0);
	SGVector<int> y = x;
	printf("y[0] = %d\n", y[0]);
}

int main()
{
	cow_singleref_write();
	printf("\n");
	cow_multiref_write();
	printf("\n");
	cow_const_effect();
	printf("\n");
	cow_const_effect2();
	printf("\n");
	cow_side_effect();
}