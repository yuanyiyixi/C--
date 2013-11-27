#include <iostream>
#include <fstream>
#include <cstdlib>
using namespace std;

int main(int argc, char *argv[])
{
	if (argc != 3)
	{
		cerr << "Please input ./cp file1 file2" << endl;
		exit(1);
	}

	//ifstream file1 (argv[1], ios::in | ios::binary | ios::ate);
	ifstream file1 (argv[1], ios::in | ios::binary);
	if (!file1.is_open())
	{
		cerr << "Error opening file1" << endl;
		exit(1);
	}
	ofstream file2 (argv[2], ios::out | ios::binary | ios::trunc);

	if (!file2.is_open())
	{
		cerr << "Error opening file2" << endl;
		exit(1);
	}

	char buf[1024];
	int n;
	while(file1.peek() !=EOF)
	{
		file1.read(buf, 1024);
		n = file1.gcount();
		file2.write(buf, n);
	}
	file1.close();
	file2.close();
#if 0
	char *buf;
	long size;

	size = file1.tellg(); 
	file1.seekg(0, ios::beg);
	buf = new char[size];	

	file1.read(buf, size);
	file2.write(buf, size);

	file1.close();
	file2.close();
	delete[] buf;
#endif

	return 0;
}
