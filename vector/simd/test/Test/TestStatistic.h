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
		struct Func1
		{
			typedef void (*FuncPtr)(const uint8_t *src, size_t stride, size_t width, size_t height, 
				 uint8_t * min, uint8_t * max, uint8_t * average);

			FuncPtr func;
			String description;

			Func1(const FuncPtr & f, const String & d) : func(f), description(d) {}

			void Call(const View & src, uint8_t * min, uint8_t * max, uint8_t * average) const
			{
				TEST_PERFORMANCE_TEST(description);
				func(src.data, src.stride, src.width, src.height, min, max, average);
			}
		};
	}

#define FUNC1(function) Func1(function, #function)

	bool GetStatisticAutoTest(int width, int height, const Func1 & f1, const Func1 & f2)
	{
		bool result = true;

		TEST_LOG_SS(Info, "Test " << f1.description << " & " << f2.description << " [" << width << ", " << height << "].");

		View src(width, height, View::Gray8, NULL, TEST_ALIGN(width));
		FillRandom(src);

		 uint8_t min1, max1, average1;
		 uint8_t min2, max2, average2;

		TEST_EXECUTE_AT_LEAST_MIN_TIME(f1.Call(src, &min1, &max1, &average1));

		TEST_EXECUTE_AT_LEAST_MIN_TIME(f2.Call(src, &min2, &max2, &average2));

        TEST_CHECK_VALUE(min);
        TEST_CHECK_VALUE(max);
        TEST_CHECK_VALUE(average);

		return result;
	}

    bool GetStatisticAutoTest(const Func1 & f1, const Func1 & f2)
    {
        bool result = true;

        result = result && GetStatisticAutoTest(W, H, f1, f2);
        result = result && GetStatisticAutoTest(W + O, H - O, f1, f2);
        result = result && GetStatisticAutoTest(W - O, H + O, f1, f2);

        return result;
    }


    namespace
    {
        struct FuncM
        {
            typedef void (*FuncPtr)(const uint8_t * mask, size_t stride, size_t width, size_t height, uint8_t index, 
                uint64_t * area, uint64_t * x, uint64_t * y, uint64_t * xx, uint64_t * xy, uint64_t * yy);

            FuncPtr func;
            String description;

            FuncM(const FuncPtr & f, const String & d) : func(f), description(d) {}

            void Call(const View & mask, uint8_t index, uint64_t * area, uint64_t * x, uint64_t * y, uint64_t * xx, uint64_t * xy, uint64_t * yy) const
            {
                TEST_PERFORMANCE_TEST(description);
                func(mask.data, mask.stride, mask.width, mask.height, index, area, x, y, xx, xy, yy);
            }
        };
    }

#define FUNC_M(function) FuncM(function, #function)

#define ARGS_M(scale, function1, function2) \
    FuncM(function1.func, function1.description + ScaleDescription(scale)), \
    FuncM(function2.func, function2.description + ScaleDescription(scale)) 

    bool GetMomentsAutoTest(ptrdiff_t width, ptrdiff_t height, const FuncM & f1, const FuncM & f2)
    {
        bool result = true;

        TEST_LOG_SS(Info, "Test " << f1.description << " & " << f2.description << " [" << width << ", " << height << "].");

        const uint8_t index = 7;
        View mask(width, height, View::Gray8, NULL, TEST_ALIGN(width));
        FillRandomMask(mask, index);

        uint64_t area1, x1, y1, xx1, xy1, yy1;
        uint64_t area2, x2, y2, xx2, xy2, yy2;

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f1.Call(mask, index, &area1, &x1, &y1, &xx1, &xy1, &yy1));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f2.Call(mask, index, &area2, &x2, &y2, &xx2, &xy2, &yy2));

        TEST_CHECK_VALUE(area);
        TEST_CHECK_VALUE(x);
        TEST_CHECK_VALUE(y);
        TEST_CHECK_VALUE(xx);
        TEST_CHECK_VALUE(xy);
        TEST_CHECK_VALUE(yy);

        return result;
    }

    bool GetMomentsAutoTest(const Point & scale, const FuncM & f1, const FuncM & f2)
    {
        bool result = true;

        result = result && GetMomentsAutoTest(W*scale.x, H*scale.y, ARGS_M(scale, f1, f2));
        result = result && GetMomentsAutoTest(W*scale.x + O, H*scale.y - O, ARGS_M(scale, f1, f2));
        result = result && GetMomentsAutoTest(W*scale.x - O, H*scale.y + O, ARGS_M(scale, f1, f2));

        return result;
    }

    bool GetMomentsAutoTest(const FuncM & f1, const FuncM & f2)
    {
        bool result = true;

        result = result && GetMomentsAutoTest(Point(1, 1), f1, f2);
#ifdef NDEBUG
        result = result && GetMomentsAutoTest(Point(5, 2), f1, f2);
#else
        result = result && GetMomentsAutoTest(Point(50, 20), f1, f2);
#endif

        return result;
    }


    namespace
    {
        struct Func3
        {
            typedef void (*FuncPtr)(const uint8_t * src, size_t stride, size_t width, size_t height, uint32_t * sums);

            FuncPtr func;
            String description;

            Func3(const FuncPtr & f, const String & d) : func(f), description(d) {}

            void Call(const View & src, uint32_t * sums) const
            {
                TEST_PERFORMANCE_TEST(description);
                func(src.data, src.stride, src.width, src.height, sums);
            }
        };
    }

#define FUNC3(function) Func3(function, #function)

    bool GetSumsAutoTest(int width, int height, const Func3 & f1, const Func3 & f2, bool isRow)
    {
        bool result = true;

        TEST_LOG_SS(Info, "Test " << f1.description << " & " << f2.description << " [" << width << ", " << height << "].");

        View src(width, height, View::Gray8, NULL, TEST_ALIGN(width));
        FillRandom(src);

        size_t size = isRow ? height : width;
        Sums sums1(size, 0);
        Sums sums2(size, 0);

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f1.Call(src, sums1.data()));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f2.Call(src, sums2.data()));

        result = result && Compare(sums1, sums2, 0, true, 32);

        return result;
    }

    bool GetSumsAutoTest(const Func3 & f1, const Func3 & f2, bool isRow)
    {
        bool result = true;

        result = result && GetSumsAutoTest(W, H, f1, f2, isRow);
        result = result && GetSumsAutoTest(W + O, H - O, f1, f2, isRow);
        result = result && GetSumsAutoTest(W - O, H + O, f1, f2, isRow);

        return result;
    }

 
    namespace
    {
        struct Func4
        {
            typedef void (*FuncPtr)(const uint8_t * src, size_t stride, size_t width, size_t height, uint64_t * sum);

            FuncPtr func;
            String description;

            Func4(const FuncPtr & f, const String & d) : func(f), description(d) {}

            void Call(const View & src, uint64_t * sum) const
            {
                TEST_PERFORMANCE_TEST(description);
                func(src.data, src.stride, src.width, src.height, sum);
            }
        };
    }

#define FUNC4(function) Func4(function, #function)

    bool SumAutoTest(int width, int height, const Func4 & f1, const Func4 & f2)
    {
        bool result = true;

        TEST_LOG_SS(Info, "Test " << f1.description << " & " << f2.description << " [" << width << ", " << height << "].");

        View src(width, height, View::Gray8, NULL, TEST_ALIGN(width));
        FillRandom(src);

        uint64_t sum1;
        uint64_t sum2;

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f1.Call(src, &sum1));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f2.Call(src, &sum2));

        TEST_CHECK_VALUE(sum);

        return result;
    }

    bool SumAutoTest(const Func4 & f1, const Func4 & f2)
    {
        bool result = true;

        result = result && SumAutoTest(W, H, f1, f2);
        result = result && SumAutoTest(W + O, H - O, f1, f2);
        result = result && SumAutoTest(W - O, H + O, f1, f2);

        return result;
    }


    namespace
    {
        struct Func5
        {
            typedef void (*FuncPtr)(const uint8_t * a, size_t aStride, const uint8_t * b, size_t bStride, size_t width, size_t height, uint64_t * sum);

            FuncPtr func;
            String description;

            Func5(const FuncPtr & f, const String & d) : func(f), description(d) {}

            void Call(const View & a, const View & b, uint64_t * sum) const
            {
                TEST_PERFORMANCE_TEST(description);
                func(a.data, a.stride, b.data, b.stride, a.width, a.height, sum);
            }
        };
    }

#define FUNC5(function) Func5(function, #function)

    bool CorrelationSumAutoTest(int width, int height, const Func5 & f1, const Func5 & f2)
    {
        bool result = true;

        TEST_LOG_SS(Info, "Test " << f1.description << " & " << f2.description << " [" << width << ", " << height << "].");

        View a(width, height, View::Gray8, NULL, TEST_ALIGN(width));
        FillRandom(a);
        View b(width, height, View::Gray8, NULL, TEST_ALIGN(width));
        FillRandom(b);

        uint64_t sum1;
        uint64_t sum2;

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f1.Call(a, b, &sum1));

        TEST_EXECUTE_AT_LEAST_MIN_TIME(f2.Call(a, b, &sum2));

        TEST_CHECK_VALUE(sum);

        return result;
    }

    bool CorrelationSumAutoTest(const Func5 & f1, const Func5 & f2)
    {
        bool result = true;

        result = result && CorrelationSumAutoTest(W, H, f1, f2);
        result = result && CorrelationSumAutoTest(W + O, H - O, f1, f2);
        result = result && CorrelationSumAutoTest(W - O, H + O, f1, f2);

        return result;
    }


}
