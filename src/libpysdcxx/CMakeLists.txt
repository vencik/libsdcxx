add_library(pysdcxx SHARED
    bigrams.cxx
    bigram_multiset.cxx
    unordered_bigram_multiset.cxx
    sequence_matcher.cxx
)
#target_link_libraries(pysdcxx LINK_PUBLIC sdcxx)
