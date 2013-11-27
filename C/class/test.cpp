#include <iostream>
using namespace std;
//1.对象中只保存成员变量，不保存成员函数，如果一个成员变量都不存在，则只占一个字节
struct A{
	void foo(){cout << "foo" << endl;}
	int i;
};
int main()
{
	A a;
	cout << "sizeof(a) = " << sizeof(a) << endl; 
	return 0;
}
