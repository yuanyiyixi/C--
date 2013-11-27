#include <iostream>
#include <set>
#include <map>
#include <iterator>
#include <string>
using namespace std;

int main()
{
	set<int> s;
	s.insert(1);
	s.insert(132);
	s.insert(168);
	s.insert(20);

	set<int>::const_iterator it;
	for (it = s.begin(); it != s.end(); it++)
		cout << *it << " ";
	cout << endl;

	int key;
	cout << "please enter one number to find : ";
		cin >> key;

	set<int>::iterator result_it;
	result_it = s.find(key);
	if(result_it != s.end())
		cout << "ok! you find it in the set!" << endl;
	else 
		cout << "no! you can't find it" << endl;
	map<string, int> m;
	m.insert(pair<string, int>("zero", 0));
	m["one"] = 1;
	m["two"] = 2;
	m["three"] = 3;

	cout << m["two"] << endl;
	cout << "size=" << m.size() << endl;
	map<string, int>::iterator mapIt;
	mapIt = m.find("zero");
	if (mapIt !=m.end())
		cout << mapIt->second << endl;
	else 
		cout << "can't find it!" << endl;
		cout << "==========" << endl;
		for (mapIt = m.begin(); mapIt != m.end(); ++mapIt)
			cout << mapIt->first << "," << mapIt->second << endl;
			cout << endl;
			return 0;
}
