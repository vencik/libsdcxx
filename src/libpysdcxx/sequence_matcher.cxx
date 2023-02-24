/**
 *  \file
 *  \brief  Sørensen–Dice similarity based sequence matching: Python binding
 *
 *  \date   2023/02/22
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

#include "libsdcxx/sequence_matcher.hxx"
#include "libsdcxx/bigrams.hxx"

#include "util.hxx"

#include <sstream>
#include <cwchar>


using wsequence_matcher = libsdcxx::wsequence_matcher;
using wbigrams = libsdcxx::wbigrams;


extern "C" {

/** Constructor */
wsequence_matcher * new_wsequence_matcher() { return new wsequence_matcher(); }

/** Destructor */
void delete_wsequence_matcher(wsequence_matcher * matcher) { delete matcher; }

/** Reserve space for sequence */
void wsequence_matcher_reserve(wsequence_matcher * matcher, size_t len) {
    matcher->reserve(len);
}

/** Size getter */
size_t wsequence_matcher_size(const wsequence_matcher * matcher) {
    return matcher->size();
}

/** Push bigrams back */
void wsequence_matcher_push_back(
    wsequence_matcher * matcher,
    const wbigrams * bgrms, int strip)
{
    matcher->push_back(*bgrms, 0 != strip);
}

/** Push in-place created bigrams back */
void wsequence_matcher_emplace_back(
    wsequence_matcher * matcher,
    const wchar_t * str, int strip)
{
    matcher->emplace_back(str, 0 != strip);
}


/** Begin match iterator */
wsequence_matcher::iterator * wsequence_matcher_begin(
    wsequence_matcher * matcher,
    const wbigrams * bgrms, double threshold)
{
    return new wsequence_matcher::iterator(matcher->begin(*bgrms, threshold));
}

/** End match iterator */
wsequence_matcher::iterator * wsequence_matcher_end(wsequence_matcher * matcher) {
    return new wsequence_matcher::iterator(matcher->end());
}

/** Dereference match iterator */
wbigrams * wsequence_matcher_iter_deref(const wsequence_matcher::iterator * iter) {
    return new wbigrams(**iter);
}

/** Matching sequence size */
size_t wsequence_matcher_iter_size(const wsequence_matcher::iterator * iter) {
    return iter->size();
}

/** Matching sequence begin */
size_t wsequence_matcher_iter_begin(const wsequence_matcher::iterator * iter) {
    return iter->begin();
}

/** Matching sequence end */
size_t wsequence_matcher_iter_end(const wsequence_matcher::iterator * iter) {
    return iter->end();
}

/** Matching sequence SDC */
double wsequence_matcher_iter_sdc(const wsequence_matcher::iterator * iter) {
    return iter->sorensen_dice_coef();
}

/** Increment match iterator */
void wsequence_matcher_iter_inc(wsequence_matcher::iterator * iter) { ++*iter; }

/** Compare match iterators (!=) */
int wsequence_matcher_iter_ne(
    const wsequence_matcher::iterator * iter1,
    const wsequence_matcher::iterator * iter2)
{
    return *iter1 != *iter2 ? 1 : 0;
}

/** Iterator destructor */
void delete_wsequence_matcher_iter(wsequence_matcher::iterator * iter) { delete iter; }


/** Serialise match iterator */
size_t wsequence_matcher_iter_str(
    const wsequence_matcher::iterator * iter,
    wchar_t * buffer, size_t max_len)
{
    return libpysdc::serialise(*iter, buffer, max_len);
}

}  // end of extern "C" decl
