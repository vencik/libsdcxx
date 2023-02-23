/**
 *  \file
 *  \brief  Sørensen–Dice coefficient on multisets of bigrams: Python binding
 *
 *  \date   2023/02/14
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

#include "libsdcxx/bigram_multiset.hxx"

#include "util.hxx"

#include <sstream>
#include <cwchar>


using wbigram_multiset = libsdcxx::wbigram_multiset;


extern "C" {

/** Default constructor */
wbigram_multiset * new_wbigram_multiset() {
    return new wbigram_multiset();
}

/** Constructor (from string) */
wbigram_multiset * new_wbigram_multiset_str(const wchar_t * str) {
    return new wbigram_multiset(str);
}

/** Copy constructor */
wbigram_multiset * new_wbigram_multiset_copy(const wbigram_multiset * bgrms) {
    return new wbigram_multiset(*bgrms);
}

/** Destructor */
void delete_wbigram_multiset(wbigram_multiset * bgrms) {
    delete bgrms;
}


/** Bigrams size */
size_t wbigram_multiset_size(const wbigram_multiset * bgrms) {
    return bgrms->size();
}


/** Begin const. iterator */
wbigram_multiset::const_iterator * wbigram_multiset_cbegin(
    const wbigram_multiset * bgrms)
{
    return new wbigram_multiset::const_iterator(bgrms->cbegin());
}

/** End const. iterator */
wbigram_multiset::const_iterator * wbigram_multiset_cend(
    const wbigram_multiset * bgrms)
{
    return new wbigram_multiset::const_iterator(bgrms->cend());
}

/** Compare const. iterators (!=) */
int wbigram_multiset_citer_ne(
    const wbigram_multiset::const_iterator * iter1,
    const wbigram_multiset::const_iterator * iter2)
{
    return *iter1 != *iter2 ? 1 : 0;
}

/** Dereference const. iterator */
void wbigram_multiset_citer_deref(
    const wbigram_multiset::const_iterator * iter,
    wchar_t * ch1, wchar_t * ch2)
{
    const auto & bigram = **iter;
    *ch1 = std::get<0>(bigram);
    *ch2 = std::get<1>(bigram);
}

/** Increment const. iterator */
void wbigram_multiset_citer_inc(wbigram_multiset::const_iterator * iter) {
    ++*iter;
}

/** Iterator destructor */
void delete_wbigram_multiset_citer(wbigram_multiset::const_iterator * iter) {
    delete iter;
}


/** += operator (add right argument bigrams to left argument) */
wbigram_multiset * wbigram_multiset_iadd(
    wbigram_multiset * larg,
    const wbigram_multiset * rarg)
{
    *larg += *rarg;
    return larg;
}

/** + operator (produce new union of 2 bigrams) */
wbigram_multiset * wbigram_multiset_add(
    const wbigram_multiset * arg1,
    const wbigram_multiset * arg2)
{
    return new wbigram_multiset(*arg1 + *arg2);
}


/** Calculate intersection size */
size_t wbigram_multiset_intersect_size(
    const wbigram_multiset * bgrms1,
    const wbigram_multiset * bgrms2)
{
    return wbigram_multiset::intersect_size(*bgrms1, *bgrms2);
}


/** Calculate Sørensen–Dice coefficient */
double wbigram_multiset_sorensen_dice_coef(
    const wbigram_multiset * bgrms1,
    const wbigram_multiset * bgrms2)
{
    return wbigram_multiset::sorensen_dice_coef(*bgrms1, *bgrms2);
}


/** Serialise bigrams */
size_t wbigram_multiset_str(
    const wbigram_multiset * bgrms,
    wchar_t * buffer, size_t max_len)
{
    return libpysdc::serialise(*bgrms, buffer, max_len);
}

}  // end of extern "C" decl
