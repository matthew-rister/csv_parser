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

	SECTION("Initializing a CSV with an empty input stream") {
		std::stringstream data;
		const Csv<> csv{data};
		REQUIRE(ToString(csv) == std::string{});
	}
}
