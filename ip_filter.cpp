#include "ip_filter.h"

#include <algorithm>
#include <iterator>
#include <istream>
#include <ostream>
#include <stdexcept>

std::vector<std::string> split(const std::string &str, char d)
{
    std::vector<std::string> r;

    std::string::size_type start = 0;
    std::string::size_type stop = str.find_first_of(d);
    while (stop != std::string::npos)
    {
        r.push_back(str.substr(start, stop - start));

        start = stop + 1;
        stop = str.find_first_of(d, start);
    }

    r.push_back(str.substr(start));

    return r;
}

Ip parse_ip(const std::string &text)
{
    const auto parts = split(text, '.');
    if (parts.size() != 4)
    {
        throw std::invalid_argument("bad ip address: " + text);
    }

    // лямбда: строка состоит только из цифр
    const auto is_number = [](const std::string &s) {
        return !s.empty() && s.size() <= 3 &&
               std::all_of(s.begin(), s.end(), [](char c) { return c >= '0' && c <= '9'; });
    };

    int bytes[4] = {0, 0, 0, 0};
    for (decltype(parts.size()) i = 0; i < parts.size(); ++i)
    {
        if (!is_number(parts[i]))
        {
            throw std::invalid_argument("bad ip address: " + text);
        }

        const int value = std::stoi(parts[i]);
        if (value > 255)
        {
            throw std::invalid_argument("bad ip address: " + text);
        }
        bytes[i] = value;
    }

    return std::make_tuple(bytes[0], bytes[1], bytes[2], bytes[3]);
}

IpPool read_pool(std::istream &in)
{
    IpPool pool;

    for (std::string line; std::getline(in, line);)
    {
        // на Windows файл может прийти с символами \r\n
        if (!line.empty() && line.back() == '\r')
        {
            line.pop_back();
        }
        if (line.empty())
        {
            continue;
        }

        const auto fields = split(line, '\t');
        pool.push_back(parse_ip(fields.at(0)));
    }

    return pool;
}

void sort_reverse(IpPool &pool)
{
    std::sort(pool.begin(), pool.end(), [](const Ip &a, const Ip &b) { return a > b; });
}

namespace
{
template <typename Predicate>
IpPool filter_if(const IpPool &pool, Predicate predicate)
{
    IpPool result;
    std::copy_if(pool.begin(), pool.end(), std::back_inserter(result), predicate);
    return result;
}
} // namespace

IpPool filter(const IpPool &pool, int first)
{
    return filter_if(pool, [first](const Ip &ip) { return std::get<0>(ip) == first; });
}

IpPool filter(const IpPool &pool, int first, int second)
{
    return filter_if(pool, [first, second](const Ip &ip) {
        return std::tie(std::get<0>(ip), std::get<1>(ip)) == std::tie(first, second);
    });
}

IpPool filter_any(const IpPool &pool, int value)
{
    return filter_if(pool, [value](const Ip &ip) {
        return std::get<0>(ip) == value || std::get<1>(ip) == value ||
               std::get<2>(ip) == value || std::get<3>(ip) == value;
    });
}

std::string ip_to_string(const Ip &ip)
{
    return std::to_string(std::get<0>(ip)) + "." + std::to_string(std::get<1>(ip)) + "." +
           std::to_string(std::get<2>(ip)) + "." + std::to_string(std::get<3>(ip));
}

void print_pool(std::ostream &out, const IpPool &pool)
{
    for (const auto &ip : pool)
    {
        out << ip_to_string(ip) << '\n';
    }
}
