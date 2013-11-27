#include <iostream>
#include <fstream>
#include <stdlib.h>
using namespace std;

int main()
{
	char buf[256];
	ifstream examplefile ("example.txt");
	if (!examplefile.is_open())
	{
		cout << "Error opening file" << endl;
		exit (1);
	}
	while(!examplefile.eof())//eof()会多读一次
	//while(examplefile.peek() != EOF)//peek()不会多读
	{
		examplefile.getline (buf, 100);
		if (examplefile.good())
		cout << buf << endl;
	//	cout << "read byte:" << examplefile.gcount() << endl;
	//	cout << "=========" << endl;
	}
	return 0;
}
