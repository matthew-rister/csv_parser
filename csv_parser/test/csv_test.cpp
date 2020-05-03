#include "catch.hpp"

#include "csv.hpp"

#include <iostream>

using namespace csv;

TEST_CASE("CSV parsing", "[csv]") {

	const std::string data{"a, 3.14, 42, true\nb, 2.72, 0, false"};
	const Csv<char, double, int, bool> csv{data};

	SECTION("The first row is correct") {

		SECTION("The first element is correct") {
			REQUIRE(csv.Get<char>(0, 0) == 'a');
		}

		SECTION("The second element is correct") {
			REQUIRE(csv.Get<double>(0, 1) == 3.14);
		}

		SECTION("The third element is correct") {
			REQUIRE(csv.Get<int>(0, 2) == 42);
		}

		SECTION("The fourth element is correct") {
			REQUIRE(csv.Get<bool>(0, 3) == true);
		}
	}

	SECTION("The second row is correct") {

		SECTION("The first element is correct") {
			REQUIRE(csv.Get<char>(1, 0) == 'b');
		}

		SECTION("The second element is correct") {
			REQUIRE(csv.Get<double>(1, 1) == 2.72);
		}

		SECTION("The third element is correct") {
			REQUIRE(csv.Get<int>(1, 2) == 0);
		}

		SECTION("The fourth element is correct") {
			REQUIRE(csv.Get<bool>(1, 3) == false);
		}
	}
}
