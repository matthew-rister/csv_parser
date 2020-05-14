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

		template <typename T> static T parse_token(const std::string& token) {
			T element;
			if constexpr (std::is_same<T, bool>::value) {
				std::istringstream{token} >> std::boolalpha >> element;
			} else {
				std::istringstream{token} >> element;
			}
			return element;
		}

		static std::vector<std::string> split(std::iostream& line_stream, const char delimiter) {
			std::vector<std::string> tokens;

			for (std::string token;
				std::getline(line_stream, token, delimiter);
				tokens.push_back(std::move(token))) {}

			return tokens;
		}

		static std::vector<std::string> split(const std::string& line, const char delimiter) {
			std::stringstream line_stream{line};
			return split(line_stream, delimiter);
		}
	};

	template <typename... ColumnTypes> class Csv final : public CsvBase {

		template <typename...> struct TypeList {};

		template <typename TupleType, typename TupleElementType, std::int32_t CurrentIndex = std::tuple_size<TupleType>::value - 1>
		struct TupleElementAtIndex {
			static const TupleElementType& get(const TupleType& tuple, const std::size_t index) {
				if (index == CurrentIndex) {
					using ActualTupleElementType = typename std::tuple_element<CurrentIndex, TupleType>::type;
					if constexpr (std::is_same<TupleElementType, ActualTupleElementType>::value) {
						return std::get<CurrentIndex>(tuple);
					} else {
						throw std::runtime_error{"Tuple element type mismatch"};
					}
				}
				return TupleElementAtIndex<TupleType, TupleElementType, CurrentIndex - 1>::get(tuple, index);
			}
		};

		template <typename TupleType, typename TupleElementType>
		struct TupleElementAtIndex<TupleType, TupleElementType, static_cast<std::int32_t>(-1)> {
			static const TupleElementType& get(const TupleType&, const std::size_t) {
				throw std::runtime_error{"Index out of bounds"};
			}
		};

	public:
		explicit Csv(const std::string& data) : elements_{parse_data(data)} {}

		template <typename ColumnType>
		[[nodiscard]] const ColumnType& get(const std::size_t row_index, const std::size_t column_index) const {
			if (row_index >= elements_.size()) {
				throw std::runtime_error{"Index out of bounds"};
			}
			return TupleElementAtIndex<std::tuple<ColumnTypes...>, ColumnType>::get(elements_[row_index], column_index);
		}

		[[nodiscard]] std::string to_string() const {
			std::ostringstream oss;
			constexpr auto index_sequence = std::make_index_sequence<sizeof...(ColumnTypes)>();
			for (std::size_t i = 0; i < elements_.size(); ++i) {
				to_string(oss, elements_[i], index_sequence);
				oss << (i < elements_.size() - 1 ? "\n" : "");
			}
			return oss.str();
		}

	private:
		static std::vector<std::tuple<ColumnTypes...>> parse_data(const std::string& data) {
			std::stringstream data_stream{data};
			const auto lines = split(data_stream, '\n');
			std::vector<std::tuple<ColumnTypes...>> elements;
			elements.reserve(lines.size());

			std::transform(std::cbegin(lines), std::cend(lines), std::back_inserter(elements),
				[](const auto& line) { return parse_line(line); });

			return elements;
		}

		static std::tuple<ColumnTypes...> parse_line(const std::string& line) {
			const auto tokens = split(line, ',');
			return parse_tokens(TypeList<ColumnTypes...>{}, tokens, 0);
		}

		template <typename ColumnType, typename... Rest>
		static std::tuple<ColumnType, Rest...> parse_tokens(
			const TypeList<ColumnType, Rest...>&, const std::vector<std::string>& tokens, const std::size_t index) {

			return std::tuple_cat(
				std::make_tuple(parse_token<ColumnType>(tokens[index])),
				parse_tokens(TypeList<Rest...>{}, tokens, index + 1));
		}

		static std::tuple<> parse_tokens(const TypeList<>&, const std::vector<std::string>&, const std::size_t) {
			return {};
		}

		template <typename TupleType, size_t... ColumnIndex>
		void to_string(std::ostream& os, const TupleType& tuple, std::index_sequence<ColumnIndex...>) const {
			((os << (ColumnIndex == 0 ? "" : ", ")
				<< (std::is_same<typename std::tuple_element<ColumnIndex, TupleType>::type, bool>::value ? std::boolalpha : std::noboolalpha)
				<< std::get<ColumnIndex>(tuple)), ...);
		}

		std::vector<std::tuple<ColumnTypes...>> elements_;
	};

	template <typename T> class Csv<T> final : public CsvBase {

	public:
		explicit Csv(const std::string& data) : elements_{parse_data(data)} {}

		[[nodiscard]] const T& get(const std::size_t row_index, const std::size_t column_index) const {
			if (row_index >= elements_.size() || column_index >= elements_[row_index].size()) {
				throw std::runtime_error{"Index out of bounds"};
			}
			return elements_[row_index][column_index];
		}

		[[nodiscard]] std::string to_string() const {
			std::ostringstream oss;
			for (std::size_t i = 0; i < elements_.size(); ++i) {
				for (std::size_t j = 0; j < elements_[i].size(); ++j) {
					oss << elements_[i][j] << (j < elements_[i].size() - 1 ? ", " : "");
				}
				oss << (i < elements_.size() - 1 ? "\n" : "");
			}
			return oss.str();
		}

	private:
		static std::vector<std::vector<T>> parse_data(const std::string& data) {
			std::stringstream data_stream{data};
			const auto lines = split(data_stream, '\n');
			std::vector<std::vector<T>> elements;
			elements.reserve(lines.size());

			std::transform(std::cbegin(lines), std::cend(lines), std::back_inserter(elements),
				[](const auto& line) { return parse_line(line); });

			return elements;
		}

		static std::vector<T> parse_line(const std::string& line) {
			const auto tokens = split(line, ',');
			std::vector<T> values;
			values.reserve(tokens.size());

			std::transform(std::cbegin(tokens), std::cend(tokens), std::back_inserter(values),
				[](const auto& token) { return parse_token<T>(token); });

			return values;
		}

		std::vector<std::vector<T>> elements_;
	};
}
