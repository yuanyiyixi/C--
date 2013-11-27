#include <iostream> 
#include <vector>
#include <deque>
#include <list>
#include <string>
#include <algorithm>
#include <iterator>
using namespace std;

template<class T> void print(vector<T>& a)
{
	typename vector<T>::iterator it;	//类中类要加typename

	for (it = a.begin(); it != a.end(); it++)
		cout << *it << endl;
}

int main()
{
	string str[] = {"Alex", "John", "Robert"};

	vector<int> v1;
	vector<int> v2(10);
	vector<int> v3(10, 0);

	vector<string> v4(str + 0, str + 3);
	vector<string>::iterator sIt = v4.begin();

	while (sIt != v4.end())
		cout << *sIt++ << " ";
	cout << endl;

	vector<string> v5(v4);
//	for (int i = 0; i < 3; i++)
	for (int i = 0; i < v5.size(); i++)
		cout << v5[i] << " ";
	cout << endl;
	print(v5);
	return 0;
}
