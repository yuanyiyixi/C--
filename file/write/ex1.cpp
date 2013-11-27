#include <fstream>
using namespace std;

int main()
{
	ofstream examplefile ("example.txt");
	file.open("example.txt", ios::out | ios::trunc);
	if (examplefile.is_open())
	{
		examplefile << "This is a line.\n";
		examplefile << "This is another line.\n";
		examplefile.close();
	}
	return 0;
}
