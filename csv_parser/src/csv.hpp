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

		template <typename TupleType, typename TupleElementType, std::int32_t CurrentIndex = std::tuple_size<TupleType>::value - 1>
		struct TupleElementAtIndex {
			static TupleElementType Get(const TupleType& tuple, const std::size_t index) {
				if (index == CurrentIndex) {
					using ActualTupleElementType = typename std::tuple_element<CurrentIndex, TupleType>::type;
					if constexpr (std::is_same<TupleElementType, ActualTupleElementType>::value) {
						return std::get<CurrentIndex>(tuple);
					} else {
						std::ostringstream oss;
						oss << "Tuple element type mismatch at index " << index;
						throw std::runtime_error{oss.str()};
					}
				}

				return TupleElementAtIndex<TupleType, TupleElementType, CurrentIndex - 1>::Get(tuple, index);
			}
		};

		template <typename TupleType, typename TupleElementType>
		struct TupleElementAtIndex<TupleType, TupleElementType, static_cast<std::int32_t>(-1)> {
			static TupleElementType Get(const TupleType&, const std::size_t) {
				throw std::runtime_error{"Tuple index out of bounds"};
			}
		};

	public:
		explicit Csv(const std::string& data) : elements_{ParseData(data)} {}

		template <typename ColumnType>
		[[nodiscard]] ColumnType Get(const std::size_t row_index, const std::size_t column_index) const {
			return TupleElementAtIndex<std::tuple<ColumnTypes...>, ColumnType>::Get(elements_[row_index], column_index);
		}

		[[nodiscard]] std::string ToString() const {
			std::ostringstream oss;

			if constexpr (sizeof...(ColumnTypes) > 0) {
				std::size_t index = 0;
				for (const auto& tuple : elements_) {
					std::apply([&](const auto& item, const auto&... items) {
						oss << item;
						((oss << ", " << items), ...);
					}, tuple);
					if (index++ < elements_.size() - 1) {
						oss << std::endl;
					}
				}
			}

			return oss.str();
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
