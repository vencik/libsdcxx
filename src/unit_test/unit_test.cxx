/**
 *  \file
 *  \brief  Unit test base class: implementation
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

#include "unit_test.hxx"

#include <cstddef>
#include <string>
#include <iostream>
#include <chrono>
#include <thread>
#include <stdexcept>


void unit_test::sleep_ms(double x) {
    std::this_thread::sleep_for(std::chrono::duration<double, std::milli>(x));
}


void unit_test::assert(const bool condition, const std::string & msg) {
    if (!condition)
        throw assertion_failed(msg);
}


void unit_test::seed_rng() {
    seed_rng(std::time(nullptr));
}

void unit_test::seed_rng(unsigned seed) {
    std::srand(seed);
    std::cerr << "RNG seeded by " << seed << std::endl;
}


std::vector<std::string> unit_test::init_args(int argc, char * const argv[]) {
    std::vector<std::string> args;

    args.reserve(argc);
    for (int i = 0; i < argc; ++i)
        args.emplace_back(argv[i]);

    return args;
}


unit_test::unit_test(int argc, char * const argv[]):
    m_args(unit_test::init_args(argc, argv))
{}


int unit_test::exec() const {
    int exit_code = 128;

    try {
        run();
        exit_code = 0;  // OK
    }
    catch (const assertion_failed & x) {
        std::cerr
            << "Assertion FAILED: "
            << x.what()
            << std::endl;
    }
    catch (const std::exception & x) {
        std::cerr
            << "Standard exception caught: "
            << x.what()
            << std::endl;
    }
    catch (...) {
        std::cerr
            << "Unhandled non-standard exception caught"
            << std::endl;
    }

    std::cerr << "Exit code: " << exit_code << std::endl;
    return exit_code;
}
