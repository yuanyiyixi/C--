#include <vector>
#include <iostream>
using namespace std;
int main()
{
	vector<int> v1;
	vector<int> v2;
//	v1.push_back (5);
	v1.push_back (1);
	v1.push_back (2);
	v1.push_back (3);
	v2.push_back (1);
	v2.push_back (2);
	v2.push_back (3);
	cout << (v1 < v2) << endl;
	return 0;
}
