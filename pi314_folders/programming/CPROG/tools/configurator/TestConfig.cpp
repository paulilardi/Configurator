#include "TestConfig.h"
#include <iostream>

using namespace std;

int main(){
	TestConfig tc, tc2, tc3;

	try{
		tc.readString("s.i=999");

		tc.jjj=6;

		tc.set("jjj","8");

		tc.readString("jjj=12");
		tc.readString("jjj=0xBAD");

		tc.set("k","[5,4,3,2,1]");

		tc.b=true;
		tc.readString("b=false");
		tc.readString("b=true");
		tc.readString("b=f");
		tc.readString("b=TrUE");
		tc.readString("b=0");

		tc.readFile("file.txt");

		tc.writeToStream(cout);

		string str = tc.toString();
		tc2.readString(str);

		static const int STRSIZE = 1000;
		char str2[STRSIZE];
		int size = tc.writeToString(str2,STRSIZE);
		tc3.readString(str2, size);

	}catch(runtime_error& e){
		cout<<e.what()<<endl;
	}

	getchar();
}