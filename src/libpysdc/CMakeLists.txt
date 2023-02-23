add_library(pysdc SHARED
    bigrams.cxx
    bigram_multiset.cxx
    unordered_bigram_multiset.cxx
    sequence_matcher.cxx
)
#target_link_libraries(pysdc LINK_PUBLIC sdcxx)
