#pragma once

#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace csv {

	template <typename... ColumnTypes> class Csv {

		template <typename...> struct TypeList {};

		template <typename TupleType, typename TupleElementType, int32_t TupleIndex> struct Index {
			static TupleElementType get(const TupleType& tuple, const std::size_t tuple_index) {
				if (tuple_index == TupleIndex) {
					if constexpr (std::is_same<TupleElementType, typename std::tuple_element<TupleIndex, TupleType>::type>::value) {
						return std::get<TupleIndex>(tuple);
					}
					else {
						throw std::runtime_error{"Tuple element type mismatch"};
					}
				}

				return Index<TupleType, TupleElementType, TupleIndex>::get(tuple, tuple_index);
			}
		};

		template <typename TupleType, typename TupleElementType> struct Index<TupleType, TupleElementType, -1> {
			static TupleElementType get(const TupleType&, const std::size_t) {
				throw std::runtime_error{"Index out of bounds"};
			}
		};

	public:
		explicit Csv(const std::string& data) : entries_{parse_data(data)} {}

		template <typename TupleElementType>
		TupleElementType get(const std::size_t row_index, const std::size_t column_index) {
			constexpr int32_t tuple_size = std::tuple_size<std::tuple<ColumnTypes...>>::value;
			return Index<std::tuple<ColumnTypes...>, TupleElementType, tuple_size - 1>::get(entries_[row_index], column_index);
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
