/**
 *  \file
 *  \brief  Sequence matcher unit test
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

#include <libsdcxx/sequence_matcher.hxx>
#include <iostream>

#include "unit_test.hxx"


/** Sequence matcher unit test */
class test_sequence_matcher: public unit_test {
    private:

    using sequence_matcher = libsdcxx::sequence_matcher;
    using bigrams = sequence_matcher::bigrams_t;

    using wsequence_matcher = libsdcxx::wsequence_matcher;
    using wbigrams = wsequence_matcher::bigrams_t;

    public:

    test_sequence_matcher(int argc, char * const argv[]): unit_test(argc, argv) {}

    /** Run unit test */
    void run() const {
        auto matcher = sequence_matcher();
        assert(matcher.begin(bigrams(), 0.0) == matcher.end(),
            "No matches possible on empty text");

        const auto bgrms_hello = bigrams("Hello");
        const auto bgrms_space = bigrams("  ");     // 2 spaces to produce a bigram
        const auto bgrms_world = bigrams("world");
        const auto bgrms_xm = bigrams(" !");        // ditto

        matcher.reserve(9);                     // text of 9 tokens
        matcher.emplace_back("Prologue");
        matcher.emplace_back(" .", true);       // strip token
        matcher.emplace_back("  ", true);       // strip token
        matcher.push_back(bgrms_hello);
        matcher.push_back(bgrms_space, true);   // strip token
        matcher.push_back(bgrms_world);
        matcher.push_back(bgrms_xm, true);      // strip token
        matcher.emplace_back("Epilogue");
        matcher.emplace_back(" .");             // strip token

        const auto bgrms_helo_wordl = bigrams::unite(
            bigrams("Helo"), bigrams("  "), bigrams("wordl"));

        auto match = matcher.begin(bgrms_helo_wordl, 0.7);

        //for (; match != matcher.end(); ++match)
        //    std::cout << "!!!" << match << std::endl;

        assert(match != matcher.end(), "Matched");
        assert(match.begin() == 3, "Match begins with \"Hello\"");
        assert(match.size() == 3, "Match is 3 tokens long");
        assert(match.end() == 6, "Match ends past \"world\"");
        assert(match.sorensen_dice_coef() > 0.7, "Matches with SDC above threshold");

        ++match;
        assert(match == matcher.end(), "No more matches");
    }

};  // end of class test_sequence_matcher


int main(int argc, char * const argv[]) {
    return test_sequence_matcher(argc, argv).exec();
}
