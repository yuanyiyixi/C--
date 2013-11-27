#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>

using namespace std;

int main()
{
	vector<int> v(10);
	vector<int>::iterator It;
	for (int i = 0; i < 10; i++)
		v[i] = i + 1;
	copy(v.begin(), v.end(), ostream_iterator<int>(cout, " "));
	cout << endl;
	It = v.begin() + 2;
	v.erase(It);
	copy(v.begin(), v.end(), ostream_iterator<int>(cout, " "));
	cout << endl;
	It = v.begin();
	v.erase(It, It + 2);
	copy(v.begin(), v.end(), ostream_iterator<int>(cout, " "));
	cout << endl;
	return 0;
}
