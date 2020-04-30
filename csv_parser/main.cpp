//#define CATCH_CONFIG_MAIN
//
//#include "catch.hpp"

#include <iostream>
#include <sstream>

#include "csv.hpp"

using namespace csv;

int main() {

	try {
		const std::string data{"1, 2, 3\n4, 5, 6\n7, 8, 9"};
		const Csv<uint8_t> csv{data};
		std::cout << csv << std::endl;
	} catch(...) {
		return EXIT_FAILURE;
	}

	return 0;
}