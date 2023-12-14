#include "../../../../testrunner/test_runner.h"
#include "../simple_file_provider.hpp"

void TestCompile() {
    typedef std::tuple<int, int, int, std::string> RowType;
    typedef nTupleUtils::TupleIndexes<0, 1> KeysIndexes;
    typedef nTupleUtils::TupleIndexes<2, 3> ValuesIndexes;

    auto prov = nSimpleDBFileProvider::Provider<RowType, KeysIndexes, ValuesIndexes>("file.txt", '|');
    std::vector<int>::iterator t;
    while (prov.HasNext()) {
        auto row = prov.GetNextRow();
        auto keys = row->GetKeys();
        std::cout << std::get<0>(keys) << " " << std::get<1>(keys) << " ";
        auto values = row->GetValues();
        std::cout << std::get<0>(values) << " " << std::get<1>(values) << " ";
        std::cout << std::endl;
        bool isSame = std::is_same_v<decltype(keys), typename nSimpleDBFileProvider::Row<RowType, KeysIndexes, ValuesIndexes>::KeysType>;
        ASSERT(isSame);
    }
}

int main() {
    TestRunner tr;
    RUN_TEST(tr, TestCompile);
}
