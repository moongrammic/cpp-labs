#include "ip_filter.h"

#include <iostream>

int main()
{
    try
    {
        auto pool = read_pool(std::cin);

        sort_reverse(pool);

        print_pool(std::cout, pool);                  // все адреса
        print_pool(std::cout, filter(pool, 1));       // первый байт 1
        print_pool(std::cout, filter(pool, 46, 70));  // первый байт 46, второй 70
        print_pool(std::cout, filter_any(pool, 46));  // любой байт 46
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
