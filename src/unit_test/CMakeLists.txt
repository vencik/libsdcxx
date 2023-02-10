add_executable(bigrams_ut bigrams.cxx)
target_link_libraries(bigrams_ut LINK_PUBLIC sdcxx)
add_test(libsdcxx::bigrams bigrams_ut)
