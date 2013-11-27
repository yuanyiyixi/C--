#include <iostream>
//#include <stdio.h>
#include <cstdio>//c++中的stdio做了封装
using namespace std;

namespace A{
	int a;
	void goo(){cout << "A::goo" << endl;}
};
//using namespace A;
using A::a;

namespace B{
	int a;
}
//using namespace B;
//void goo(){cout << "goo" << endl;}
namespace {
	void foo(){cout << "::foo" << endl;}
}
void foo(){cout << "foo" << endl;}
int a;
int main()
{
	//	int a;
	//a = 10;
	A::a = 1000;
	B::a = 100;
	//printf("a =  %d\n", a);
	//std::cout << "a=" << a << std::endl;//std的名字空间里才有cout对象
	cout << "a=" << ::a << endl;

	cout << "A::a=" << A::a << endl;
	cout << "B::a=" << B::a << endl;
	printf("<printf>a = %d\n",a);

	::foo();

	return 0;
}
