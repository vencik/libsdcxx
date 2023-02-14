/**
 *  \file
 *  \brief  Bigrams unit test
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

#include <libsdcxx/bigram_multiset.hxx>

#include "unit_test.hxx"


/** Bigrams unit test */
class test_bigram_multiset: public unit_test {
    private:

    using bigram_multiset = libsdcxx::bigram_multiset;
    using wbigram_multiset = libsdcxx::wbigram_multiset;

    public:

    test_bigram_multiset(int argc, char * const argv[]): unit_test(argc, argv) {}

    /** Run unit test */
    void run() const {
        const auto bgrms = bigram_multiset();
        std::cout << "bigram_multiset() == " << bgrms << std::endl;
        assert(bgrms.size() == 0, "Empty bigrams have size 0");

        const auto bgrms_abcd = bigram_multiset("abcd");
        std::cout << "bigrams(\"abcd\") == " << bgrms_abcd << std::endl;
        assert(bgrms_abcd.size() == 3, "abcd -> |{ab, bc, cd}| == 3");

        const auto bgrms_bcd = bigram_multiset("bcd");
        std::cout << "bigram_multiset(\"bcd\") == " << bgrms_bcd << std::endl;
        assert(bgrms_bcd.size() == 2, "bcd -> |{bc, cd}| == 2");

        const auto bgrms_abcd_bcd = bigram_multiset::unite(bgrms_abcd, bgrms_bcd);
        std::cout
            << "bigram_multiset::unite(bigram_multiset(\"abcd\"), bigram_multiset(\"bcd\")) == "
            << bgrms_abcd_bcd << std::endl;
        assert(bgrms_abcd_bcd.size() == 5, "|{ab, bc, cd} + {bc, cd}| == 3");

        const auto isect_size = bigram_multiset::intersect_size(bgrms_abcd, bgrms_bcd);
        std::cout
            << "bigram_multiset::intersect_size(bigram_multiset(\"abcd\"), bigram_multiset(\"bcd\")) == "
            << isect_size << std::endl;
        assert(isect_size == 2, "|intersection({ab, bc, cd}, {bc, cd})| == 2");

        const auto sdc = bigram_multiset::sorensen_dice_coef(bgrms_abcd, bgrms_bcd);
        std::cout
            << "bigram_multiset::sorensen_dice_coef(bigram_multiset(\"abcd\"), bigram_multiset(\"bcd\")) == "
            << sdc << std::endl;
        assert(sdc == 0.8, "SDC({ab, bc, cd}, {bc, cd}) == 2 * 2 / (3 + 2) == 4/5");

        const auto wbgrms = wbigram_multiset(L"S\u00f8rensen");
        std::wcout << L"wbigram_multiset(\"S\u00f8rensen\") == " << wbgrms << std::endl;
        assert(wbgrms.size() == 7, "|{So, or, re, en, ns, se, en}| == 7");
    }

};  // end of class test_bigram_multiset


int main(int argc, char * const argv[]) {
    return test_bigram_multiset(argc, argv).exec();
}
