#pragma once

#include <algorithm>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace csv {

	class CsvBase {

	protected:
		CsvBase() = default;

		template <typename T> static T ParseToken(const std::string& token) {
			T element;
			if constexpr (std::is_same<T, bool>::value) {
				std::istringstream{token} >> std::boolalpha >> element;
			} else {
				std::istringstream{token} >> element;
			}
			return element;
		}

		static std::vector<std::string> Split(std::iostream& line_stream, const char delimiter) {
			std::vector<std::string> tokens;

			for (std::string token;
				std::getline(line_stream, token, delimiter);
				tokens.push_back(std::move(token))) {}

			return tokens;
		}

		static std::vector<std::string> Split(const std::string& line, const char delimiter) {
			std::stringstream line_stream{line};
			return Split(line_stream, delimiter);
		}
	};

	template <typename... ColumnTypes> class Csv final : public CsvBase {

		template <typename...> struct TypeList {};

		template <typename TupleType, typename TupleElementType, std::int32_t CurrentIndex = std::tuple_size<TupleType>::value - 1>
		struct TupleElementAtIndex {
			static const TupleElementType& Get(const TupleType& tuple, const std::size_t index) {
				if (index == CurrentIndex) {
					using ActualTupleElementType = typename std::tuple_element<CurrentIndex, TupleType>::type;
					if constexpr (std::is_same<TupleElementType, ActualTupleElementType>::value) {
						return std::get<CurrentIndex>(tuple);
					} else {
						throw std::runtime_error{"Tuple element type mismatch"};
					}
				}
				return TupleElementAtIndex<TupleType, TupleElementType, CurrentIndex - 1>::Get(tuple, index);
			}
		};

		template <typename TupleType, typename TupleElementType>
		struct TupleElementAtIndex<TupleType, TupleElementType, static_cast<std::int32_t>(-1)> {
			static const TupleElementType& Get(const TupleType&, const std::size_t) {
				throw std::runtime_error{"Index out of bounds"};
			}
		};

	public:
		explicit Csv(const std::string& data) : elements_{ParseData(data)} {}

		template <typename ColumnType>
		[[nodiscard]] const ColumnType& Get(const std::size_t row_index, const std::size_t column_index) const {
			if (row_index >= elements_.size()) {
				throw std::runtime_error{"Index out of bounds"};
			}
			return TupleElementAtIndex<std::tuple<ColumnTypes...>, ColumnType>::Get(elements_[row_index], column_index);
		}

	private:
		static std::vector<std::tuple<ColumnTypes...>> ParseData(const std::string& data) {
			std::stringstream data_stream{data};
			const auto lines = Split(data_stream, '\n');
			std::vector<std::tuple<ColumnTypes...>> elements;
			elements.reserve(lines.size());

			std::transform(std::cbegin(lines), std::cend(lines), std::back_inserter(elements),
				[](const auto& line) { return ParseLine(line); });

			return elements;
		}

		static std::tuple<ColumnTypes...> ParseLine(const std::string& line) {
			const auto tokens = Split(line, ',');
			return ParseTokens(TypeList<ColumnTypes...>{}, tokens, 0);
		}

		template <typename ColumnType, typename... Rest>
		static std::tuple<ColumnType, Rest...> ParseTokens(
			const TypeList<ColumnType, Rest...>&, const std::vector<std::string>& tokens, const std::size_t index) {

			return std::tuple_cat(
				std::make_tuple(ParseToken<ColumnType>(tokens[index])),
				ParseTokens(TypeList<Rest...>{}, tokens, index + 1));
		}

		static std::tuple<> ParseTokens(const TypeList<>&, const std::vector<std::string>&, const std::size_t) {
			return {};
		}

		std::vector<std::tuple<ColumnTypes...>> elements_;
	};

	template <typename T> class Csv<T> final : public CsvBase {

	public:
		explicit Csv(const std::string& data) : elements_{ParseData(data)} {}

		[[nodiscard]] const T& Get(const std::size_t row_index, const std::size_t column_index) const {
			if (row_index >= elements_.size() || column_index >= elements_[row_index].size()) {
				throw std::runtime_error{"Index out of bounds"};
			}
			return elements_[row_index][column_index];
		}

	private:
		static std::vector<std::vector<T>> ParseData(const std::string& data) {
			std::stringstream data_stream{data};
			const auto lines = Split(data_stream, '\n');
			std::vector<std::vector<T>> elements;
			elements.reserve(lines.size());

			std::transform(std::cbegin(lines), std::cend(lines), std::back_inserter(elements),
				[](const auto& line) { return ParseLine(line); });

			return elements;
		}

		static std::vector<T> ParseLine(const std::string& line) {
			const auto tokens = Split(line, ',');
			std::vector<T> values;
			values.reserve(tokens.size());

			std::transform(std::cbegin(tokens), std::cend(tokens), std::back_inserter(values),
				[](const auto& token) { return ParseToken<T>(token); });

			return values;
		}

		std::vector<std::vector<T>> elements_;
	};
}
