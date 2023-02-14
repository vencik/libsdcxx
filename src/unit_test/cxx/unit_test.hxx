#ifndef unit_test_hxx
#define unit_test_hxx

/**
 *  \file
 *  \brief  Unit test base class
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

#include <stdexcept>
#include <vector>
#include <string>

#undef assert


/** Unit test */
class unit_test {
    protected:

    /** UT assertion failed */
    class assertion_failed: public std::logic_error {
        public:

        assertion_failed(const std::string & msg): std::logic_error(msg.c_str()) {}

    };  // end of class assertion_failed

    /** Sleep for \c x milliseconds */
    static void sleep_ms(double x);

    /** UT assertion (throws \c unit_test::assertion_failed) */
    static void assert(const bool condition, const std::string & msg);

    /** Seed random number generator by current time (in seconds since the Epoch) */
    static void seed_rng();

    /** Seed random number generator  by \c seed */
    static void seed_rng(unsigned seed);

    const std::vector<std::string> m_args;  /**< command line arguments */

    private:

    /** \c m_args initialiser */
    static std::vector<std::string> init_args(int argc, char * const argv[]);

    public:

    /** Constructor (from command line arguments) */
    unit_test(int argc, char * const argv[]);

    /** Run unit test */
    virtual void run() const = 0;

    /**
     *  \brief  Exception-safe wrapper around \c run
     *
     *  The intended usage is to define the UT program \c main function as such:
     *
     *  ```
     *  int main(int argc, char * const argv[]) {
     *      // my_test is derived from unit_test by implementing its run method
     *      return my_test(argc, argv).exec();
     *  }
     *  ```
     *
     *  \return Unit test exit code
     */
    int exec() const;

};  // end of class bigrams_test

#endif  // end of #ifndef unit_test_hxx
