#include <iostream>
using namespace std;

extern void testTape();
extern void testTable();
extern void testMachine();

int main() {


	cout << "-----------------------------------" << endl;
	cout << "<<< Turing Machine >>>" << endl;
	cout << "-----------------------------------" << endl;
	cout << "1.Test Table" << endl;
	cout << "2.Test Tape" << endl;
	cout << "3.Test Machine" << endl;
	cout << "-----------------------------------" << endl;
	int choice;
	cout << "[ Input ] :" << endl;
	cin >> choice;

	if (!(choice <= 3 or choice >= 1)) {
		cout << "Error : Unexpected Input" << endl;
		exit(0);
	}

	if (choice == 1) {
		testTable();
	}
	else if (choice == 2) {
		testTape();
	}
	else if (choice == 3) {
		testMachine();
	}
	else {
		cout << "Error : Unexpected Input" << endl;
		exit(0);
	}


}