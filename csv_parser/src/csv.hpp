#pragma once

#include <algorithm>
#include <execution>
#include <sstream>
#include <string>
#include <vector>

namespace csv {

	template <typename T> class Csv {

	public:
		explicit Csv(const std::string& data, const bool has_row_header = false, const bool has_column_header = false) {
			const auto lines = split(data, '\n');
			auto lines_begin = std::cbegin(lines);
			auto lines_size = lines.size();

			if (has_row_header) {
				const auto row_header = split(*lines_begin, ',');
				auto row_header_begin = std::cbegin(row_header);

				if (has_column_header) {
					++row_header_begin;
				}

				std::move(row_header_begin, std::cend(row_header), std::back_inserter(row_header_));
				++lines_begin;
				--lines_size;
			}

			entries_.reserve(lines_size);

			for (auto line_iterator = lines_begin; line_iterator != std::cend(lines); ++line_iterator) {
				const auto tokens = split(*line_iterator, ',');
				auto tokens_begin = std::cbegin(tokens);
				auto tokens_size = tokens.size();

				if (has_column_header) {
					column_header_.push_back(std::move(*tokens_begin));
					++tokens_begin;
					--tokens_size;
				}

				entries_.emplace_back();
				entries_.back().reserve(tokens_size);

				for (auto tokens_iterator = tokens_begin; tokens_iterator != std::cend(tokens); ++tokens_iterator) {
					const auto value = parse(*tokens_iterator);

					if constexpr (std::is_move_assignable<T>::value) {
						entries_.back().push_back(std::move(value));
					} else {
						entries_.back().push_back(value);
					}
				}
			}
		}

		friend std::ostream& operator<<(std::ostream& os, const Csv& csv) {
			if (!csv.row_header_.empty()) {
				if (!csv.column_header_.empty()) {
					os << " , ";
				}
				for (std::size_t i = 0; i < csv.row_header_.size(); ++i) {
					os << csv.row_header_[i] << (i < csv.row_header_.size() - 1 ? ", " : "");
				}
				os << '\n';
			}
			for (std::size_t i = 0; i < csv.entries_.size(); ++i) {
				if (!csv.column_header_.empty()) {
					os << csv.column_header_[i] << (!csv.entries_[i].empty() ? ", " : "");
				}
				for (std::size_t j = 0; j < csv.entries_[i].size(); ++j) {
					os << csv.entries_[i][j] << (j < csv.entries_[i].size() - 1 ? ", " : "");
				}
				os << '\n';
			}
			return os;
		}

	private:
		static std::vector<std::string> split(const std::string& line, const char delimiter) {
			std::vector<std::string> tokens;
			std::istringstream iss{line};

			for (std::string token; std::getline(iss, token, delimiter);) {
				tokens.push_back(trim(token));
			}

			return tokens;
		}

		static std::string trim(const std::string& s) {

			auto begin = s.begin();
			while (begin != s.end() && isspace(*begin)) ++begin;

			auto rbegin = s.rbegin();
			while (rbegin != s.rend() && isspace(*rbegin)) ++rbegin;

			return std::string{begin, rbegin.base()};
		}

		static T parse(const std::string& token) {
			T t;
			std::istringstream{token} >> t;
			return t;
		}

		std::vector<std::string> row_header_, column_header_;
		std::vector<std::vector<T>> entries_;
	};
}
