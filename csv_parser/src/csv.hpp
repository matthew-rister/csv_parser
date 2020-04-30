#pragma once

#include <algorithm>
#include <execution>
#include <sstream>
#include <string>
#include <vector>

namespace csv {

	template <typename T> class Csv {

	public:
		explicit Csv(const std::string& data) {
			const auto lines = split(data, '\n');
			entries_.reserve(lines.size());

			for (auto line_iterator = std::cbegin(lines); line_iterator != std::cend(lines); ++line_iterator) {
				const auto tokens = split(*line_iterator, ',');
				entries_.emplace_back();
				entries_.back().reserve(tokens.size());

				for (auto tokens_iterator = std::cbegin(tokens); tokens_iterator != std::cend(tokens); ++tokens_iterator) {
					const auto value = parse_token(*tokens_iterator);

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
				for (std::size_t i = 0; i < csv.row_header_.size(); ++i) {
					os << csv.row_header_[i];
					if (i < csv.row_header_.size() - 1) {
						os << ", ";
					}
				}
				os << '\n';
			}
			for (std::size_t i = 0; i < csv.entries_.size(); ++i) {
				for (std::size_t j = 0; j < csv.entries_[i].size(); ++j) {
					os << csv.entries_[i][j];
					if (j < csv.entries_[i].size() - 1) {
						os << ", ";
					}
				}
				os << '\n';
			}
			return os;
		}

	private:
		static std::vector<std::string> split(const std::string& data, const char delimiter) {
			std::vector<std::string> lines;
			std::istringstream iss{data};

			for (std::string line; std::getline(iss, line, delimiter);) {
				lines.push_back(std::move(line));
			}

			return lines;
		}

		static T parse_token(const std::string& token) {
			T t;
			std::istringstream{token} >> t;
			return t;
		}

		std::vector<std::string> row_header_;
		std::vector<std::vector<T>> entries_;
	};
}
