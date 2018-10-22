/*
* Tests for Simd Library (http://simd.sourceforge.net).
*
* Copyright (c) 2011-2016 Yermalayeu Ihar.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/
#include "Test/TestUtils.h"
#include "Test/TestPerformance.h"
#include "Test/TestData.h"

namespace Test
{
	namespace
	{
		struct Func
		{
			typedef uint32_t (*FunkPtr)(const void *src, size_t size);

			FunkPtr func;
			String description;

			Func(const FunkPtr & f, const String & d) : func(f), description(d) {}

			uint32_t Call(const std::vector<uint8_t> & src) const
			{
				TEST_PERFORMANCE_TEST(description);
				return func(src.data(), src.size());
			}
		};
	}

#define FUNC(func) Func(func, #func)

    void SetRandom(unsigned char * data, size_t size)
    {
        for(size_t i = 0; i < size; ++i)
            data[i] = Random(256);
    }

    bool Crc32AutoTest(size_t size, const Func & f1, const Func & f2)
    {
        bool result = true;

        std::vector<unsigned char> src(size);
        TEST_ALIGN(size);

        TEST_LOG_SS(Info, "Test " << f1.description << " & " << f2.description << " for size = " << size << ".");

        SetRandom(src.data(), src.size());

        uint32_t crc1, crc2;

		TEST_EXECUTE_AT_LEAST_MIN_TIME(crc1 = f1.Call(src));

		TEST_EXECUTE_AT_LEAST_MIN_TIME(crc2 = f2.Call(src));

        TEST_CHECK_VALUE(crc);

        return result;
    }

    bool Crc32AutoTest(const Func & f1, const Func & f2)
    {
        bool result = true;

        result = result && Crc32AutoTest(W*H, f1, f2);
        result = result && Crc32AutoTest(W*H + O, f1, f2);

        return result;
    }


}
