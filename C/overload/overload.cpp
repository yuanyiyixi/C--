#include <iostream>
using namespace std;

int add(int a, int b)
{
	return a + b;
}
#if 1
float add(float a, float b)
{
	return a + b;
}
#endif
double add (double a, double b)
{
	return a + b;
}

int main(void)
{
	cout << add(1,2) << endl << add((float)2.1,float(3.14)) << endl;
	cout << add(1,2) << endl << add(2.1f,3.14f) << endl;
	return 0;
}
