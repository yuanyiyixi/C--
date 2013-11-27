//cosnt成员变量只能在构造函数初始化列表里进行初始化
class A{
public:
	A(int i):SIZE(i)	//  初始化列表才执行初始化
	{
	//	SIZE = i;	//构造函数体内相当于赋值
	}
	const int SIZE;
};

int main()
{
	A a(10);

	return 0;
}
