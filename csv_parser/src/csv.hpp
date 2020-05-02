#pragma once

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

namespace csv {

	template <typename T> class Csv {

	public:
		explicit Csv(const std::string& data) : entries_{parse_data(data)} {}

		friend std::ostream& operator<<(std::ostream& os, const Csv& csv) {
			for (std::size_t i = 0; i < csv.entries_.size(); ++i) {
				for (std::size_t j = 0; j < csv.entries_[i].size(); ++j) {
					os << csv.entries_[i][j] << (j < csv.entries_[i].size() - 1 ? ", " : "");
				}
				os << '\n';
			}
			return os;
		}

	private:
		static std::vector<std::vector<T>> parse_data(const std::string& data) {
			const auto lines = split(data, '\n');
			std::vector<std::vector<T>> entries;
			entries.reserve(lines.size());

			std::transform(std::cbegin(lines), std::cend(lines), std::back_inserter(entries), [](const auto& line) {
				return parse_line(line);
			});

			return entries;
		}

		static std::vector<T> parse_line(const std::string& line) {
			const auto tokens = split(line, ',');
			std::vector<T> values;
			values.reserve(tokens.size());

			std::transform(std::cbegin(tokens), std::cend(tokens), values, [](const auto& token) {
				return parse_token(token);
			});

			return values;
		}

		static T parse_token(const std::string& token) {
			T t;
			std::istringstream{token} >> t;
			return t;
		}

		static std::vector<std::string> split(const std::string& line, const char delimiter) {
			std::vector<std::string> tokens;
			std::istringstream iss{ line };

			for (std::string token;
				std::getline(iss, token, delimiter);
				tokens.push_back(std::move(token))) {
			}

			return tokens;
		}

		std::vector<std::vector<T>> entries_;
	};
}
