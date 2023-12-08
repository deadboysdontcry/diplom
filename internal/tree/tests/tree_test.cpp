#include "../../../testrunner/test_runner.h"
#include "../tree.hpp"
#include "../tree_handler.hpp"

void TestCorrectness() {
    typedef std::tuple<int, int, int, int> RowType;
    typedef nTupleUtils::TupleIndexes<0,1> KeysIndexes;
    typedef nTupleUtils::TupleIndexes<2,3> ValuesIndexes;


    nSimpleDBFileProvider::Provider<RowType, KeysIndexes, ValuesIndexes> provider("file.txt", '|');

    auto tree = Tree<RowType, KeysIndexes, ValuesIndexes, decltype(provider)>(provider);
    for(auto it = tree.begin(); it != tree.end(); ++it) {
        if (it.GetState().row.has_value()) {
            nTupleUtils::PrintTuple(it.GetState().row->GetRaw(), std::cout);
            std::cout << std::endl;
        } else {
            std::cout << it.GetState().level << std::endl;
        }
    }

    auto treeHandler = TreeHandler(tree);

    auto sumValueJoiner = [](const std::tuple<int, int>& f, const std::tuple<int, int>& s) {
        auto first = std::get<0>(f) + std::get<0>(s);
        auto second = std::get<1>(f) + std::get<1>(s);
        return std::make_tuple(first, second);
    };
    auto sumNE = std::make_tuple(0, 0);
    treeHandler.CalculateAndCollectResultsInFile("sum_result.txt", sumValueJoiner, sumNE);

    auto multValueJoiner = [](const std::tuple<int, int>& f, const std::tuple<int, int>& s) {
        auto first = std::get<0>(f) * std::get<0>(s);
        auto second = std::get<1>(f) * std::get<1>(s);
        return std::make_tuple(first, second);
    };
    auto multNE = std::make_tuple(1, 1);
    treeHandler.CalculateAndCollectResultsInFile("mult_result.txt", multValueJoiner, multNE);

    auto minValueJoiner = [](const std::tuple<int, int>& f, const std::tuple<int, int>& s) {
        auto first = std::min(std::get<0>(f), std::get<0>(s));
        auto second = std::min(std::get<1>(f),std::get<1>(s));
        return std::make_tuple(first, second);
    };
    auto minNE = std::make_tuple(1e9, 1e9);
    treeHandler.CalculateAndCollectResultsInFile("min_result.txt", minValueJoiner, minNE);
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestCorrectness);
}
