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

		const Csv<int32_t> csv{data};

		SECTION("Accessing CSV elements returns the correct element") {
			for (int32_t i = 0; i < 3; ++i) {
				for (int32_t j = 0; j < 3; ++j) {
					REQUIRE(csv.Get(i, j) == 3 * i + j);
				}
			}
		}
	}

	SECTION("Initializing a CSV with heterogeneous data") {
		std::stringstream data;
		data << "a, 3.141, 42, true" << std::endl
			<< "b, 2.718, 0, false" << std::endl
			<< "c, 1.618, 7, true";

		const Csv<char, double, int32_t, bool> csv{data};

		SECTION("Accessing CSV elements returns the correct element") {

			SECTION("Accessing character elements is correct") {
				REQUIRE(csv.Get<char>(0, 0) == 'a');
				REQUIRE(csv.Get<char>(1, 0) == 'b');
				REQUIRE(csv.Get<char>(2, 0) == 'c');
			}

			SECTION("Accessing double floating point elements is correct") {
				REQUIRE(csv.Get<double>(0, 1) == Approx(3.141));
				REQUIRE(csv.Get<double>(1, 1) == Approx(2.718));
				REQUIRE(csv.Get<double>(2, 1) == Approx(1.618));
			}

			SECTION("Accessing integer elements is correct") {
				REQUIRE(csv.Get<int32_t>(0, 2) == 42);
				REQUIRE(csv.Get<int32_t>(1, 2) == 0);
				REQUIRE(csv.Get<int32_t>(2, 2) == 7);
			}

			SECTION("Accessing boolean elements is correct") {
				REQUIRE(csv.Get<bool>(0, 3) == true);
				REQUIRE(csv.Get<bool>(1, 3) == false);
				REQUIRE(csv.Get<bool>(2, 3) == true);
			}
		}
	}

	SECTION("Initializing a CSV with numeric limits") {
		constexpr auto int64_t_max = std::numeric_limits<int64_t>::max();
		constexpr auto int64_t_min = std::numeric_limits<int64_t>::min();
		constexpr auto double_max = std::numeric_limits<double>::max();
		constexpr auto double_min = std::numeric_limits<double>::max();

		std::stringstream data;
		data << int64_t_max << ", " << double_max << std::endl
			<< int64_t_min << ", " << double_min;

		const Csv<int64_t, double> csv{data};

		SECTION("Parsing integer numeric limits is correct") {
			REQUIRE(csv.Get<int64_t>(0, 0) == int64_t_max);
			REQUIRE(csv.Get<int64_t>(1, 0) == int64_t_min);
		}

		SECTION("Parsing double numeric limits is correct") {
			REQUIRE(csv.Get<double>(0, 1) == Approx{double_max});
			REQUIRE(csv.Get<double>(1, 1) == Approx{double_min});
		}
	}
}
