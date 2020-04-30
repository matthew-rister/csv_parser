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

			for (const auto& line : lines) {
				const auto tokens = split(line, ',');
				entries_.emplace_back();
				entries_.back().reserve(tokens.size());

				for (const auto& token : tokens) {
					const auto value = parse(token);

					if constexpr (std::is_move_assignable<T>::value) {
						entries_.back().push_back(std::move(value));
					} else {
						entries_.back().push_back(value);
					}
				}
			}
		}

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
		static std::vector<std::string> split(const std::string& line, const char delimiter) {
			std::vector<std::string> tokens;
			std::istringstream iss{line};

			for (std::string token;
				std::getline(iss, token, delimiter);
				tokens.push_back(std::move(token))) {}

			return tokens;
		}

		static T parse(const std::string& token) {
			T t;
			std::istringstream{token} >> t;
			return t;
		}

		std::vector<std::vector<T>> entries_;
	};
}
