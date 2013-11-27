#include <iostream>
using namespace std;

void myswap(int *a , int *b)
{
	int tmp;
	tmp = *a;
	*a = *b;
	*b = tmp;
}
void myswap (int& a, int& b)
{
	int tmp;
	tmp = a;
	a = b;
	b = tmp;
}

int  arr[] = {1, 2, 3};
/*const*/ int& foo(int in)
{
	//error!
	//int tmp;
	//return tmp; 

	return arr[in];
}
//引用的用法
//引用可以做函数的参数，相当于传参传的对象本身
//引用可以做函数的返回值类型 注：不能返回局部变量的引用
int main()
{
	int m = 1, mm(2);//int mm(2)等价于int mm = 2
	int &n = m; //int &n; n = m;不正确，必须创建时初始化
	n = mm;// 相当于m = mm
	int *pm = &m;

	cout << "&pm = 0x" << hex << &pm << endl;
	cout << "&m = 0x" << hex << &m << endl;
	cout << "&n = 0x" << hex << &n << endl;
	cout << dec;

	int a = 1, b = 2;
	myswap(&a, &b);
	cout << "a = " << a << ", b = " << b << endl;
	myswap(a, b);
	cout << "a = " << a << ", b = " << b << endl;
	
//	foo(2) = 1000;
	cout << "arr[2] = " << arr[2] << endl;
	return 0;
}
