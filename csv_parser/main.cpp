#include <iostream>

#include "csv.hpp"

using namespace csv;

int main() {
	const std::string data{"1, a, 3.14\n2, b, 3.15\n3, c, 3.16"};
	const Csv<int, char, double> csv{data};
	std::cout << csv;

	return 0;
}