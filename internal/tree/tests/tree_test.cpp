#include "../../../testrunner/test_runner.h"
#include "../tree.hpp"

void TestCorrectness() {
    typedef std::tuple<int, int, int, int> RowType;
    typedef nTupleUtils::TupleIndexes<0,1> KeysIndexes;
    typedef nTupleUtils::TupleIndexes<2, 3> ValuesIndexes;

    auto keyPred = [](std::tuple<int, int> t) {
        return std::get<0>(t) < 3;
    };

    auto valueJoiner = [](const std::tuple<int, int>& f, const std::tuple<int, int>& s) {
        auto first = std::get<0>(f) + std::get<0>(s);
        auto second = std::get<1>(f) + std::get<1>(s);
        return std::make_tuple(first, second);
    };

    auto ne = std::make_tuple(0, 0);

    auto tree = Tree<RowType, KeysIndexes, ValuesIndexes>("file.txt");
    auto res = tree.Calculate(keyPred, valueJoiner, ne);
    
    ASSERT_EQUAL(std::get<0>(res), 9);
    ASSERT_EQUAL(std::get<1>(res), 12);
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestCorrectness);
}
