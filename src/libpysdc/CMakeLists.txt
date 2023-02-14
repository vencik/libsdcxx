add_library(pysdc SHARED
    pysdc_bigrams.cxx
    pysdc_bigram_multiset.cxx
    pysdc_unordered_bigram_multiset.cxx
)
#target_link_libraries(pysdc LINK_PUBLIC sdcxx)
