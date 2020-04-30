#pragma once

#include <sstream>
#include <string>
#include <vector>

namespace csv {

	template <typename T> class Csv {

	public:
		explicit Csv(const std::string& data) {
			for (const auto& line : read_lines(data)) {
				entries_.push_back(read_line(line));
			}
		}

		friend std::ostream& operator<<(std::ostream& os, const Csv& csv) {
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
			std::string line;
			std::istringstream iss{data};

			while (std::getline(iss, line)) {
				lines.push_back(std::move(line));
			}

			return lines;
		}


		static std::vector<T> read_line(const std::string& line) {
			std::vector<T> tokens;
			std::string token;
			std::istringstream iss{line};

			while (std::getline(iss, token, ',')) {
				T t;
				std::istringstream{token} >> t;
				if constexpr (std::is_move_assignable<T>::value) {
					tokens.push_back(std::move(t));
				} else {
					tokens.push_back(t);
				}
			}

			return tokens;
		}

		std::vector<std::vector<T>> entries_;
	};
}
