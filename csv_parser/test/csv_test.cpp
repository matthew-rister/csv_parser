#include "catch.hpp"

#include "csv.hpp"

using namespace csv;

namespace {
	template <typename... ColumnTypes>
	std::string ToString(const Csv<ColumnTypes...>& csv) {
		std::ostringstream oss;
		oss << csv;
		return oss.str();
	}
}

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
}
