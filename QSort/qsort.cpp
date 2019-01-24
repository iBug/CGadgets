#include <iostream>
#include <fstream>
#include <cstring>
using namespace std;

int n,*d,tmp;
ifstream fin;
ofstream fout;
void Sort(int,int);

int main(int argc,char* argv[]){
	ostream* p_out;
	istream* p_in;
	if(argc==1 || (argc==2 && strcmp(argv[1],"c"))){
		p_out = &cout;p_in = &cin;
	}
	else if(argc==2 && strcmp(argv[1],"f")){
		fin.open("QSort.in");fout.open("QSort.out");
	}
	else if(argc==3 && strcmp(argv[1],"f")){
		fin.open(argv[2]);fout.open("QSort.out");
	}
	else if(argc==4 && strcmp(argv[1],"f")){
		fin.open(argv[2]);fout.open(argv[3]);
	}else{
		cerr<<"Command error!"<<endl;
		return 1;
	}
	if(argc>1 && strcmp(argv[1],"f")){p_out = &fout;p_in = &fin;}
	
	ostream& _out = *p_out;
	istream& _in = *p_in;
	
	_in>>n;
	d = new int[n];
	for(int i=0;i<n;i++)
		_in>>d[i];
	Sort(0,n-1);
	for(int i=0;i<n;i++)
		_out<<d[i]<<" ";
	_out<<std::endl;
	delete[] d;
	return 0;
}

void Sort(int a,int b){
	if(a>=b)return;
	int x = a,y = b,f = 0;
	while(x<y){
		if(d[x]>d[y]){
			tmp = d[x];
			d[x] = d[y];
			d[y] = tmp;
			f ^= 1;
		}
		f == 0 ? y-- : x++ ;
	}
	Sort(a,--x);
	Sort(++y,b);
}

