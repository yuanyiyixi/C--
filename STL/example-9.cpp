#include <vector>
#include <algorithm>
#include <iostream>
using namespace std;

int main()
{
	vector<int> v;
	v.push_back(1);
	v.push_back(2);
	v.push_back(3);
	v.push_back(4);

	vector<int>::iterator p;
	p = find(v.begin(), v.end(), 3);
	if(p != v.end())
		cout << *p << endl;
	p = find(v.begin(), v.end(), 9);

	if (p == v.end())
		cout << "not fond" << endl;

	p = find(v.begin() + 1, v.end() - 2, 5);
	if (p != (v.end() - 2))
		cout << *p << endl;

	int array[] = {10, 20, 30, 40};
	int *pp = find(array, array + 4, 20);
	cout << *pp << endl;

	return 0;
}
