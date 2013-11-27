#include <fstream>
#include <iostream>
#include <cstdlib>
using namespace std;

//ios::trunc 和ios::app 是互斥的,不能同时存在
//ios::ate 可以和ios::trunc在一起出现,这里的ios::ate其实是和ios::in配合，表示到文件末尾读数据
int main()
//ios::ate 和ios::app类似，都是在文件结尾操作，但是ios::app是针对写，而ios::ate针对读写有效，另外ios::ate在文件不存在时不会创建文件，而ios::app会创建新文件
{
	fstream file;
	//file.open("test.txt", ios::in | ios::out | ios::app);//OK
	//file.open("test.txt", ios::in | ios::out | ios::trunc | ios::app);//error
	//file.open("test.txt", ios::in | ios::out | ios::trunc | ios::ate);//OK
	file.open("test.txt", ios::in | ios::out | ios::ate);//OK
	if (!file.is_open())
	{
		cerr << "file open error! " << endl;
		exit(1);
	}

	//对文件的操作
	file << "abcd";
	file << "e";
	file << "f";

	file.close();
}
