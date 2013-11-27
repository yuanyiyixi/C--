#include "people.h"
#include <iostream>
using namespace std;

void People::display()
{
	cout << "People info: " << endl;
	cout << "Name: " << name << endl;
	cout << "Age: " << age << endl;

}

//void People::talk(const char * str)
void People::talk()
{
	cout << "How do you do" << endl;
	//cout << str << endl;
}

void People::display(const People & a)const
{
	cout << "Another People info: " << endl;
	cout << "Name: " << a.name << endl;
	cout << "Age: " << a.age << endl;
}
#if 1
People::People(int myage, const char *myname)
{
	cout << "ctor" << endl;
	age = myage;
	name = new char[strlen(myname) + 1];
	strcpy(name, myname);
}
People::People()
:age(10),name(NULL)	//是初始化列表
{
	//构造函数体内的语句相当于赋值，比初始化列表的语句后执行
	cout << "ctor_c" << endl;
	//age = 30;
	name = new char[strlen("XiaoWang") + 1];
	strcpy(name, "XiaoWang");
}
#endif
People::~People()
{
	cout << "dtor" << endl;
	delete[] name;
}
