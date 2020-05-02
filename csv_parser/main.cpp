#include <iostream>
#include <sstream>
#include <string>
#include <vector>

template <typename...>
struct TypeList {};

std::tuple<> parse(TypeList<>, const std::vector<std::string>&, const std::size_t) {
	return {};
}

template <typename T>
T parse(const std::vector<std::string>& v, const std::size_t i) {
	T t;
	std::istringstream{v[i]} >> t;
	return t;
}

template<typename T, typename ... Rest>
std::tuple<T, Rest...> parse(TypeList<T, Rest...>, const std::vector<std::string>& v, const std::size_t i) {
	const auto t1 = std::make_tuple(parse<T>(v, i));
	const auto t2 = parse(TypeList<Rest...>{}, v, i + 1);
	return std::tuple_cat(t1, t2);
}

template<typename... Types>
std::tuple<Types...> parse(const std::vector<std::string>& v) {
	return parse(TypeList<Types...>{}, v, 0);
}

int main() {

	const auto tokens = std::vector<std::string>{ "a", "1", "3.14" };
	const auto [c, i, d] = parse<char, int, double>(tokens);
	std::cout << c << ' ' << i << ' ' << d;

	return 0;
}