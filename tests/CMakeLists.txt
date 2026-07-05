add_executable(database_tests
    test_leaf.cpp
    test_internal.cpp
    test_btree.cpp
    test_pager.cpp
)

target_link_libraries(database_tests
    PRIVATE
        Catch2::Catch2WithMain
        sqlite_lib
)

include(${Catch2_SOURCE_DIR}/extras/Catch.cmake)

catch_discover_tests(database_tests)
