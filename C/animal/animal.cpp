#include <iostream>
using namespace std;
//多态的定义：
//在程序中运行时，依据实际的对象类型，来决定绑定哪个子类的同名函数的方法，叫做多态，他是一种运行时（函数）绑定
//多态有时又叫override重写或覆盖,它和hide隐藏不一样，hide不能有virtual关键字，而且hide的函数参数可以不一样

// 多态的三要素：
//1.要有一个继承关系，。诶有继承就没有多态
//2.子类中要有和父类中名字相同的同名函数， 并且连参数都要一样
//3.使用父类指针或者父类引用去调用这些同名方法

//哪些函数可以成为虚函数（前面机上virtual关键字）？
//1.只有类的成员函数才能加上virtual关键字，顶层函数（比如main）不能成为虚函数
//2.构造函数不可以成为虚函数，但析构函数可以
//3.static静态成员函数也不能成为虚函数
class Animal{
public:
	//virtual static void foo(){}	//error!
	//virtual Animal(){}	//error!
	~Animal();
	virtual void talk(){cout << "Animal" << endl;}
	virtual void foo(){cout << "Animal::foo" << endl;}
	//virtual void talk() = 0;//纯虚函数（纯粹的虚函数），相当于仅仅提供了一个接口,需要在实现子类时实现这个接口,包含纯虚函数的类是抽象类。抽象类不可实例化
};

Animal::~Animal()
{
}

class Dog : public Animal{
public:
	void talk(){cout << "Wang Wang" << endl;}
	void foo(){cout << "Dog::foo" << endl;}
};

class Cat : public Animal{
public:
	void talk(){cout << "Mao Mao" << endl;}
	void foo(){cout << "Cat::foo" << endl;}
};

void saywhat(Animal *p)
{
	p->talk();
}

void saywhat(Animal &a)
{
	a.talk();
}

//虚函数表：
//1.在对象开头有一个指向虚函数表的指针， 相同类型的对象共享一份虚函数表
//2.虚函数表按类型来实现， 是由C++编译器在构造函数中添加的代码自动填写这张表的， 表的内容是一些指向虚函数的函数指针
int main()
{
	Animal a, b;
	cout << "vptr a=0x" << hex << *(int *)&a << endl;
	cout << "vptr b=0x" << *(int *)&b << endl;
	//手动查找这张虚函数表内的函数指针，然后用指针执行虚函数
	void (*p)(void) = (void(*)(void)) (*(int *)((*(int *)&a) + 4));
	cout << "========" << endl;
	p();
	a.talk();
	cout << "========" << endl;
	//a.talk();

	Cat c;
	cout << "vptr c=0x" << *(int *)&c << endl;
	p =(void(*)(void))(*(int *)((*(int *)&c) + 4));
	p();
//	c.talk();

	Dog d;
	cout << "vptr d=0x" << *(int *)&d << endl;
	p =(void(*)(void))(*(int *)((*(int *)&d) + 4));
	p();
//	d.talk();
#if 0
	saywhat(&a);
	saywhat(&c);
	saywhat(&d);
	cout << "======" << endl;
	saywhat(a);
	saywhat(c);
	saywhat(d);
#endif
	return 0;
}
