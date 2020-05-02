#pragma once

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

namespace csv {

	template <typename...> struct TypeList {};

	template <typename T> std::tuple<T> parse(const std::vector<std::string>& v, const std::size_t i) {
		T t;
		std::istringstream{v[i]} >> t;
		return std::make_tuple(t);
	}

	std::tuple<> parse(TypeList<>, const std::vector<std::string>&, const std::size_t) {
		return {};
	}

	template<typename T, typename... Rest>
	std::tuple<T, Rest...> parse(TypeList<T, Rest...>, const std::vector<std::string>& v, const std::size_t i) {
		return std::tuple_cat(
			parse<T>(v, i),
			parse(TypeList<Rest...>{}, v, i + 1));
	}

	template<typename... Types> std::tuple<Types...> parse(const std::vector<std::string>& v) {
		return parse(TypeList<Types...>{}, v, 0);
	}

	template <typename... Types> void print(std::ostream& os, const std::tuple<Types...>& t) {
		if constexpr (sizeof...(Types) > 0) {
			std::apply(
				[&os](const auto& item, const auto&... items) {
					os << item;
					((os << ", " << items), ...);
				}, t);
		}
	}

	template <typename... ColumnTypes> class Csv {

	public:
		explicit Csv(const std::string& data) : entries_{parse_data(data)} {}

		friend std::ostream& operator<<(std::ostream& os, const Csv& csv) {

			for (const auto& row : csv.entries_) {
				print(os, row);
				os << std::endl;
			}

			return os;
		}

	private:
		static std::vector<std::tuple<ColumnTypes...>> parse_data(const std::string& data) {
			const auto lines = split(data, '\n');
			std::vector<std::tuple<ColumnTypes...>> entries;
			entries.reserve(lines.size());

			std::transform(std::cbegin(lines), std::cend(lines), std::back_inserter(entries), [](const auto& line) {
				return parse_line(line);
			});

			return entries;
		}

		static std::tuple<ColumnTypes...> parse_line(const std::string& line) {
			const auto tokens = split(line, ',');
			return parse<ColumnTypes...>(tokens);
		}

		static std::vector<std::string> split(const std::string& line, const char delimiter) {
			std::vector<std::string> tokens;
			std::istringstream iss{line};

			for (std::string token;
				std::getline(iss, token, delimiter);
				tokens.push_back(std::move(token))) {
			}

			return tokens;
		}

		std::vector<std::tuple<ColumnTypes...>> entries_;
	};
}
