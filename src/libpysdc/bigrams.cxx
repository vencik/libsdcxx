/**
 *  \file
 *  \brief  Sørensen–Dice coefficient on multisets of bigrams: Python binding
 *
 *  \date   2023/02/09
 *  \author Václav Krpec  <vencik@razdva.cz>
 *
 *
 *  LEGAL NOTICE
 *
 *  Copyright (c) 2023, Václav Krpec
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the distribution.
 *
 *  3. Neither the name of the copyright holder nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 *  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 *  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 *  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER
 *  OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 *  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 *  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 *  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 *  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 *  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "libsdcxx/bigrams.hxx"

#include "util.hxx"

#include <sstream>
#include <cwchar>


using wbigrams = libsdcxx::wbigrams;


extern "C" {

/** Default constructor */
wbigrams * new_wbigrams() { return new wbigrams(); }

/** Constructor (from string) */
wbigrams * new_wbigrams_str(const wchar_t * str) {
    return new wbigrams(str);
}

/** Copy constructor */
wbigrams * new_wbigrams_copy(const wbigrams * bgrms) { return new wbigrams(*bgrms); }

/** Destructor */
void delete_wbigrams(wbigrams * bgrms) { delete bgrms; }


/** Bigrams size */
size_t wbigrams_size(const wbigrams * bgrms) { return bgrms->size(); }


/** Begin const. iterator */
wbigrams::const_iterator * wbigrams_cbegin(const wbigrams * bgrms) {
    return new wbigrams::const_iterator(bgrms->cbegin());
}

/** End const. iterator */
wbigrams::const_iterator * wbigrams_cend(const wbigrams * bgrms) {
    return new wbigrams::const_iterator(bgrms->cend());
}

/** Compare const. iterators (!=) */
int wbigrams_citer_ne(
    const wbigrams::const_iterator * iter1,
    const wbigrams::const_iterator * iter2)
{
    return *iter1 != *iter2 ? 1 : 0;
}

/** Dereference const. iterator */
void wbigrams_citer_deref(
    const wbigrams::const_iterator * iter,
    wchar_t * ch1, wchar_t * ch2, size_t * cnt)
{
    const auto & bigram_cnt = **iter;
    const auto & bigram = std::get<0>(bigram_cnt);
    *ch1 = std::get<0>(bigram);
    *ch2 = std::get<1>(bigram);
    *cnt = std::get<1>(bigram_cnt);
}

/** Increment const. iterator */
void wbigrams_citer_inc(wbigrams::const_iterator * iter) { ++*iter; }

/** Iterator destructor */
void delete_wbigrams_citer(wbigrams::const_iterator * iter) { delete iter; }


/** += operator (add right argument bigrams to left argument) */
wbigrams * wbigrams_iadd(wbigrams * larg, const wbigrams * rarg) {
    *larg += *rarg;
    return larg;
}

/** + operator (produce new union of 2 bigrams) */
wbigrams * wbigrams_add(const wbigrams * arg1, const wbigrams * arg2) {
    return new wbigrams(*arg1 + *arg2);
}


/** Calculate intersection size */
size_t wbigrams_intersect_size(const wbigrams * bgrms1, const wbigrams * bgrms2) {
    return wbigrams::intersect_size(*bgrms1, *bgrms2);
}


/** Calculate Sørensen–Dice coefficient */
double wbigrams_sorensen_dice_coef(const wbigrams * bgrms1, const wbigrams * bgrms2) {
    return wbigrams::sorensen_dice_coef(*bgrms1, *bgrms2);
}


/** Serialise bigrams */
size_t wbigrams_str(const wbigrams * bgrms, wchar_t * buffer, size_t max_len) {
    return libpysdc::serialise(*bgrms, buffer, max_len);
}

}  // end of extern "C" decl
