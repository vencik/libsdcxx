#ifndef libsdcxx__bigram_multiset_hxx
#define libsdcxx__bigram_multiset_hxx

/**
 *  \file
 *  \brief  String smilarity using Sørensen–Dice coefficient on multisets of bigrams
 *
 *  See https://en.wikipedia.org/wiki/S%C3%B8rensen%E2%80%93Dice_coefficient
 *
 *  \date   2023/02/13
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

#include <cstddef>
#include <set>
#include <unordered_set>
#include <string>
#include <tuple>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <functional>


namespace libsdcxx {

/**
 *  \brief  String Bigrams (implemented using std::{unordered_}multiset)
 *
 *  \tparam  Char           Character type
 *  \tparam  Iplementation  Container type (a multiset)
 */
template <typename Char, class Implementation>
class basic_bigram_multiset {
    public:

    using char_t = Char;                                /**< Character type */
    using string_t = std::basic_string<char_t>;         /**< String type    */
    using bigram_t = std::tuple<char_t, char_t>;        /**< Bigram type    */

    private:

    using impl_t = Implementation;

    public:

    using const_iterator = typename impl_t::const_iterator;  /**< Const. iterator type */

    private:

    impl_t m_impl;  /**< Sorted list of bigrams */

    public:

    /** Empty bigram multiset constructor */
    basic_bigram_multiset() = default;

    /**
     *  \brief  Constructor (from a string)
     *
     *  \param  str  Bigrams source
     */
    basic_bigram_multiset(const std::basic_string<char_t> & str) {
        if (str.size() > 1)  // there must be at least 2 characters to create bigrams
            for (size_t i = 0; i < str.size() - 1; ++i)  // create bigrams
                m_impl.emplace(str[i], str[i+1]);
    }

    /** Copy constructor */
    basic_bigram_multiset(const basic_bigram_multiset & ) = default;

    /** Move constructor */
    basic_bigram_multiset(basic_bigram_multiset && ) = default;

    /** Copy assignment */
    basic_bigram_multiset & operator = (const basic_bigram_multiset & ) = default;

    /**
     *  \brief  Size getter
     *
     *  \return Number of bigrams
     */
    size_t size() const { return m_impl.size(); }

    /** \brief  Begin const. iterator getter */
    const_iterator cbegin() const { return m_impl.cbegin(); }

    /** \brief  End const. iterator getter */
    const_iterator cend() const { return m_impl.cend(); }

    /** \brief  Begin iterator getter */
    const_iterator begin() const { return m_impl.begin(); }

    /** \brief  End iterator getter */
    const_iterator end() const { return m_impl.end(); }

    /**
     *  \brief  Update current bigram multiset by other bigrams
     *
     *  \param  other  Other bigrams
     */
    basic_bigram_multiset & operator += (const basic_bigram_multiset & other) {
        if (size() == 0)  // optimisation for empty multiset
            return *this = other;

        for (const auto & bigram: other.m_impl)
            m_impl.insert(bigram);

        return *this;
    }

    private:

    /** Union of bigram multisets (fixed point of template recursion) */
    static basic_bigram_multiset unite(const basic_bigram_multiset & arg) {
        return arg;  // make a copy
    }

    public:

    /**
     *  \brief  Union of bigram multisets
     *
     *  The unionis constructed right-to-left (but that's OK, set union is comutative).
     *
     *  \param  arg1  Bigram multiset
     *  \param  args  Bigram multisets
     *
     *  \return Union of bigrams
     */
    template <class ... Bigrams>
    static basic_bigram_multiset unite(
        const basic_bigram_multiset & arg1,
        const Bigrams & ... args)
    {
        return unite(args...) += arg1;
    }

    /**
     *  \brief  Union of 2 bigram multisets
     *
     *  \param  other  Bigram multiset
     *
     *  \return Union of bigram multisets
     */
    basic_bigram_multiset operator + (const basic_bigram_multiset & other) const {
        return unite(*this, other);
    }

    /**
     *  \brief  Bigram multisets intersection size
     *
     *  Calculate bigram multiset intersection cardinality.
     *
     *  \param  bigrams1  Bigram multiset
     *  \param  bigrams2  Bigram multiset
     *
     *  \return Size of the bigram multisets intersection
     */
    static size_t intersect_size(
        const basic_bigram_multiset & bigrams1,
        const basic_bigram_multiset & bigrams2)
    {
        struct insert_counter {  // we shall only count the number of attempted insertions
            size_t & m_cnt;

            insert_counter(size_t & cnt): m_cnt(cnt) {}

            insert_counter & operator * () { return *this; }  // don't do anything

            void operator ++ () {}  // ditto

            void operator = (const bigram_t & ) { ++m_cnt; }  // count inserted bigrams
        };

        size_t size = 0;
        std::set_intersection(
            bigrams1.m_impl.begin(), bigrams1.m_impl.end(),
            bigrams2.m_impl.begin(), bigrams2.m_impl.end(),
            insert_counter(size));
        return size;
    }

    /**
     *  \brief  Bigram multisets Sørensen–Dice coefficient
     *
     *  Note that SDC of bigram multisets with empty intersection is 0.
     *  That means that if the SDC is used to calculate string similarity then single-char
     *  strings shall need direct comparison (SDC shall be 0 by default as single
     *  character doesn't produce any bigrams).
     *  Alternatively, you may augment single-char strings by a selected character
     *  (e.g. white space) to enforce at least 2-character strings...
     *
     *  \param  bigrams1  Bigram multiset
     *  \param  bigrams2  Bigram multiset
     *
     *  \return Sørensen–Dice coefficient
     */
    static double sorensen_dice_coef(
        const basic_bigram_multiset & bigrams1,
        const basic_bigram_multiset & bigrams2)
    {
        const auto isect_size = intersect_size(bigrams1, bigrams2);
        return isect_size ? 2.0 * isect_size / (bigrams1.size() + bigrams2.size()) : 0.0;
    }

};  // end of template class basic_bigram_multiset


/**
 *  \brief  (Wide) bigram multiset serialisation
 *
 *  \tparam  Char           Character type
 *  \tparam  Iplementation  Container type (a multiset)
 *
 *  \param  out    Output stream
 *  \param  bgrms  Bigram multiset
 *  \param  name   Bigram multiset class (display) name
 *
 *  \return \c out
 */
template <typename Char, class Implementation>
std::basic_ostream<Char> & serialise_bigram_multiset (
    std::basic_ostream<Char> & out,
    const basic_bigram_multiset<Char, Implementation> & bgrms,
    const char * name)
{
    static const auto * left_curly_bracket = "{";

    out << name << "(size: " << bgrms.size() << ", ";

    const auto * separator = left_curly_bracket;
    for (const auto & bigram: bgrms) {
        out << separator << std::get<0>(bigram) << std::get<1>(bigram);
        separator = ", ";
    }
    out << (separator == left_curly_bracket ? "{}" : "}") << ')';

    return out;
}


/**
 *  \brief  Bigram hash function
 *
 *  For character types which will fit twice in \c size_t, that will do nicely.
 *  If character type is of the same size as size_t, we shall have to actually hash
 *  the characters.
 *
 *  \tparam  Char           Character type
 *  \tparam  fits_size_t    Character tuple fits in \c size_t
 */
template <typename Char, bool fits_size_t>
struct hash_bigram {};

template <typename Char>
struct hash_bigram<Char, true> {
    /** Simply concatenate bytes, whole tuple fits in size_t */
    size_t operator () (const std::tuple<Char, Char> & bigram) const noexcept {
        return
            static_cast<size_t>(std::get<1>(bigram)) << (8 * sizeof(Char)) |
            static_cast<size_t>(std::get<0>(bigram));
    }
};

template <typename Char>
struct hash_bigram<Char, false> {
    /** Hash combination using the method from Boost */
    size_t operator () (const std::tuple<Char, Char> & bigram) const noexcept {
        const auto hash1 = std::hash<Char>{}(std::get<0>(bigram));
        const auto hash2 = std::hash<Char>{}(std::get<1>(bigram));
        return hash1 ^ (hash2 * 0x9e3779b9 + (hash1 << 6) + (hash1 >> 2));
    }
};


/**< ASCII/ANSI string bigram multisets */
using bigram_multiset =
    basic_bigram_multiset<char, std::multiset<std::tuple<char, char>>>;
using unordered_bigram_multiset =
    basic_bigram_multiset<char,
        std::unordered_multiset<std::tuple<char, char>,
            hash_bigram<char, 2 * sizeof(char) <= sizeof(size_t)>>>;

/**< UNICODE string bigram multisets */
using wbigram_multiset =
    basic_bigram_multiset<wchar_t, std::multiset<std::tuple<wchar_t, wchar_t>>>;
using unordered_wbigram_multiset =
    basic_bigram_multiset<wchar_t,
        std::unordered_multiset<std::tuple<wchar_t, wchar_t>,
            hash_bigram<wchar_t, 2 * sizeof(wchar_t) <= sizeof(size_t)>>>;


/** Bigram multiset serialisation operator */
inline std::ostream & operator << (
    std::ostream & out, const bigram_multiset & bgrms)
{
    return serialise_bigram_multiset(out, bgrms, "bigram_multiset");
}

/** Unordered bigram multiset serialisation operator */
inline std::ostream & operator << (
    std::ostream & out, const unordered_bigram_multiset & bgrms)
{
    return serialise_bigram_multiset(out, bgrms, "unordered_bigram_multiset");
}


/** Wide bigram multiset serialisation operator */
inline std::wostream & operator << (
    std::wostream & out, const wbigram_multiset & bgrms)
{
    return serialise_bigram_multiset(out, bgrms, "wbigram_multiset");
}

/** Unordered wide bigram multiset serialisation operator */
inline std::wostream & operator << (
    std::wostream & out, const unordered_wbigram_multiset & bgrms)
{
    return serialise_bigram_multiset(out, bgrms, "unordered_wbigram_multiset");
}

}  // end of namespace libsdcxx

#endif  // end of #ifndef libsdcxx__bigram_multiset_hxx
