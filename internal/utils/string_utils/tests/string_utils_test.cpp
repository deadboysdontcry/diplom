#include "../../../../testrunner/test_runner.h"
#include "../string_utils.hpp"

#include <array>
#include <string>

void TestArrayFromParsedStringSimple1() {
    std::string str = "test abc test2 cba";
    std::array<std::string_view, 4> expected = {"test", "abc", "test2", "cba"};
    auto actual = nStringUtils::ParseStringIntoArray<4>(str, ' ');
    for (int i = 0; i < 4; i++) {
        ASSERT(std::string(expected[i]) == std::string(actual[i]));
    }
}

void TestArrayFromParsedStringSimple2() {
    std::string str = "test|abc|test2|cba";
    std::array<std::string_view, 4> expected = {"test", "abc", "test2", "cba"};
    auto actual = nStringUtils::ParseStringIntoArray<4>(str, '|');
    for (int i = 0; i < 4; i++) {
        ASSERT(std::string(expected[i]) == std::string(actual[i]));
    }
}

void TestFromStringArithmetic() {
    {
        int a{5};
        std::string aStr = "5";
        ASSERT_EQUAL(a, nStringUtils::GetFromString<int>(aStr));
    }
    {
        long a{5};
        std::string aStr = "5";
        ASSERT_EQUAL(a, nStringUtils::GetFromString<long>(aStr));
    }
    {
        int64_t a{5};
        std::string aStr = "5";
        ASSERT_EQUAL(a, nStringUtils::GetFromString<int64_t>(aStr));
    }
    {
        float a{5.};
        std::string aStr = "5";
        ASSERT_EQUAL(a, nStringUtils::GetFromString<float>(aStr));
    }
    {
        long double a{5.};
        std::string aStr = "5";
        ASSERT_EQUAL(a, nStringUtils::GetFromString<long double>(aStr));
    }
}

void TestFromStringStaticMethod() {
    class WithMethod {
    public:
        static WithMethod FromString(const std::string& s) {
            WithMethod w;
            w.toCheck = true;
            return w;
        }
        bool toCheck{false};
    };
    auto w = nStringUtils::GetFromString<WithMethod>(std::string(""));
    AssertEqual(w.toCheck, true);
}

void TestFromStringStream() {
    std::string expected = "expected";
    auto actual = nStringUtils::GetFromString<std::string>(expected);
    AssertEqual(expected, actual);
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestArrayFromParsedStringSimple1);
    RUN_TEST(tr, TestArrayFromParsedStringSimple2);
    RUN_TEST(tr, TestFromStringArithmetic);
    RUN_TEST(tr, TestFromStringStaticMethod);
    RUN_TEST(tr, TestFromStringStream);
}
