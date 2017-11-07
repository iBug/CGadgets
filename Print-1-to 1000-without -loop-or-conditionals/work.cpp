#include <iostream>

template <int N>
class NumberPrinter{
	public:
	NumberPrinter(){
		std::cout << N << std::endl;
	}
};

template <int N>
class NumberGenerator : public NumberGenerator<N-1>, public NumberPrinter<N> {
};

template <>
class NumberGenerator<1> : public NumberPrinter<1> {
};

int main(){
  // You may need to supply option `-ftemplate-depth=1001` to g++
  // so this program cam compile correctly
	NumberGenerator<1000> a;
	return 0;
}
