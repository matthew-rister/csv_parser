#pragma once

#include <algorithm>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace csv {

	template <typename... ColumnTypes> class Csv {

		template <typename...>
		struct TypeList {};

		template <typename TupleType, typename TupleElementType, int32_t TupleIndex>
		struct Index {
			static TupleElementType Get(const TupleType& tuple, const std::size_t tuple_index) {
				if (tuple_index == TupleIndex) {
					if constexpr (std::is_same<TupleElementType, typename std::tuple_element<TupleIndex, TupleType>::type>::value) {
						return std::get<TupleIndex>(tuple);
					} else {
						throw std::runtime_error{"Tuple element type mismatch"};
					}
				}

				return Index<TupleType, TupleElementType, TupleIndex>::Get(tuple, tuple_index);
			}
		};

		template <typename TupleType, typename TupleElementType>
		struct Index<TupleType, TupleElementType, -1> {
			static TupleElementType Get(const TupleType&, const std::size_t) {
				throw std::runtime_error{"Index out of bounds"};
			}
		};

	public:
		explicit Csv(const std::string& data) : elements_{ParseData(data)} {}

		template <typename TupleElementType>
		TupleElementType Get(const std::size_t row_index, const std::size_t column_index) {
			constexpr int32_t tuple_size = std::tuple_size<std::tuple<ColumnTypes...>>::value;
			return Index<std::tuple<ColumnTypes...>, TupleElementType, tuple_size - 1>::Get(elements_[row_index], column_index);
		}

		friend std::ostream& operator<<(std::ostream& os, const Csv& csv) {
			if constexpr (sizeof...(ColumnTypes) > 0) {
				std::for_each(std::cbegin(csv.elements_), std::cend(csv.elements_), [&](const auto& tuple) {
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
		static std::vector<std::tuple<ColumnTypes...>> ParseData(const std::string& data) {
			const auto lines = Split(data, '\n');
			std::vector<std::tuple<ColumnTypes...>> entries;
			entries.reserve(lines.size());

			std::transform(std::cbegin(lines), std::cend(lines), std::back_inserter(entries),
				[](const auto& line) { return ParseLine(line); });

			return entries;
		}

		static std::tuple<ColumnTypes...> ParseLine(const std::string& line) {
			const auto tokens = Split(line, ',');
			return ParseTokens(TypeList<ColumnTypes...>{}, tokens);
		}

		template <typename ColumnType, typename... Rest>
		static std::tuple<ColumnType, Rest...> ParseTokens(
			const TypeList<ColumnType, Rest...>&, const std::vector<std::string>& tokens, const std::size_t index = 0) {

			return std::tuple_cat(
				ParseToken<ColumnType>(tokens[index]),
				ParseTokens(TypeList<Rest...>{}, tokens, index + 1));
		}

		static std::tuple<> ParseTokens(const TypeList<>&, const std::vector<std::string>&, const std::size_t) {
			return {};
		}

		template <typename ColumnType>
		static std::tuple<ColumnType> ParseToken(const std::string& token) {
			ColumnType element;
			std::istringstream{token} >> element;
			return std::make_tuple(element);
		}

		static std::vector<std::string> Split(const std::string& line, const char delimiter) {
			std::vector<std::string> tokens;
			std::istringstream iss{line};

			for (std::string token;
			     std::getline(iss, token, delimiter);
			     tokens.push_back(std::move(token))) {}

			return tokens;
		}

		std::vector<std::tuple<ColumnTypes...>> elements_;
	};
}
