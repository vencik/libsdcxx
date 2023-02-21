#ifndef libsdcxx__bigrams_hxx
#define libsdcxx__bigrams_hxx

/**
 *  \file
 *  \brief  String smilarity using Sørensen–Dice coefficient on multisets of bigrams
 *
 *  See https://en.wikipedia.org/wiki/S%C3%B8rensen%E2%80%93Dice_coefficient
 *
 *  \date   2023/02/07
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
#include <cassert>
#include <string>
#include <tuple>
#include <list>
#include <vector>
#include <algorithm>
#include <iostream>


namespace libsdcxx {

/**
 *  \brief  String Bigrams
 *
 *  \tparam  Char  Character type
 */
template <typename Char>
class basic_bigrams {
    public:

    using char_t = Char;                                /**< Character type         */
    using string_t = std::basic_string<char_t>;         /**< String type            */
    using bigram_t = std::tuple<char_t, char_t>;        /**< Bigram type            */
    using bigram_cnt_t = std::tuple<bigram_t, size_t>;  /**< [Bigram, count] tuple  */

    private:

    using impl_t = std::list<bigram_cnt_t>;
    using init_t = std::tuple<const impl_t, const size_t>;

    public:

    using const_iterator = typename impl_t::const_iterator;  /**< Const. iterator type */

    private:

    impl_t  m_impl;     /**< Sorted list of bigrams     */
    size_t  m_size;     /**< Individual bigram count    */

    /**
     *  \brief  [bigram, count] comparator
     *
     *  \param  bc1  [bigram, count] tuple
     *  \param  bc2  [bigram, count] tuple
     *
     *  \return Negative value if bc1 < bc2, 0 if equal, positive value otherwise
     */
    static ssize_t cmp(const bigram_cnt_t & bc1, const bigram_cnt_t & bc2) {
        const auto & b1 = std::get<0>(bc1);
        const auto & b2 = std::get<0>(bc2);

        ssize_t diff = ssize_t(std::get<0>(b1)) - ssize_t(std::get<0>(b2));
        if (0 == diff)  // lexicographic descend
            diff = ssize_t(std::get<1>(b1)) - ssize_t(std::get<1>(b2));

        return diff;
    }

    public:

    /** Default constructor */
    basic_bigrams(): m_size(0) {}

    /**
     *  \brief  Constructor (from a string)
     *
     *  \param  str  Bigrams source
     */
    basic_bigrams(const string_t & str): m_size(0) {
        if (str.size() < 2)  // there must be at least 2 characters to create bigrams
            return;

        std::vector<bigram_t> bigram_vec;
        bigram_vec.reserve(std::min(0lu, str.size() - 1));  // abcd -> {ab, bc, cd}

        for (size_t i = 0; i < str.size() - 1; ++i)  // create bigrams
            bigram_vec.emplace_back(str[i], str[i+1]);

        std::sort(bigram_vec.begin(), bigram_vec.end());

        m_size = bigram_vec.size();
        assert(m_size > 0);  // there is at least one bigram

        auto bigram = bigram_vec.cbegin();
        m_impl.emplace_back(*bigram, 1);
        for (++bigram; bigram != bigram_vec.cend(); ++bigram) {  // unify same bigrams
            auto & last_bigram_cnt = m_impl.back();

            if (std::get<0>(last_bigram_cnt) == *bigram)
                ++std::get<1>(last_bigram_cnt);  // existing bigram, increase count
            else
                m_impl.emplace_back(*bigram, 1);
        }
    }

    /** Copy constructor */
    basic_bigrams(const basic_bigrams & ) = default;

    /** Move constructor */
    basic_bigrams(basic_bigrams && ) = default;

    /** Copy assignment */
    basic_bigrams & operator = (const basic_bigrams & ) = default;

    /**
     *  \brief  Size getter
     *
     *  \return Number of bigrams
     */
    size_t size() const { return m_size; }

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
    basic_bigrams & operator += (const basic_bigrams & other) {
        if (size() == 0)  // optimisation for empty multiset
            return *this = other;

        auto other_bigram = other.m_impl.cbegin();

        for (auto my_bigram = m_impl.begin(); my_bigram != m_impl.end(); ) {
            if (other_bigram == other.m_impl.cend())  // reached end of the other bigrams
                return *this;  // we're done

            const auto bg_cmp = cmp(*my_bigram, *other_bigram);

            if (bg_cmp < 0) {  // seek suitable position
                ++my_bigram;
                continue;
            }

            if (0 == bg_cmp) {  // bigram(s) already present, merge
                std::get<1>(*my_bigram) += std::get<1>(*other_bigram);
                ++my_bigram;
            }

            else  // position found, insert bigram
                m_impl.insert(my_bigram, *other_bigram);

            m_size += std::get<1>(*other_bigram);
            ++other_bigram;
        }

        // Append trailng bigrams if any
        for (; other_bigram != other.m_impl.cend(); ++other_bigram) {
            m_impl.push_back(*other_bigram);
            m_size += std::get<1>(*other_bigram);
        }

        return *this;
    }

    private:

    /** Union of bigrams (fixed point of template recursion) */
    static basic_bigrams unite(const basic_bigrams & arg) {
        return arg;  // make a copy
    }

    public:

    /**
     *  \brief  Union of bigrams
     *
     *  The unionis constructed right-to-left (but that's OK, set union is comutative).
     *
     *  \param  arg1  Bigram multiset
     *  \param  args  Bigram multisets
     *
     *  \return Union of bigrams
     */
    template <class ... Bigrams>
    static basic_bigrams unite(const basic_bigrams & arg1, const Bigrams & ... args) {
        return unite(args...) += arg1;
    }

    /**
     *  \brief  Union of 2 bigrams
     *
     *  \param  other  Bigram multiset
     *
     *  \return Union of bigrams
     */
    basic_bigrams operator + (const basic_bigrams & other) const {
        return unite(*this, other);
    }

    /**
     *  \brief  Bigram multisets intersection size
     *
     *  Calculate bigram multiset intersection cardinality.
     *  (The intersection is not actually constructed.)
     *
     *  \param  bigrams1  Bigram multiset
     *  \param  bigrams2  Bigram multiset
     *
     *  \return Size of the bigram multisets intersection
     */
    static size_t intersect_size(
        const basic_bigrams & bigrams1,
        const basic_bigrams & bigrams2)
    {
        size_t size = 0;

        auto bg1 = bigrams1.m_impl.cbegin();
        auto bg2 = bigrams2.m_impl.cbegin();
        while (bg1 != bigrams1.m_impl.cend() && bg2 != bigrams2.m_impl.cend()) {
            const auto bg_cmp = cmp(*bg1, *bg2);

            if (bg_cmp < 0) ++bg1;  // no match, next in bigrams1

            else if (bg_cmp == 0) {  // match, update size
                size += std::min(std::get<1>(*bg1), std::get<1>(*bg2));
                ++bg1;
                ++bg2;
            }

            else ++bg2;  // no match, next in bigrams2
        }

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
        const basic_bigrams & bigrams1,
        const basic_bigrams & bigrams2)
    {
        const auto isect_size = intersect_size(bigrams1, bigrams2);
        return isect_size ? 2.0 * isect_size / (bigrams1.size() + bigrams2.size()) : 0.0;
    }

};  // end of template class basic_bigrams


/**
 *  \brief  (Wide) bigrams serialisation
 *
 *  \tparam  Char  Character type
 *
 *  \param  out    Output stream
 *  \param  bgrms  Bigrams
 *  \param  name   Bigram class (display) name
 *
 *  \return \c out
 */
template <typename Char>
std::basic_ostream<Char> & serialise_bigrams (
    std::basic_ostream<Char> & out,
    const basic_bigrams<Char> & bgrms,
    const char * name)
{
    static const auto * left_curly_bracket = "{";

    out << name << "(size: " << bgrms.size() << ", ";

    const auto * separator = left_curly_bracket;
    for (const auto & bigram_cnt: bgrms) {
        const auto & bigram = std::get<0>(bigram_cnt);
        const auto cnt = std::get<1>(bigram_cnt);
        out << separator << std::get<0>(bigram) << std::get<1>(bigram) << ": " << cnt;
        separator = ", ";
    }
    out << (separator == left_curly_bracket ? "{}" : "}") << ')';

    return out;
}


using bigrams = basic_bigrams<char>;        /**< ASCII/ANSI string bigrams  */
using wbigrams = basic_bigrams<wchar_t>;    /**< UNICODE string bigrams     */


/** Serialisation operator */
inline std::ostream & operator << (std::ostream & out, const bigrams & bgrms) {
    return serialise_bigrams(out, bgrms, "bigrams");
}

/** Serialisation operator */
inline std::wostream & operator << (std::wostream & out, const wbigrams & bgrms) {
    return serialise_bigrams(out, bgrms, "wbigrams");
}

}  // end of namespace libsdcxx

#endif  // end of #ifndef libsdcxx__bigrams_hxx
