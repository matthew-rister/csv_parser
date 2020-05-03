#include "catch.hpp"

#include "csv.hpp"

using namespace csv;

TEST_CASE("Output streamming", "[csv]") {

	const std::string data{"a, 3.14, 1, 1.12\nb, 3.15, 2, 1.13\n, 3.16, 3, 1.14"};
	Csv<char, double, int, double> csv{data};

	REQUIRE(csv.Get<double>(1, 3) == 1.13);
}