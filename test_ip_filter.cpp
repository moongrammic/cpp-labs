#define BOOST_TEST_MODULE test_ip_filter

#include "ip_filter.h"

#include <boost/test/unit_test.hpp>

#include <sstream>
#include <stdexcept>

namespace
{
std::string to_text(const IpPool &pool)
{
    std::ostringstream out;
    print_pool(out, pool);
    return out.str();
}

IpPool make_pool(const std::string &text)
{
    std::istringstream in(text);
    return read_pool(in);
}
} // namespace

BOOST_AUTO_TEST_SUITE(test_ip_filter)

BOOST_AUTO_TEST_CASE(test_split)
{
    const std::vector<std::string> empty = {""};
    const std::vector<std::string> one = {"11"};
    const std::vector<std::string> dots = {"", "", ""};
    const std::vector<std::string> tail = {"11", ""};
    const std::vector<std::string> head = {"", "11"};
    const std::vector<std::string> two = {"11", "22"};

    BOOST_CHECK(split("", '.') == empty);
    BOOST_CHECK(split("11", '.') == one);
    BOOST_CHECK(split("..", '.') == dots);
    BOOST_CHECK(split("11.", '.') == tail);
    BOOST_CHECK(split(".11", '.') == head);
    BOOST_CHECK(split("11.22", '.') == two);
}

BOOST_AUTO_TEST_CASE(test_parse_ip)
{
    BOOST_CHECK(parse_ip("1.2.3.4") == std::make_tuple(1, 2, 3, 4));
    BOOST_CHECK(parse_ip("255.0.10.200") == std::make_tuple(255, 0, 10, 200));

    BOOST_CHECK_THROW(parse_ip("1.2.3"), std::invalid_argument);
    BOOST_CHECK_THROW(parse_ip("1.2.3.4.5"), std::invalid_argument);
    BOOST_CHECK_THROW(parse_ip("1.2.3.256"), std::invalid_argument);
    BOOST_CHECK_THROW(parse_ip("1..3.4"), std::invalid_argument);
    BOOST_CHECK_THROW(parse_ip("a.b.c.d"), std::invalid_argument);
    BOOST_CHECK_THROW(parse_ip("-1.2.3.4"), std::invalid_argument);
}

BOOST_AUTO_TEST_CASE(test_read_pool_ignores_other_fields_and_crlf)
{
    const auto pool = make_pool("1.2.3.4\t111\t0\r\n5.6.7.8\t2\t3\n");

    BOOST_REQUIRE_EQUAL(pool.size(), 2u);
    BOOST_CHECK(pool[0] == std::make_tuple(1, 2, 3, 4));
    BOOST_CHECK(pool[1] == std::make_tuple(5, 6, 7, 8));
}

BOOST_AUTO_TEST_CASE(test_sort_is_numeric_and_reverse)
{
    auto pool = make_pool("1.1.1.1\n1.10.1.1\n1.2.1.1\n");
    sort_reverse(pool);

    // 10 > 2 > 1, а не строковое "2" > "10" > "1"
    BOOST_CHECK_EQUAL(to_text(pool), "1.10.1.1\n1.2.1.1\n1.1.1.1\n");
}

BOOST_AUTO_TEST_CASE(test_filter_by_first_byte)
{
    auto pool = make_pool("1.231.69.33\n46.70.1.1\n1.1.234.8\n2.1.1.1\n");
    sort_reverse(pool);

    BOOST_CHECK_EQUAL(to_text(filter(pool, 1)), "1.231.69.33\n1.1.234.8\n");
}

BOOST_AUTO_TEST_CASE(test_filter_by_first_and_second_byte)
{
    auto pool = make_pool("46.70.225.39\n46.71.1.1\n45.70.1.1\n46.70.29.76\n");
    sort_reverse(pool);

    BOOST_CHECK_EQUAL(to_text(filter(pool, 46, 70)), "46.70.225.39\n46.70.29.76\n");
}

BOOST_AUTO_TEST_CASE(test_filter_any_byte)
{
    auto pool = make_pool("186.204.34.46\n1.2.3.4\n46.1.1.1\n39.46.86.85\n5.189.203.46\n");
    sort_reverse(pool);

    BOOST_CHECK_EQUAL(to_text(filter_any(pool, 46)),
                      "186.204.34.46\n46.1.1.1\n39.46.86.85\n5.189.203.46\n");
}

BOOST_AUTO_TEST_SUITE_END()
