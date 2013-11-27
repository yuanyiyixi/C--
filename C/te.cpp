 #include<iostream>

 using namespace std;

  

  int main(int argc, char** argv)

  {

	  int i=1;

	  int& ref=i;

	  int x=ref;

	  cout<<"x is "<<x<<endl;

	   

	   ref=2;

	   int* p=&i;

	   cout<<"ref = "<<ref<<", i = "<<i<<endl;

  }
