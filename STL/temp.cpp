#include <iostream>
using namespace std;

class A{
public:
	enum Color{Red = 1, Black, Blue, White, Yellow};
	Color c;
	void set_color(A::Color newcolor);
};
void A::set_color(A::Color newcolor)
{
	c = newcolor;
}
int main()
{
	A a;
	a.c = A::Red;
	return 0;
}
