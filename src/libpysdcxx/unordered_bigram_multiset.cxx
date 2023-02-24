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


using unordered_wbigram_multiset = libsdcxx::unordered_wbigram_multiset;


extern "C" {

/** Default constructor */
unordered_wbigram_multiset * new_unordered_wbigram_multiset() {
    return new unordered_wbigram_multiset();
}

/** Constructor (from string) */
unordered_wbigram_multiset * new_unordered_wbigram_multiset_str(const wchar_t * str) {
    return new unordered_wbigram_multiset(str);
}

/** Copy constructor */
unordered_wbigram_multiset * new_unordered_wbigram_multiset_copy(
    const unordered_wbigram_multiset * bgrms)
{
    return new unordered_wbigram_multiset(*bgrms);
}

/** Destructor */
void delete_unordered_wbigram_multiset(unordered_wbigram_multiset * bgrms) {
    delete bgrms;
}


/** Bigrams size */
size_t unordered_wbigram_multiset_size(const unordered_wbigram_multiset * bgrms) {
    return bgrms->size();
}


/** Begin const. iterator */
unordered_wbigram_multiset::const_iterator * unordered_wbigram_multiset_cbegin(
    const unordered_wbigram_multiset * bgrms)
{
    return new unordered_wbigram_multiset::const_iterator(bgrms->cbegin());
}

/** End const. iterator */
unordered_wbigram_multiset::const_iterator * unordered_wbigram_multiset_cend(
    const unordered_wbigram_multiset * bgrms)
{
    return new unordered_wbigram_multiset::const_iterator(bgrms->cend());
}

/** Compare const. iterators (!=) */
int unordered_wbigram_multiset_citer_ne(
    const unordered_wbigram_multiset::const_iterator * iter1,
    const unordered_wbigram_multiset::const_iterator * iter2)
{
    return *iter1 != *iter2 ? 1 : 0;
}

/** Dereference const. iterator */
void unordered_wbigram_multiset_citer_deref(
    const unordered_wbigram_multiset::const_iterator * iter,
    wchar_t * ch1, wchar_t * ch2)
{
    const auto & bigram = **iter;
    *ch1 = std::get<0>(bigram);
    *ch2 = std::get<1>(bigram);
}

/** Increment const. iterator */
void unordered_wbigram_multiset_citer_inc(
    unordered_wbigram_multiset::const_iterator * iter)
{
    ++*iter;
}

/** Iterator destructor */
void delete_unordered_wbigram_multiset_citer(
    unordered_wbigram_multiset::const_iterator * iter)
{
    delete iter;
}


/** += operator (add right argument bigrams to left argument) */
unordered_wbigram_multiset * unordered_wbigram_multiset_iadd(
    unordered_wbigram_multiset * larg,
    const unordered_wbigram_multiset * rarg)
{
    *larg += *rarg;
    return larg;
}

/** + operator (produce new union of 2 bigrams) */
unordered_wbigram_multiset * unordered_wbigram_multiset_add(
    const unordered_wbigram_multiset * arg1,
    const unordered_wbigram_multiset * arg2)
{
    return new unordered_wbigram_multiset(*arg1 + *arg2);
}


/** Calculate intersection size */
size_t unordered_wbigram_multiset_intersect_size(
    const unordered_wbigram_multiset * bgrms1,
    const unordered_wbigram_multiset * bgrms2)
{
    return unordered_wbigram_multiset::intersect_size(*bgrms1, *bgrms2);
}


/** Calculate Sørensen–Dice coefficient */
double unordered_wbigram_multiset_sorensen_dice_coef(
    const unordered_wbigram_multiset * bgrms1,
    const unordered_wbigram_multiset * bgrms2)
{
    return unordered_wbigram_multiset::sorensen_dice_coef(*bgrms1, *bgrms2);
}


/** Serialise bigrams */
size_t unordered_wbigram_multiset_str(
    const unordered_wbigram_multiset * bgrms,
    wchar_t * buffer, size_t max_len)
{
    return libpysdc::serialise(*bgrms, buffer, max_len);
}

}  // end of extern "C" decl
