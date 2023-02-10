/**
 *  \file
 *  \brief  Sørensen–Dice coefficient on multisets of bigrams: implementations
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

#include "bigrams.hxx"


namespace libsdcxx {

std::ostream & operator << (std::ostream & out, const bigrams & bgrms) {
    static const auto * left_curly_bracket = "{";

    out << "bigrams(size: " << bgrms.size() << ", ";

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


std::wostream & operator << (std::wostream & out, const wbigrams & wbgrms) {
    static const auto * left_curly_bracket = L"{";

    out << L"wbigrams(size: " << wbgrms.size() << L", ";

    const auto * separator = left_curly_bracket;
    for (const auto & bigram_cnt: wbgrms) {
        const auto & bigram = std::get<0>(bigram_cnt);
        const auto cnt = std::get<1>(bigram_cnt);
        out << separator << std::get<0>(bigram) << std::get<1>(bigram) << L": " << cnt;
        separator = L", ";
    }
    out << (separator == left_curly_bracket ? L"{}" : L"}") << L')';

    return out;
}

}  // end of namespace libsdcxx
