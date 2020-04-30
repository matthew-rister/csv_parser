#pragma once

#include <algorithm>
#include <execution>
#include <sstream>
#include <string>
#include <vector>

namespace csv {

	template <typename T> class Csv {

	public:
		explicit Csv(const std::string& data, const bool has_row_header = false) {

			const auto lines = read_lines(data);
			auto lines_begin = std::cbegin(lines);
			auto lines_size = lines.size();

			if (has_row_header && lines_begin != std::cend(lines)) {
				row_header_ = read_line<std::string>(lines[0]);
				++lines_begin;
				--lines_size;
			}

			entries_.reserve(lines_size);

			std::transform(lines_begin, std::cend(lines), std::back_inserter(entries_), [](const auto& line) {
				return read_line(line);
			});
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
		static std::vector<std::string> read_lines(const std::string& data) {
			std::vector<std::string> lines;
			std::istringstream iss{data};

			for (std::string line; std::getline(iss, line);) {
				lines.push_back(std::move(line));
			}

			return lines;
		}

		template <typename U = T> static std::vector<U> read_line(const std::string& line) {
			std::vector<U> tokens;
			std::istringstream iss{line};

			for (std::string token; std::getline(iss, token, ',');) {
				const auto u = read_token<U>(token);
				if constexpr (std::is_move_assignable<U>::value) {
					tokens.push_back(std::move(u));
				} else {
					tokens.push_back(u);
				}
			}

			return tokens;
		}

		template <typename U = T> static U read_token(const std::string& token) {
			U u;
			std::istringstream{token} >> u;
			return u;
		}

		std::vector<std::string> row_header_;
		std::vector<std::vector<T>> entries_;
	};
}
