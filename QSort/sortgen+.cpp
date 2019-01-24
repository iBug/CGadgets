#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <ctime>
using namespace std;

int len;

int main(int argc,char**argv){
	ostream* p_out;istream* p_in;
	ofstream fout;ifstream fin;
	if(argc==1 || (argc==2 && strcmp(argv[1],"-c")==0)){
		p_out = &cout;
		p_in = &cin;
	}else if(argc==2 && strcmp(argv[1],"-f")==0){
		fout.open("Sort.in");
		p_out = &fout;
		p_in = &cin;
	}else if(argc==3 && strcmp(argv[1],"-f")==0){
		fout.open(argv[2]);
		p_out = &fout;
		p_in = &cin;
	}
	
	ostream &_out = *p_out;
	istream &_in = *p_in;
	srand((unsigned)time(NULL));
	_in>>len;
	_out<<len<<endl;
	for(int i=0;i<len;i++)
		_out<<(rand()&(rand()<<15)&((rand()&1)<<30))<<(char)0x20;
	_out<<endl;
	return 0;
}
 
