#pragma once

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>


namespace csv {

	template <typename... ColumnTypes> class Csv {

	public:
		explicit Csv(const std::string& data) : entries_{parse_data(data)} {}

		template <typename ColumnType> ColumnType get(const std::size_t i, const std::size_t j) {
			return get2<ColumnType>(entries_[i], j);
		}

		friend std::ostream& operator<<(std::ostream& os, const Csv& csv) {
			if constexpr (sizeof...(ColumnTypes) > 0) {
				std::for_each(std::cbegin(csv.entries_), std::cend(csv.entries_), [&](const auto& tuple) {
					std::apply([&](const auto& item, const auto&... items) {
						os << item;
						((os << ", " << items), ...);
					}, tuple);
					os << std::endl;
				});
			}

			return os;
		}

	private:
		static std::vector<std::tuple<ColumnTypes...>> parse_data(const std::string& data) {
			const auto lines = split(data, '\n');
			std::vector<std::tuple<ColumnTypes...>> entries;
			entries.reserve(lines.size());

			std::transform(std::cbegin(lines), std::cend(lines), std::back_inserter(entries),
				[](const auto& line) { return parse_line(line); });

			return entries;
		}

		template <typename...> struct TypeList {};

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

		template <typename ColumnType, typename TupleType, std::size_t TupleSize> struct Index {
			static ColumnType get(const TupleType& tuple, const std::size_t index) {
				if (index == TupleSize - 1) {
					if constexpr (std::is_same<ColumnType, typename std::tuple_element<TupleSize - 1, TupleType>::type>::value) {
						return std::get<TupleSize - 1>(tuple);
					}
					else {
						throw std::runtime_error{"Type"};
					}
				}

				return Index<ColumnType, TupleType, TupleSize - 1>::get(tuple, index);
			}
		};

		template <typename ColumnType, typename TupleType> struct Index<ColumnType, TupleType, 0> {
			static ColumnType get(const TupleType&, const std::size_t) {
				throw std::runtime_error{"Index out of bounds"};
			}
		};

		template <typename ColumnType, class Tuple> static ColumnType get2(const Tuple& tuple, const std::size_t i) {
			return Index<ColumnType, Tuple, std::tuple_size<Tuple>::value>::get(tuple, i);
		}

		std::vector<std::tuple<ColumnTypes...>> entries_;
	};
}
