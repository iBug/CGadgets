#include <windows.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <vector>

const char * const Version = "1.0"; 

typedef double Frequency;

class Key;
extern const Key Key_SILENT;
class Key {
	public:
	enum {C = 0, bD, D, bE, E, F, bG, G, bA, A, bB, B};
	typedef signed short Pitch;
	typedef signed short Octave;
	typedef signed long Absolute;
	
	Pitch p;
	Octave o;
	
	Key(void) : Key(Key_SILENT) {};
	Key(const Pitch& _p, const Octave& _o) : p(_p), o(_o) {}
	Key(const Key&) = default;
	~Key(void) = default;
	Absolute abs(void) const{
		return (this->o * 12) + this->p;
	}
	Key operator+(Absolute i) const{
		Absolute abs = this->abs() + i;
		return Key(Pitch(abs % 12), Octave(abs / 12));
	}
	Key operator-(Absolute i) const{return (*this) + (-i);}
	Absolute operator-(const Key& k) const{
		return this->abs() - k.abs();
	}
	bool operator==(const Key& k) const{return this->abs() == k.abs();}
	// operator Absolute(void) const{return this->abs();}
};

inline Key operator+(signed i, const Key& k){return k + i;}
const Key Key_SILENT(0, 0);

class Note {
	public:
	typedef unsigned long Duration;
	Key k;
	Duration d;
};

class Sheet {
	public:
	typedef double Bpm;
	typedef unsigned long Step;
	typedef typename Key::Absolute Offset;
	Bpm bpm;
	Offset off;
	std::vector<Note> n;
	friend void Play(const Sheet&);
};


void Play(const Sheet&);
void PlayNote(const Note&);
void PlayKey(const Key&, const Note::Duration&);

void Play(const Sheet& s){
	for (const auto& x : s.n){
		double dur = double(x.d) * 60.0 / (s.bpm * 16.0) * 1000.0;
		// std::cerr << x.k.p << " " << x.k.o << " " << dur << std::endl;
		PlayKey(x.k + s.off, Note::Duration(dur));
	}
}

void PlayNote(const Note& n){
	PlayKey(n.k, n.d); 
}

void PlayKey(const Key& k, const Note::Duration& d){
	static const Frequency Freq_Ref(261.60);
	static const Key Key_Ref(Key::C, 4); // Reference: C4 = 261.6 Hz
	if (k == Key_SILENT){
		Sleep(DWORD(d));
		return;
	}
	// std::cerr << k.p << " " << k.o << " " << std::endl;
	Key::Absolute diff = k - Key_Ref;
	DWORD dFreq = DWORD(round(Freq_Ref * pow(2.0, (1.0/12.0) * diff)));
	Beep(dFreq, d-2);
	Sleep(1);
}

void ReadSheet(std::istream& is, Sheet& s){
	is >> s.bpm >> s.off;
	is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	Note n;
	std::string str;
	std::istringstream istr;
	while (!is.eof()){
		getline(is, str);
		if (str.size() == 0 || str[0] == '#'){
			//std::cerr << "Skipped line: " << str << std::endl;
			continue;
		}
		istr.str(str.c_str());
		istr.seekg(0);
		istr >> n.k.p >> n.k.o >> n.d;
		s.n.push_back(n);
	}
}

void PrintUsage(std::ostream& o){
	o << "Usage: BeepPlay.exe <File>" << std::endl << std::endl;
	o << "\"play.txt\" is automatically selected." << std::endl;
}

int main(int argc, char **argv){
	std::vector<std::string> args(argv, argv + argc);
	if (args.size() <= 1){
		PrintUsage(std::cerr);
		args.push_back("play.txt");
		//return 1;
	}
	Sheet sheet;
	std::ifstream fin(args[1]);
	ReadSheet(fin, sheet);
	fin.close(); 
	Play(sheet);
	return 0;
}

