// linux build: clang++ catcher-1.cpp -o catcher-1.out
#include <iostream>
using namespace std;

int main(){
	char cmd[20] = "./a.out 13 3";
	try{
		system(cmd);
	}catch(void* e){
		cout << "Good catch";
	}
	cout << "End";
	return 0;
}
