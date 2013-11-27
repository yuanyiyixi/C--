#include <iostream>
#include <fstream>
#include <cstdlib>
using namespace std;

const char *filename = "example.txt";
int main()
{
	long l, m;
	char buf[100];
	ifstream file (filename, ios::in | ios::binary);
	if (!file.is_open())
	{
		cerr << "Error open file";
		exit(1);
	}

	while(file.peek() != EOF)
	{
		file.getline(buf,100);
		cout << buf << endl;
	}
	file.clear();//清除流的错误状态， 否则seekg(0)不起作用
	file.seekg(0);

	while(file.peek() != EOF)
	{
		file.getline(buf,100);
		cout << buf << endl;
	}

	file.close();
	return 0;
}
