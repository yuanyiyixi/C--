#include <iostream> 
#include <vector>
#include <deque>
#include <string>
#include <algorithm>
#include <iterator>
using namespace std;

//template<class T> void print(vector<T>& a)
template<class T> void print(deque<T>& a)
{
	//typename vector<T>::iterator it;	//类中类要加typename
	typename deque<T>::iterator it;	//类中类要加typename

	for (it = a.begin(); it != a.end(); it++)
		cout << *it << endl;
}

int main()
{
	string str[] = {"Alex", "John", "Robert"};

	deque<int> v1;
	deque<int> v2(10);
	deque<int> v3(10, 0);

	deque<string> v4(str + 0, str + 3);
//	deque<string>::iterator sIt = v4.begin();//read and write
	deque<string>::const_iterator sIt = v4.begin();//only read

	while (sIt != v4.end())
		cout << *sIt++ << " ";
	cout << endl;

	deque<string> v5(v4);
//	for (int i = 0; i < 3; i++)
	for (int i = 0; i < v5.size(); i++)
		cout << v5[i] << " ";
	cout << endl;
	print(v5);
	return 0;
}
