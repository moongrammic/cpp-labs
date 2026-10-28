#pragma once

#include <iosfwd>
#include <string>
#include <tuple>
#include <vector>

// IPv4-адрес хранится как кортеж из четырёх чисел (байтов), а не как строки:
// тогда сравнение идёт по числам (2 < 10), а не посимвольно ("2" > "10").
// Тип кортежа выводится через decltype.
using Ip = decltype(std::make_tuple(0, 0, 0, 0));
using IpPool = std::vector<Ip>;

// ("", '.') -> [""]; ("11.", '.') -> ["11", ""]; (".11", '.') -> ["", "11"]
std::vector<std::string> split(const std::string &str, char d);

// "46.70.225.39" -> (46, 70, 225, 39). Бросает std::invalid_argument при ошибке.
Ip parse_ip(const std::string &text);

// Читает строки вида "ip \t text \t text", учитывается только первое поле.
IpPool read_pool(std::istream &in);

// Обратная лексикографическая сортировка (по первому байту, затем по второму и т.д.).
void sort_reverse(IpPool &pool);

// Первый байт равен first.
IpPool filter(const IpPool &pool, int first);

// Первый байт равен first, второй равен second.
IpPool filter(const IpPool &pool, int first, int second);

// Любой из четырёх байтов равен value.
IpPool filter_any(const IpPool &pool, int value);

std::string ip_to_string(const Ip &ip);

// Печатает по одному адресу в строке.
void print_pool(std::ostream &out, const IpPool &pool);
