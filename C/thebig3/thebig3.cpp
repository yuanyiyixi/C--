#include <iostream>
#include <cstring>
using namespace std;

//1.当对象内部有指针类型的数据成员时，应当提供深拷贝构造函数和赋值函数
//2.the big three 在类中是比较重要的函数， 作为好习惯，应当都提供我们的实现，不要使用系统提供的默认实现
//3.如果我们提供了自己的构造函数，那么系统就不再提供一个无参的默认构造函数，这个函数在很多时候都会用到，所以无参构造函数我们一般也要提供
struct Person{
public: 
	//Person();//作为好习惯，这个函数我们也应提供
	Person(int myage, const char *myname);
	Person(const Person& a);
	Person(const char *myname);
	~Person();
	Person& operator=(const Person& a);
	void display() const;
	void set_name(const char* newname);
private:
	int age;
	char *name;
};

void Person::set_name(const char* newname)
{
	name = new char[strlen(newname) + 1];
	strcpy(name, newname);
}

Person::Person(int myage, const char *myname)
:age(myage), name(NULL)
{
	cout << "ctor(int, const char *)" << endl;
	name = new char[strlen(myname) + 1];
	strcpy(name, myname);
}

#if 1
Person::Person(const Person& a)
:age(a.age), name(new char[strlen(a.name) + 1])
{
	cout <<"ctor(const Person& a)" <<  endl;
//	this->age = a.age;
//	this->name = a.name;//浅拷贝，这是系统提供的默认拷贝构造函数做的事
//	name = new char[strlen(a.name) + 1];
	strcpy(name,a.name);
}
#endif

Person::~Person()
{
	cout << "dtor" << endl;
	cout << "<dtor>Name" << name << endl;
	delete[] name;
}

#if 1
Person& Person::operator=(const Person& a)
{
	cout << "operator=" << endl;
//作用：1.提高代码效率2.保证不会因为自赋值破坏自身数据
	if(this == &a)
		return *this;

	age = a.age;
	delete[] name;
	//name = a.name;//浅拷贝，系统提供的默认赋值函数会做浅拷贝
	name = new char[strlen(a.name) + 1];
	strcpy(name, a.name);

		return *this;
}
#endif

Person::Person(const char *myname)
:age(0), name(NULL)
{
	cout << "ctor(const char*)" << endl;
	name = new char[strlen(myname) + 1];
	strcpy(name, myname);
}

void Person::display() const
{
	cout << "Person info:" << endl;
	cout << "Name:" << name << endl;
	cout << "Age:" << age << endl;
}

int main()
{
#if 0
	Person a(10, "XiaoMing"), b(15, "XiaoLi");
	a.display();
	b.display();
	a = b;
	a.display();
	 
	Person c = b;
	c.display();
#endif
	Person d = (10,"XiaoWu");
	d = "Xiaohuang";
	d.display();
	d.set_name("XiaoWu");

	return 0;
}
