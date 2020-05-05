#pragma once

#include <algorithm>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace csv {

	class IndexOutOfBoundsException final : public std::exception {

	public:
		explicit IndexOutOfBoundsException(const std::size_t index)
			: index_{index} {}

		char const* what() const override {
			std::stringstream oss;
			oss << "Index out of bounds: " << index_;
			const auto what = oss.str();
			return what.c_str();
		}

	private:
		size_t index_;
	};

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
			static const TupleElementType& get(const TupleType&, const std::size_t index) {
				throw IndexOutOfBoundsException{index};
			}
		};

	public:
		explicit Csv(std::iostream& data) : elements_{parse_data(data)} {}

		template <typename ColumnType>
		[[nodiscard]] const ColumnType& get(const std::size_t row_index, const std::size_t column_index) const {
			if (row_index >= elements_.size()) {
				throw IndexOutOfBoundsException{row_index};
			}
			return TupleElementAtIndex<std::tuple<ColumnTypes...>, ColumnType>::get(elements_[row_index], column_index);
		}

		[[nodiscard]] std::string to_string() const {
			std::ostringstream oss;
			if constexpr (sizeof...(ColumnTypes) > 0) {
				for (std::size_t i = 0; i < elements_.size(); ++i) {
					std::apply([&](const auto& item, const auto&... items) {
						oss << item;
						((oss << ", " << items), ...);
					}, elements_[i]);
					oss << (i < elements_.size() - 1 ? "\n" : "");
				}
			}
			return oss.str();
		}

	private:
		static std::vector<std::tuple<ColumnTypes...>> parse_data(std::iostream& data) {
			const auto lines = split(data, '\n');
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

		std::vector<std::tuple<ColumnTypes...>> elements_;
	};

	template <typename T> class Csv<T> final : public CsvBase {

	public:
		explicit Csv(std::iostream& data) : elements_{parse_data(data)} {}

		[[nodiscard]] const T& get(const std::size_t row_index, const std::size_t column_index) const {
			if (row_index >= elements_.size()) {
				throw IndexOutOfBoundsException{row_index};
			}
			if (column_index >= elements_[row_index].size()) {
				throw IndexOutOfBoundsException{column_index};
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
		static std::vector<std::vector<T>> parse_data(std::iostream& data) {
			const auto lines = split(data, '\n');
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
