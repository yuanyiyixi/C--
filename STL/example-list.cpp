#include <iostream> 
#include <vector>
#include <deque>
#include <list>
#include <string>
#include <algorithm>
#include <iterator>
using namespace std;

template<class T> void print(list<T>& a)
{
	typename list<T>::iterator it;	//类中类要加typename

	for (it = a.begin(); it != a.end(); it++)
		cout << *it << endl;
}

int main()
{
	string str[] = {"Alex", "John", "Robert"};

	list<int> v1;
	list<int> v2(10);
	list<int> v3(10, 0);

	list<string> v4(str + 0, str + 3);
	list<string>::iterator sIt = v4.begin();

	while (sIt != v4.end())
		cout << *sIt++ << " ";
	cout << endl;

	list<string> v5(v4);
//	for (int i = 0; i < 3; i++)
/*	for (int i = 0; i < v5.size(); i++)	//v5只能用迭代器遍历
		cout << v5.at(i) << " ";
	cout << endl;*/
	print(v5);
	return 0;
}
