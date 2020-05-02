#pragma once

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

namespace csv {

	template <typename... ColumnTypes> class Csv {

	public:
		explicit Csv(const std::string& data) : entries_{parse_data(data)} {}

		friend std::ostream& operator<<(std::ostream& os, const Csv& csv) {

			if constexpr (sizeof...(ColumnTypes) > 0) {
				for (const auto& row : csv.entries_) {
					std::apply(
						[&os](const auto& item, const auto&... items) {
							os << item;
							((os << ", " << items), ...);
						}, row);
					os << std::endl;
				}
			}

			return os;
		}

	private:
		template <typename...> struct TypeList {};

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
			return parse_tokens(TypeList<ColumnTypes...>{}, tokens, 0);
		}

		template <typename ColumnType, typename... Rest> static std::tuple<ColumnType, Rest...> parse_tokens(
			const TypeList<ColumnType, Rest...>&, const std::vector<std::string>& tokens, const std::size_t index) {

			return std::tuple_cat(
				parse_token<ColumnType>(tokens[index]),
				parse_tokens(TypeList<Rest...>{}, tokens, index + 1));
		}

		static std::tuple<> parse_tokens(const TypeList<>&, const std::vector<std::string>&, const std::size_t) {
			return {};
		}

		template <typename T> static std::tuple<T> parse_token(const std::string& token) {
			T t;
			std::istringstream{token} >> t;
			return std::make_tuple(t);
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
