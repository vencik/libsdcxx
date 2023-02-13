add_library(unit_test unit_test.cxx)

add_executable(test_bigrams test_bigrams.cxx)
target_link_libraries(test_bigrams LINK_PUBLIC unit_test)
add_test(libsdcxx::test_bigrams test_bigrams)

#add_executable(test_bigram_multiset test_bigram_multiset.cxx)
#target_link_libraries(test_bigrams LINK_PUBLIC unit_test)
#add_test(libsdcxx::test_bigram_multiset test_bigram_multiset)
