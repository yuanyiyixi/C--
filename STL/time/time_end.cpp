#include <deque>
//#include <vector>
using namespace std;

int main()
{
	deque<int> v(10, 0);
	//vector<int> v;

	for (int i = 0; i < 100000; i++)
		v.push_back(i);

		return 0;
}
