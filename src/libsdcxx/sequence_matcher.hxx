#ifndef libsdcxx__sequence_matcher_hxx
#define libsdcxx__sequence_matcher_hxx

/**
 *  \file
 *  \brief  Bigrams sequence smilarity using Sørensen–Dice coefficient
 *
 *  See https://en.wikipedia.org/wiki/S%C3%B8rensen%E2%80%93Dice_coefficient
 *
 *  \date   2023/02/21
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

#include <cassert>
#include <cstddef>
#include <variant>
#include <vector>
#include <unordered_set>
#include <string>
#include <iostream>

#include "bigrams.hxx"


namespace libsdcxx {

/**
 *  \brief   String sequence matching using Sørensen-Dice bigram multiset similarity
 *
 *  SDC calculated on matched string (or strings) bigram multiset vs bigram multiset
 *  of adjacent sub-sequences of provided sequence of strings' bigrams.
 *
 *  The idea is to match textual expressions to a text in a "fuzzy" manner (based on
 *  acceptable string similarity).
 *  The matcher uses Sørensen-Dice coefficient on bigram multisets as the similarity
 *  score.
 *  Bigrams (couples of adjacent characters) have the following benefits:
 *  * They still retain certain level of the string structure (unlike e.g. single
 *    characters)
 *  * They are easily computed in O(n) time (in terms of the string length)
 *  * The bigram multiset cardinality is known even without their computation
 *    (equal to size of the string minus 1)
 *
 *  The matcher operates over upper triangular matrix of the text token sequence
 *  bigram multisets.
 *  Each row contains bigram multisets unions from adjacent sub-sequences of respective
 *  length.
 *  1st row stores bigrams of individual tokens, 2nd row union of bigrams of adjacent
 *  token couples, 3rd row triplets and so on...
 *  Last row contains only one item: union of bigrams of all the tokens in the text.
 *
 *  Matching a string to the text represented by the above bigrams matrix means
 *  first calculating union of the string bigrams, then calculating SD similarity
 *  of the string bigrams and all (*necessary*, see below) bigrams of the sub-sequences
 *  in the matrix.
 *  Those sub-sequences with SDC above a chosen threshold are declared to be matches.
 *
 *  Clearly, the matching is computationally rather heavy (in the naive case).
 *  However, several optimisations may be applied to significantly decrease
 *  the computational complexity.
 *
 *  First, dynamic programming approach is used to calculate the bigrams (and their
 *  sizes) in the bigrams matrix.
 *  This is indeed necessary in order to avoid re-computations of the same bigrams
 *  (as overlapping sub-sequences clearly share them).
 *
 *  Dynamic construction of the sub-sequence bigrams matrix works as follows.
 *  In 0-based indexing, let us define the upper triangular sub-sequence bigrams matrix,
 *  thus:
 *
 *  B_{0,j} = Bigrams(T_j)  for j in 0..|T|-1 (T_j being j-th token of text T) and
 *  B_{i,j} = B_{i/2,j} + B_{i-i/2-1,j+i/2+1}  (using integer (truncated) division by 2)
 *
 *  B_{i,j} is now union of bigrams of token sequence of length i+1, starting at j.
 *
 *  Proof (by mathematical induction):
 *  Base case for B_{0,j} follows from definition (bigrams for single tokens).
 *  Induction step:
 *  For i > 0: B_{i,j} is an union of B_{i/2,j} and B_{i-i/2-1,j+i/2+1}.
 *  If i is even, i.e. i = 2k, trunc(i/2) = k.
 *  If i is odd, i.e. i = 2k + 1, trunc(i/2) = k as well.
 *  From induction predicate, B_{i/2,j} = B_{k,j} keeps bigrams union of token
 *  sequence of length k+1 starting at j.
 *  The last token in that sequence has therefore index j+k.
 *  Now, trunc(i/2) + 1 = k+1, so B_{i-i/2-1,j+i/2+1} = B_{i-k-1,j+k+1} therefore keeps
 *  bigrams union of token sequence of length i-k starting at j+k+1 (from IP again).
 *  The 1st token in the latter sequence is therefore adjacent to the last token of the
 *  former sequence indeed.
 *  Length of these 2 sequences concatenation is k+1 + i-k = i+1.
 *  Hence, we conclude that the union of B_{k,j} and B_{i-k-1,j+k+1} indeed keeps
 *  union of bigrams of token sequence of length i+1 starting at index j, QED.
 *
 *  Note that in order to calculate the size of the bigrams union, one doesn't
 *  necessarily have to construct it.
 *  As noted above, the number of token bigrams simply equals to the token size minus 1.
 *  Cardinality of union of bigram multisets is then a simple sum of sub-set
 *  cardinalities.
 *  Sizes of the bigrams in the matrix are therefore easily calculated using the very
 *  same recursive indexing descend as applies to computation of the unions above.
 *
 *  The above yields another optimisation of the match computation.
 *  Since successful match must reach certain SDC threshold, and thanks to properties
 *  of SDC calculation, we may derive an upper bound of the SDC which we can use to
 *  eliminate some bigrams union operations.
 *
 *  Derivation of the criterion for even considering SDC calculation follows:
 *
 *  For SDC(A,B) = 2|A \cap B| / (|A|+|B|)  we need an upper bound.
 *  Observe that |A \cap B| <= min{|A|,|B|}  (as the intersection may never be greater
 *  than the smaller of the sets).
 *  Also, if A = B then |A \cap B| = |A| = |B| = min{|A|,|B|}.
 *  Therefore, SDC(A,B) <= 2min{|A|,|B|} / (|A|+|B|) is the optimal upper bound for SDC
 *  for sets A and B.
 *  Let us denote it SDC_ub(A,B) from now on.
 *
 *  Now, consider a SDC threshold T, limiting acceptable SDC for a match.
 *  That is to say, SDC(A,B) >= T in order to pronounce B a match to A.
 *
 *  Clearly, SDC_ub(A,B) must also be at least T in order to even bother with SDC
 *  calculation:
 *  SDC_ub(A,B) < T => SDC(A,B) <= SDC_ub(A,B) < T  (and therefore not a match).
 *
 *  We shall infer even simpler condition for SDC(A,B) calculation consideration:
 *  Let us assume that |A| <= |B|, i.e. min{|A|,|B|} = |A|
 *  (swap A and B if it's the other way round).
 *  That gives us the requirement:
 *  T <= SDC_ub(A,B) = 2|A| / (|A|+|B|)
 *
 *  Let's simplify even further:
 *  1/T >= (|A|+|B|) / 2|A| = |A| / 2|A| + |B| / 2|A| = 1/2 + |B| / 2|A|
 *  1/T - 1/2 >= |B| / 2|A|
 *  2/T - 1 >= |B| / |A|
 *
 *  2/T - 1 is therefore upper bound for acceptable sets cardinality ratio of |B| / |A|,
 *  where |A| <= |B|.
 *  Unless the above condition holds, calculation of SDC is pointless as it would not
 *  reach T.
 *
 *  Also note that as soon as 2/T - 1 < |B| / |A|, there's no point in trying even
 *  greater B sets; that is to say, if
 *  |B1| < |B2| and 2/T - 1 < |B1| / |A| then 2/T - 1 < |B1| / |A| < |B2| / |A|  as well.
 *
 *  This effectively means that if we take B to represent the token sub-sequence and A
 *  the matched string(s), as soon as we get to the point of breaching of the upper bound
 *  condition, we may stop trying to extend the sub-sequence.
 *
 *  Another optimisation is achieved by omitting from consideration sub-sequences that
 *  begin or end with unacceptable (aka "strip") tokens.
 *  These would typically be e.g. white spaces and punctuation marks.
 *
 *  See https://en.wikipedia.org/wiki/S%C3%B8rensen%E2%80%93Dice_coefficient
 *
 *  \tparam  Bigrams  Bigram multiset implementation
 */
template <class Bigrams>
class basic_sequence_matcher {
    public:

    using bigrams_t = Bigrams;                      /**< Bigrams type   */
    using char_t = typename bigrams_t::char_t;      /**< Character type */
    using string_t = std::basic_string<char_t>;     /**< String type    */

    private:

    /** Bigram multiset matrix cell */
    class mx_cell {
        public:

        /** Type of value currently stored in bigram multiset matrix cell */
        enum val_type_t {
            NONE = 0,   /**< No value set yet   */
            SIZE,       /**< Size only          */
            BIGRAMS,    /**< Bigrams and size   */
        };

        private:

        /** Value currently stored in bigram multiset matrix cell */
        using val_t = std::variant<size_t, bigrams_t>;

        val_type_t m_type;  /**< Value type */
        val_t m_value;      /**< Value      */

        public:

        mx_cell(): m_type(NONE) {}

        /** Bigrams matrix cell from existing bigrams */
        mx_cell(const bigrams_t & bgrms): m_type(BIGRAMS), m_value(bgrms) {}

        /** Bigrams matrix cell from moved bigrams */
        mx_cell(bigrams_t && bgrms): m_type(BIGRAMS), m_value(bgrms) {}

        /** Move constructor */
        mx_cell(mx_cell && ) = default;

        /** Move assignment operator */
        mx_cell & operator = (mx_cell && ) = default;

        val_type_t content() const { return m_type; }

        public:

        void set_size(size_t size) {
            assert(m_type == NONE);  // only done once
            m_value = size;
            m_type = SIZE;
        }

        size_t size() const {
            assert(m_type != NONE);  // size must be set
            return m_type == SIZE
                ? std::get<size_t>(m_value)
                : std::get<bigrams_t>(m_value).size();
        }

        void set_bigrams(bigrams_t && bgrms) {
            assert(m_type != BIGRAMS);  // only done once
            m_value = bgrms;
            m_type = BIGRAMS;
        }

        const bigrams_t & bigrams() const {
            assert(m_type == BIGRAMS);
            return std::get<bigrams_t>(m_value);
        }

    };  // end of class mx_cell

    using row_t = std::vector<mx_cell>;     /**< Bigram multiset matrix row type    */
    using matrix_t = std::vector<row_t>;    /**< Bigram multiset matrix type        */

    using ix_set_t = std::unordered_set<size_t>;    /**< Set of indices */

    matrix_t m_mx;          /**< Bigram multiset (or multiset size) matrix      */
    ix_set_t m_strip_ixs;   /**< Set of "strip" strings indices in the sequence */

    public:

    class iterator {
        friend class basic_sequence_matcher;

        private:

        static const bigrams_t s_empty_bigrams;     /**< Empty bigrams */

        enum end_t {
            END     /**< End iterator */
        };

        basic_sequence_matcher & m_matcher;  /**< Matcher                           */
        const bigrams_t & m_bigrams;         /**< Matched string(s) bigrams         */
        double m_sdc_threshold;              /**< Sørensen-Dice coef. threshold     */
        double m_card_ratio_threshold;       /**< Bigrams cardinality ratio thresh. */
        size_t m_i, m_j;                     /**< Bigrams matrix row & col. indices */
        double m_sdc;                        /**< Sørensen-Dice coef. at [i,j]      */

        private:

        /**
         *  \brief  Constructor (construction is reserved to the matcher)
         *
         *  \param  matcher    Sequence matcher
         *  \param  bgrms      Matched string(s) bigram multiset
         *  \param  threshold  Match Sørensen-Dice coefficient threshold
         *  \param  i          Beginning bigrams matrix row index
         *  \param  j          Beginning bigrams matrix column index
         */
        iterator(
            basic_sequence_matcher & matcher,
            const bigrams_t & bgrms,
            double threshold,
            size_t i = 0, size_t j = 0)
        :
            m_matcher(matcher),
            m_bigrams(bgrms),
            m_sdc_threshold(threshold),
            m_card_ratio_threshold(2.0 / threshold - 1.0),
            m_i(i), m_j(j), m_sdc(0.0)
        {
            next_match();  // find 1st match
        }

        iterator(basic_sequence_matcher & matcher, end_t ):
            iterator(matcher, s_empty_bigrams, 0.0, 0, matcher.size())
        {}

        /** Shift to next matching sub-sequence */
        void next_match() {
            for (; m_j < m_matcher.size(); ++m_j) {
                // Skip sub-sequence beginning with "strip" string
                if (m_matcher.m_strip_ixs.count(m_j)) continue;

                for (; m_i < m_matcher.size() - m_j; ++m_i) {
                    // Skip sub-sequence ending with "strip" string
                    if (m_matcher.m_strip_ixs.count(m_j + m_i)) continue;

                    // Check cardinality ratio
                    double card_ratio =
                        static_cast<double>(m_matcher.bigrams_size(m_i, m_j)) /
                        static_cast<double>(m_bigrams.size());

                    bool subseq_short = card_ratio < 1.0;  // sub-sequence is shorter

                    // Make sure we take bigger / smaller ratio
                    if (subseq_short) card_ratio = 1.0 / card_ratio;

                    if (card_ratio > m_card_ratio_threshold) {  // SDC would be too small
                        if (subseq_short) continue;  // try longer sub-sequence

                        break;  // sub-sequence got too long; no point in extending it
                    }

                    // Only now it's necessary to calculate SDC
                    m_sdc = bigrams_t::sorensen_dice_coef(
                        m_matcher.bigrams(m_i, m_j), m_bigrams);
                    if (m_sdc < m_sdc_threshold) continue;  // still not up to scratch

                    return;  // match found
                }

                m_i = 0;  // reset row index
            }
        }

        public:

        /** Copy constructor */
        iterator(const iterator & ) = default;

        /** Move constructor */
        iterator(iterator && ) = default;

        /** Copy assignment operator */
        iterator & operator = (const iterator & ) = default;

        /**
         *  \brief  Dereference
         *
         *  \return Matching sub-sequence bigram multiset
         */
        const bigrams_t & operator * () const { return m_matcher.bigrams(m_i, m_j); }

        /** Matching sub-sequence size (number of strings in the sub-sequence) */
        size_t size() const { return m_i + 1; }

        /** Matching sub-sequence begin (index of the 1st string in the sub-sequence) */
        size_t begin() const { return m_j; }

        /** Matching sub-sequence end (just past the last string index) */
        size_t end() const { return begin() + size(); }

        /** Match score (Sørensen-Dice coefficient) */
        double sorensen_dice_coef() const { return m_sdc; }

        /** Shift to next matching sub-sequence */
        iterator & operator ++ () {
            ++m_i;  // update row index to shift from current match
            next_match();  // seek next match
            return *this;
        }

        /** Comparison (eq) */
        bool operator == (const iterator & other) const {
            return this->m_i == other.m_i && this->m_j == other.m_j;
        }

        /** Comparison (ne) */
        bool operator != (const iterator & other) const { return !(*this == other); }

        /** Comparison (lt) */
        bool operator < (const iterator & other) const {
            if (this->m_j < other.m_j) return true;
            if (this->m_j > other.m_j) return false;
            return this->m_i < other.m_i;
        }

        /** Comparison (gt) */
        bool operator > (const iterator & other) const { return other < *this; }

        /** Comparison (le) */
        bool operator <= (const iterator & other) const { return !(*this > other); }

        /** Comparison (ge) */
        bool operator >= (const iterator & other) const { return !(*this < other); }

    };  // end of class iterator

    /** Default constructor */
    basic_sequence_matcher() = default;

    /** Reserve space for sequence */
    void reserve(size_t len) {
        m_mx.reserve(len);
        m_strip_ixs.reserve(len);
    }

    /** Copy constructor (copying is forbidden) */
    basic_sequence_matcher(const basic_sequence_matcher & ) = delete;

    /** Move constructor */
    basic_sequence_matcher(basic_sequence_matcher && ) = default;

    /** Copy assignment (copying is forbidden) */
    basic_sequence_matcher & operator = (const basic_sequence_matcher & ) = delete;

    /**
     *  \brief  Size getter
     *
     *  \return Sequence length
     */
    size_t size() const { return m_mx.size(); }

    private:

    /** Extend bigram multiset matrix for another sequence member */
    void add_row() {
        size_t back = size();
        m_mx.emplace_back();
        if (m_mx.capacity() > back)  // matrix size was reserved
            m_mx[back].reserve(m_mx.capacity() - back);
    }

    public:

    /**
     *  \brief  Push another string bigram multiset to sequence
     *
     *  \param  bgrms  Bigram multiset
     *  \param  strip  This is a "strip" string (not a begin/end of valid sub-sequence)
     */
    void push_back(const bigrams_t & bgrms, bool strip = false) {
        if (strip) m_strip_ixs.insert(size());
        add_row();
        m_mx[0].emplace_back(bgrms);
        for (size_t i = 1; i < size(); ++i) m_mx[i].emplace_back();
    }

    /**
     *  \brief  Push another string bigram multiset to sequence
     *
     *  \param  bgrms  Bigram multiset
     *  \param  strip  This is a "strip" string (not a begin/end of valid sub-sequence)
     */
    void push_back(bigrams_t && bgrms, bool strip = false) {
        if (strip) m_strip_ixs.insert(size());
        add_row();
        m_mx[0].emplace_back(bgrms);
        for (size_t i = 1; i < size(); ++i) m_mx[i].emplace_back();
    }

    /**
     *  \brief  Construct another string bigram multiset at end of sequence
     *
     *  \param  str    Another string in the sequence
     *  \param  strip  This is a "strip" string (not a begin/end of valid sub-sequence)
     */
    void emplace_back(const string_t & str, bool strip = false) {
        push_back(bigrams_t(str), strip);
    }

    /**
     *  \brief  Begin matching
     *
     *  Match iterator provides matches in ascending lexicographic order by their begin
     *  and size.
     *
     *  \param  bgrms       Bigram multiset
     *  \param  threshold   Sørensen-Dice coefficient (match score) threshold
     *
     *  \return Match iterator
     */
    iterator begin(const bigrams_t & bgrms, double threshold) {
        return iterator(*this, bgrms, threshold);
    }

    /** End iterator (all possible matches iterated) */
    iterator end() { return iterator(*this, iterator::END); }

    private:

    /**
     *  \brief  Get indices of sub-bigrams
     *
     *  \param  i   Bigrams row index
     *  \param  j   Bigrams column index
     *  \param  i1  1st sub-bigrams row index
     *  \param  j1  1st sub-bigrams column index
     *  \param  i2  1st sub-bigrams row index
     *  \param  j2  1st sub-bigrams column index
     */
    static void sub_ix(
        size_t i, size_t j,
        size_t & i1, size_t & j1,
        size_t & i2, size_t & j2)
    {
        i1 = i / 2;
        j1 = j;

        i2 = i1 + 1;
        j2 = j + i2;
        i2 = i - i2;
    }

    /**
     *  \brief  Bigrams size
     *
     *  \param  i  Row index
     *  \param  j  Column index
     *
     *  \return Size of bigrams at [i,j]
     */
    size_t bigrams_size(size_t i, size_t j) {
        assert(i < size());
        auto & row = m_mx[i];

        assert(j < row.size());
        auto & cell = row[j];

        if (cell.content() != mx_cell::NONE) return cell.size();  // size already known

        size_t i1, j1, i2, j2; sub_ix(i, j, i1, j1, i2, j2);
        size_t result = bigrams_size(i1, j1) + bigrams_size(i2, j2);

        cell.set_size(result);  // cache for the next time
        return result;
    }

    /**
     *  \brief  Bigrams getter
     *
     *  \param  i  Row index
     *  \param  j  Column index
     *
     *  \return Bigrams at [i,j]
     */
    const bigrams_t & bigrams(size_t i, size_t j) {
        assert(i < size());
        auto & row = m_mx[i];

        assert(j < row.size());
        auto & cell = row[j];

        if (cell.content() != mx_cell::BIGRAMS) {  // bigrams not computed yet
            size_t i1, j1, i2, j2; sub_ix(i, j, i1, j1, i2, j2);
            cell.set_bigrams(bigrams(i1, j1) + bigrams(i2, j2));
        }

        return cell.bigrams();
    }

};  // end of template class basic_sequence_matcher

template <class Bigrams>
const Bigrams basic_sequence_matcher<Bigrams>::iterator::s_empty_bigrams;


/**
 *  \brief  Sequence matcher iterator (i.e. match) serialisation
 *
 *  \tparam  Char     Character type
 *  \tparam  Bigrams  Bigrams type (not really used)
 *
 *  \param  out    Output stream
 *  \param  match  Match iterator
 */
template <typename Char, class Bigrams>
void serialise_match (
    std::basic_ostream<Char> & out,
    const typename basic_sequence_matcher<Bigrams>::iterator & match)
{
    out << "match(begin: " << match.begin()
        << ", end: " << match.end()
        << ", size: " << match.size()
        << ", SDC: " << match.sorensen_dice_coef()
        //<< ", " << *match
        << ")";
}


/**< ASCII/ANSI string sequence matcher */
using sequence_matcher = basic_sequence_matcher<bigrams>;

/** Match serialisation operator */
template <typename Char>
std::basic_ostream<Char> & operator << (
    std::basic_ostream<Char> & out,
    const sequence_matcher::iterator & match)
{
    serialise_match<Char, sequence_matcher::bigrams_t>(out, match);
    return out;
}

/**< UNICODE string sequence matcher */
using wsequence_matcher = basic_sequence_matcher<wbigrams>;

/** Match serialisation operator */
template <typename Char>
std::basic_ostream<Char> & operator << (
    std::basic_ostream<Char> & out,
    const wsequence_matcher::iterator & match)
{
    serialise_match<Char, wsequence_matcher::bigrams_t>(out, match);
    return out;
}

}  // end of namespace libsdcxx

#endif  // end of #ifndef libsdcxx__sequence_matcher_hxx
