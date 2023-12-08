#include "../../../../testrunner/test_runner.h"
#include "../tuple_utils.hpp"

#include <array>
#include <string>
#include <tuple>

struct TempStruct {
    static TempStruct FromString(const std::string& s) {
        TempStruct t;
        t.str = s;
        return t;
    }   
    friend bool operator==(const TempStruct& t1, const TempStruct& t2) {
        return t1.str == t2.str;
    }
    std::string str; 
};

void TestParseIntoTuple() {
    std::string s = "abc lol 1";
    auto expected = make_tuple(std::string("abc"), std::string("lol"), 1);
    auto t = typename nTupleUtils::IndexTupleBuilder<4>::Indexes();
    auto actual = nTupleUtils::ParseIntoTuple<decltype(expected)>(s, ' ');
    ASSERT(actual == expected)
    s = "a|1|abc";
    auto expected2 = make_tuple(std::string("a"), 1, TempStruct{"abc"});
    auto actual2 = nTupleUtils::ParseIntoTuple<decltype(expected2)>(s, '|');
    ASSERT(actual2 == expected2);
}

void TestCutByIndexes() {
    auto t = std::make_tuple("a", 1, 2, 1.0);
    auto expected1 = std::make_tuple(1,2);
    auto expected2 = std::make_tuple("a", 1.0);
    auto actual1 = nTupleUtils::CutByIndexes<nTupleUtils::TupleIndexes<1,2>>::Do(t);
    auto actual2 = nTupleUtils::CutByIndexes<nTupleUtils::TupleIndexes<0,3>>::Do(t);
    ASSERT(actual1 == expected1);
    ASSERT(actual2 == expected2);
}

void TestGetFirstNEIdx() {
    auto f1 = make_tuple(1,2,3), f2 = make_tuple(1,2,1);
    auto idx = nTupleUtils::GetFirstNEIdx(f1, f2);
    ASSERT(idx = 2);
}

void TestPrintFirstK() {
    auto t = std::make_tuple("a", 1, 2, 1.0);
    nTupleUtils::PrintFirstK(t, 2, std::cout);
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestParseIntoTuple);
    RUN_TEST(tr, TestCutByIndexes);
    RUN_TEST(tr, TestGetFirstNEIdx);
    RUN_TEST(tr, TestPrintFirstK);
}