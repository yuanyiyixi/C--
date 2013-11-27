#include <string>	//STL中的头文件,is not <cstring> or <string.h>
#include <iostream>
#include <algorithm>	//算法, sort()排序
#include <vector>	//矢量，向量，大小会自动维护的动态数组
#include <fstream>	//处理文件
#include <iterator>	//迭代器,把它看成是访问模板库数据结构的特殊指针
using namespace std;

int main()
{
	ifstream in("name.txt");
	string strtmp;
	vector<string> vect;
	while(getline(in, strtmp, '\n'))
		vect.push_back(strtmp.substr(0, strtmp.find(' ')));
	sort(vect.begin(), vect.end());
	vector<string>::iterator it = unique(vect.begin(), vect.end());

	vector<string>::iterator ip;
	for (ip = vect.begin(); ip != it; ++ip)
		cout << *ip << endl;
//	copy(vect.begin(), it, ostream_iterator<string>(cout, "\n"));

	return 0;
}
