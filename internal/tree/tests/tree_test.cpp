#include "../../../testrunner/test_runner.h"
#include "../tree.hpp"
#include "../tree_handler.hpp"

int main() {
    // Тип записи, тип ключа, тип значения
    typedef std::tuple<std::string, std::string, std::string, int, float> RawRecordType;
    typedef nTupleUtils::TupleIndexes<0, 1, 2> KeysIndexes;
    typedef nTupleUtils::TupleIndexes<3, 4> ValuesIndexes;

    // provider - класс, который предоставляет последовательный доступ к каждой записи
    nSimpleDBFileProvider::Provider<RawRecordType, KeysIndexes, ValuesIndexes> provider("file.txt", '|');

    auto tree = Tree<RawRecordType, KeysIndexes, ValuesIndexes, decltype(provider)>(provider);


    auto neutralElement = make_tuple<int, float>(0, 0.);

    // функция для объединения внутренних значений
    auto innerJoiner = [neutralElement](const std::tuple<int, float>& lhs, const std::tuple<int, float>& rhs) {
        if (lhs == neutralElement) {
            return std::make_tuple(std::get<0>(rhs), std::get<1>(rhs));
        }
        if (rhs == neutralElement) {
            return std::make_tuple(std::get<0>(lhs), std::get<1>(lhs));
        }
        auto averagePeopleCnt = (std::get<0>(lhs) + std::get<0>(rhs));
        auto averageLifeTime = std::get<0>(lhs) * std::get<1>(lhs) / averagePeopleCnt + std::get<0>(rhs) * std::get<1>(rhs) / averagePeopleCnt;
        return std::make_tuple(averagePeopleCnt, averageLifeTime);
    };

    // функция для объединения листовых значений
    auto leafeJoiner = [neutralElement, innerJoiner](const std::tuple<int, float>& lhs, const std::tuple<int, float>& rhs) {
        if (lhs == neutralElement) {
            return std::make_tuple(std::get<0>(rhs), std::get<1>(rhs));
        }
        if (rhs == neutralElement) {
            return std::make_tuple(std::get<0>(lhs), std::get<1>(lhs));
        }
        auto toRet = innerJoiner(lhs, rhs);
        std::get<0>(toRet) = std::get<0>(toRet) / 2;
        return toRet;
    };

    std::array<std::tuple<int, float>, tree.GetLevelsCount()> arr;
    std::fill(arr.begin(), arr.end(), neutralElement);
    // сам алгоритм
    // итерируемся по вершинам дерева
    for (auto it = tree.begin(); it != tree.end(); ++it) {
        auto curLevel = it.GetLevel();
        if (it.IsLeave()) {
            arr[curLevel] = leafeJoiner(arr[curLevel], it.GetState().GetValues());
        } else {
            nTupleUtils::PrintFirstK(it.GetState().GetKeys(), curLevel, std::cout);
            std::cout << std::endl;
            nTupleUtils::PrintTuple(arr[curLevel + 1], std::cout);
            std::cout << std::endl;
            std::cout << std::endl;

            arr[curLevel] = innerJoiner(arr[curLevel], arr[curLevel + 1]);
            arr[curLevel + 1] = neutralElement;
        }
    }
}
