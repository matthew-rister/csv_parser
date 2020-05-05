#include "catch.hpp"

#include "csv.hpp"

using namespace csv;

TEST_CASE("CSV parsing with homogeneous data", "[csv]") {

	SECTION("Parsing an empty input stream does not throw an exception") {
		std::stringstream data;
		REQUIRE_NOTHROW(Csv<int32_t>{data});
	}

	SECTION("Parsing an input sream with multiple rows and columns is correct") {
		const std::string input{"0, 1, 2\n3, 4, 5\n6, 7, 8"};
		std::stringstream input_stream{input};
		const Csv<int32_t> csv{input_stream};

		for (auto i = 0; i < 3; ++i) {
			for (auto j = 0; j < 3; ++j) {
				REQUIRE(csv.get(i, j) == 3 * i + j);
			}
		}
	}

	SECTION("Writing a CSV to an output stream is identical to its input") {
		const std::string input{"0, 1, 2\n3, 4, 5\n6, 7, 8"};
		std::stringstream input_stream{input};
		const Csv<int> csv{input_stream};
		REQUIRE(csv.to_string() == input);
	}
}

TEST_CASE("CSV parsing with heterogeneous data") {

	SECTION("Parsing an empty input stream does not throw an exception") {
		std::stringstream data;
		REQUIRE_NOTHROW(Csv<>{data});
	}

	SECTION("Parsing an input sream with multiple rows and columns") {
		const std::string input{"a, 3.141, 42, true\nb, 2.718, 0, false\nc, 1.618, 7, true"};
		std::stringstream input_stream{input};
		const Csv<char, double, int32_t, bool> csv{input_stream};

		SECTION("Character parsing is correct") {
			REQUIRE(csv.get<char>(0, 0) == 'a');
			REQUIRE(csv.get<char>(1, 0) == 'b');
			REQUIRE(csv.get<char>(2, 0) == 'c');
		}

		SECTION("Double-precision floating-point parsing is correct") {
			REQUIRE(csv.get<double>(0, 1) == Approx(3.141));
			REQUIRE(csv.get<double>(1, 1) == Approx(2.718));
			REQUIRE(csv.get<double>(2, 1) == Approx(1.618));
		}

		SECTION("Integer parsing is correct") {
			REQUIRE(csv.get<int32_t>(0, 2) == 42);
			REQUIRE(csv.get<int32_t>(1, 2) == 0);
			REQUIRE(csv.get<int32_t>(2, 2) == 7);
		}

		SECTION("Boolean parsing is correct") {
			REQUIRE(csv.get<bool>(0, 3) == true);
			REQUIRE(csv.get<bool>(1, 3) == false);
			REQUIRE(csv.get<bool>(2, 3) == true);
		}
	}

	SECTION("Parsing an input stream with numeric limits") {
		constexpr auto int64_t_max = std::numeric_limits<int64_t>::max();
		constexpr auto int64_t_min = std::numeric_limits<int64_t>::min();
		constexpr auto double_max = std::numeric_limits<double>::max();
		constexpr auto double_min = std::numeric_limits<double>::max();

		std::stringstream input_stream;
		input_stream << int64_t_max << ", " << double_max << '\n' << int64_t_min << ", " << double_min;
		const Csv<int64_t, double> csv{input_stream};

		SECTION("Parsing integer numeric limits is correct") {
			REQUIRE(csv.get<int64_t>(0, 0) == int64_t_max);
			REQUIRE(csv.get<int64_t>(1, 0) == int64_t_min);
		}

		SECTION("Parsing double-precision floating-point numeric limits is correct") {
			REQUIRE(csv.get<double>(0, 1) == Approx{ double_max });
			REQUIRE(csv.get<double>(1, 1) == Approx{ double_min });
		}
	}

	SECTION("Error handling") {
		std::stringstream input_stream;
		input_stream << "a, 1, 3.14\nb, 2, 2,71\nc, 3, 1.618";
		const Csv<char, int, double> csv{input_stream};

		SECTION("Attempting to access an element with an invalid template argument throws an exception") {
			REQUIRE_THROWS(csv.get<double>(0, 0));
		}

		SECTION("Attempting to access an element with an invalid row index throws an exception") {
			REQUIRE_THROWS_AS(csv.get<char>(3, 0), IndexOutOfBoundsException);
		}

		SECTION("Attempting to access an element with an invalid column index throws an exception") {
			REQUIRE_THROWS_AS(csv.get<double>(0, 3), IndexOutOfBoundsException);
		}
	}

	SECTION("Writing a CSV to an output stream is identical to its input") {
		const std::string input{"a, 3.141, 42, true\nb, 2.718, 0, false\nc, 1.618, 7, true"};
		std::stringstream input_stream{input};
		const Csv<char, double, int32_t, bool> csv{input_stream};

		// TODO: fix output stream conversion for booleans
		REQUIRE(csv.to_string() == std::string{ "a, 3.141, 42, 1\nb, 2.718, 0, 0\nc, 1.618, 7, 1" });
	}
}
