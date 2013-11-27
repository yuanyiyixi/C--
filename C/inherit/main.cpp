#include <iostream>
#include "student.h"
using namespace std;

//隐藏hide和重载overload的区别：
//1.隐藏是指的不同类型中的同名函数，一个在父类，一个在子类，而重载是在同一个类型中出现的同名函数，或者都是顶层函数
//2.隐藏指的是函数名相同，参数不限（相同不同都可以），而重载指的是函数名相同，但参数的个数或类型必须不同

//隐藏在什么时候会用到?	当父类中的函数继承到子类中不再正确时，需要重新实现一个子类版本的相同方法，这就构成了隐藏

int main()
{
#if 0
	Student s(20, 100, "XiaoLi");
	s.Person::display(10);
	s.display();
//	s.display(10);//名字隐藏（hide）即使发生在函数参数不一样时也会发生，(父类中有参数，子类中无参数仍会隐藏)只要函数名字一样
#endif

#if 1
	cout << sizeof(Person) << endl;
	cout << sizeof(Student) << endl;

	//当使用父类指针指向堆上的子类对象时，应该让析构函数称为虚函数，否则不会调用子类的析构函数，而是调用了父类的析构函数，从而出现内存泄漏的可能
	//通常我们都推荐让一个类的析构函数成为虚函数
	Student *ps = new Student(21, 90, "XiaoWu");
	ps->display();// 编译时(函数)绑定,根据ps指针类型，在编译时去决定调用哪一个函数（跟指针类型对应）
	delete ps;
#endif
#if 0
	//用子类指针指向父类对象,这是错误的，C++不允许子类指针指向父类对象
	//但强制类型转换可以，不过不要这样做，这样是不安全的
	Student *ps = (Student *)new Person(20, "XiaoLi");
	ps->display();
	delete ps;
#endif
	return 0;
}
