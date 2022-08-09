#include <iostream>
#include "random.h"

int main(int argc, char *argv[]) {
	Random RNG;

	for(int i=0; i<25; ++i) {
		std::cout << RNG.d6() << '\t' << RNG.d20() << '\t';
		std::cout << RNG.customInt(1, 16) << '\n';
	}

	return 0;
}
