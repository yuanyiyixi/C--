#include <vector>
#include <iostream>
using namespace std;

int main()
{
	int i, a[5] = {1, 2, 3, 4, 5};
	vector<int> v(5);
	//cout << a[*v.begin()] << endl;
	cout << v.end() - v.begin() << endl;
	for (i = 0; i < v.size(); i++)
		v[i] = i;
	v.at(4) = 100;
	for (i = 0; i < v.size(); i++)
		cout << v[i] << ",";
	cout << endl;
	vector<int> v2(a, a + 5);
	v2.insert (v2.begin() + 2, 13);
	for (i = 0; i < v2.size(); i++ )
		cout << v2[i] << ",";
	return 0;
}
