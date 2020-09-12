#define CATCH_CONFIG_MAIN

#include "catch.hpp"

#include "csv.hpp"

using namespace csv;

TEST_CASE("CSV parsing with homogeneous data", "[csv]") {

	SECTION("Parsing an empty string does not throw an exception") {
		const std::string data;
		REQUIRE_NOTHROW(Csv<int32_t>{data});
	}

	SECTION("Parsing a CSV with multiple rows and columns is correct") {
		const std::string data{"0, 1, 2\n3, 4, 5\n6, 7, 8"};
		const Csv<int32_t> csv{data};

		for (auto i = 0; i < 3; ++i) {
			for (auto j = 0; j < 3; ++j) {
				REQUIRE(csv.Get(i, j) == 3 * i + j);
			}
		}
	}

	SECTION("Error handling") {
		const std::string data{"0, 1, 2\n3, 4, 5\n6, 7, 8"};
		const Csv<int32_t> csv{data};

		SECTION("Attempting to access an element with an invalid row index throws an exception") {
			REQUIRE_THROWS(csv.Get(3, 0));
		}

		SECTION("Attempting to access an element with an invalid column index throws an exception") {
			REQUIRE_THROWS(csv.Get(0, 3));
		}
	}
}

TEST_CASE("CSV parsing with heterogeneous data") {

	SECTION("Parsing an empty string does not throw an exception") {
		const std::string data;
		REQUIRE_NOTHROW(Csv<>{data});
	}

	SECTION("Parsing a CSV with multiple rows and columns") {
		const std::string data{"a, 3.141, 42, true\nb, 2.718, 0, false\nc, 1.618, 7, true"};
		const Csv<char, double, int32_t, bool> csv{ data };

		SECTION("Character parsing is correct") {
			REQUIRE(csv.Get<char>(0, 0) == 'a');
			REQUIRE(csv.Get<char>(1, 0) == 'b');
			REQUIRE(csv.Get<char>(2, 0) == 'c');
		}

		SECTION("Double-precision floating-point parsing is correct") {
			REQUIRE(csv.Get<double>(0, 1) == Approx(3.141));
			REQUIRE(csv.Get<double>(1, 1) == Approx(2.718));
			REQUIRE(csv.Get<double>(2, 1) == Approx(1.618));
		}

		SECTION("Integer parsing is correct") {
			REQUIRE(csv.Get<int32_t>(0, 2) == 42);
			REQUIRE(csv.Get<int32_t>(1, 2) == 0);
			REQUIRE(csv.Get<int32_t>(2, 2) == 7);
		}

		SECTION("Boolean parsing is correct") {
			REQUIRE(csv.Get<bool>(0, 3) == true);
			REQUIRE(csv.Get<bool>(1, 3) == false);
			REQUIRE(csv.Get<bool>(2, 3) == true);
		}
	}

	SECTION("Parsing a CSV with numeric limits") {
		constexpr auto int64_t_max = std::numeric_limits<int64_t>::max();
		constexpr auto int64_t_min = std::numeric_limits<int64_t>::min();
		constexpr auto double_max = std::numeric_limits<double>::max();
		constexpr auto double_min = std::numeric_limits<double>::max();

		std::stringstream data;
		data << int64_t_max << ", " << double_max << '\n' << int64_t_min << ", " << double_min;
		const Csv<int64_t, double> csv{data.str()};

		SECTION("Parsing integer numeric limits is correct") {
			REQUIRE(csv.Get<int64_t>(0, 0) == int64_t_max);
			REQUIRE(csv.Get<int64_t>(1, 0) == int64_t_min);
		}

		SECTION("Parsing double-precision floating-point numeric limits is correct") {
			REQUIRE(csv.Get<double>(0, 1) == Approx{double_max});
			REQUIRE(csv.Get<double>(1, 1) == Approx{double_min});
		}
	}

	SECTION("Error handling") {
		const std::string data{"a, 3.141, 42, true\nb, 2.718, 0, false\nc, 1.618, 7, true"};
		const Csv<char, double, int32_t, bool> csv{data};

		SECTION("Attempting to access an element with an invalid template argument throws an exception") {
			REQUIRE_THROWS(csv.Get<double>(0, 0));
		}

		SECTION("Attempting to access an element with an invalid row index throws an exception") {
			REQUIRE_THROWS(csv.Get<char>(3, 0));
		}

		SECTION("Attempting to access an element with an invalid column index throws an exception") {
			REQUIRE_THROWS(csv.Get<double>(0, 4));
		}
	}
}
