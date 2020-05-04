#include "catch.hpp"

#include "csv.hpp"

using namespace csv;

TEST_CASE("CSV Initialization", "[csv]") {

	SECTION("Initializing a CSV with an empty input stream does not throw an exception") {
		std::stringstream data;
		REQUIRE_NOTHROW(Csv<>{data});
	}

	SECTION("Initializing a CSV with homogeneous data") {
		std::stringstream data;
		data << "0, 1, 2" << std::endl
			<< "3, 4, 5" << std::endl
			<< "6, 7, 8";

		const Csv<int, int, int> csv{data};

		SECTION("Accessing CSV elements returns the correct element") {
			for (int i = 0; i < 3; ++i) {
				for (int j = 0; j < 3; ++j) {
					REQUIRE(csv.Get<int>(i, j) == 3 * i + j);
				}
			}
		}
	}

	SECTION("Initializing a CSV with heterogeneous data") {
		std::stringstream data;
		data << "a, 3.141, 42, true" << std::endl
			<< "b, 2.718, 0, false" << std::endl
			<< "c, 1.618, 7, true";

		const Csv<char, double, int, bool> csv{data};

		SECTION("Accessing CSV elements returns the correct element") {

			SECTION("Accessing character elements is correct") {
				REQUIRE(csv.Get<char>(0, 0) == 'a');
				REQUIRE(csv.Get<char>(1, 0) == 'b');
				REQUIRE(csv.Get<char>(2, 0) == 'c');
			}

			SECTION("Accessing double floating point elements is correct") {
				REQUIRE(csv.Get<double>(0, 1) == 3.141);
				REQUIRE(csv.Get<double>(1, 1) == 2.718);
				REQUIRE(csv.Get<double>(2, 1) == 1.618);
			}

			SECTION("Accessing integer elements is correct") {
				REQUIRE(csv.Get<int>(0, 2) == 42);
				REQUIRE(csv.Get<int>(1, 2) == 0);
				REQUIRE(csv.Get<int>(2, 2) == 7);
			}

			SECTION("Accessing boolean elements is correct") {
				REQUIRE(csv.Get<bool>(0, 3) == true);
				REQUIRE(csv.Get<bool>(1, 3) == false);
				REQUIRE(csv.Get<bool>(2, 3) == true);
			}
		}
	}
}
