#pragma once

#include <sstream>
#include <vector>

namespace csv {

	template <typename T>
	class Csv {

	public:
		explicit Csv(const std::string& data) {
			std::string line;
			std::istringstream line_stream{data};

			while (std::getline(line_stream, line)) {
				entries_.emplace_back(std::vector<T>{});
				std::string token;
				std::istringstream token_stream{line};

				while (std::getline(token_stream, token, ',')) {
					T t;
					std::istringstream conversion_stream{token};
					conversion_stream >> t;
					entries_.back().push_back(t);
				}
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
		std::vector<std::vector<T>> entries_;
	};
}
