add_executable(test_bigrams test_bigrams.cxx)
target_link_libraries(test_bigrams LINK_PUBLIC sdcxx)
add_test(libsdcxx::test_bigrams test_bigrams)
