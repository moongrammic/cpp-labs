cmake_minimum_required(VERSION 3.10)

set(PATCH_VERSION "1" CACHE STRING "Patch version")

project(ip_filter VERSION 1.0.${PATCH_VERSION})

find_package(Boost COMPONENTS unit_test_framework REQUIRED)

add_library(ip_filter_lib ip_filter.cpp)
add_executable(ip_filter main.cpp)
add_executable(test_ip_filter test_ip_filter.cpp)

set_target_properties(ip_filter_lib ip_filter test_ip_filter PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
)

target_compile_definitions(test_ip_filter PRIVATE BOOST_TEST_DYN_LINK)
target_include_directories(test_ip_filter PRIVATE ${Boost_INCLUDE_DIR})

target_link_libraries(ip_filter PRIVATE ip_filter_lib)
target_link_libraries(test_ip_filter PRIVATE ${Boost_LIBRARIES} ip_filter_lib)

foreach(target ip_filter_lib ip_filter test_ip_filter)
    if (MSVC)
        target_compile_options(${target} PRIVATE /W4)
    else ()
        target_compile_options(${target} PRIVATE -Wall -Wextra -pedantic -Werror)
    endif()
endforeach()

install(TARGETS ip_filter RUNTIME DESTINATION bin)

if (WIN32)
    set(CPACK_GENERATOR ZIP)
elseif (APPLE)
    set(CPACK_GENERATOR TGZ)
else ()
    set(CPACK_GENERATOR DEB)
endif()

set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")

set(CPACK_PACKAGE_CONTACT example@example.com)

include(CPack)

enable_testing()

add_test(test_ip_filter test_ip_filter)
