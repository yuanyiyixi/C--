#include "stack.h"

int main(){
	Stack<int> instack1, instack2;
	int val;
	for (val = 0; val < 4; ++val){
		instack1.push(val);
		instack2.push(2 * val);
	}
	while(instack1.count()){
		val = instack1.pop();
		cout << val << endl;
	}
	Stack<char> stringstack;
	stringstack.push('A');
	stringstack.push('B');
	stringstack.push('C');
	char val2;
	while (stringstack.count()){
		val2 = stringstack.pop();
		cout << val2 << endl;
	}
	cout << "Now instack2 will self destruct." << endl;

	return 0;
}
