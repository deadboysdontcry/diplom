#pragma once

#include "tree.hpp"
#include "../db_providers/provider.hpp"
#include <stdexcept>

template <typename Tuple, typename KeysIndexes, typename ValuesIndexes, ProviderConcept<Tuple, KeysIndexes, ValuesIndexes> Provider>
class TreeHandler {
private:
    typedef typename Tree<Tuple, KeysIndexes, ValuesIndexes, Provider>::ValueType ValueType;

public:
    TreeHandler(Tree<Tuple, KeysIndexes, ValuesIndexes, Provider>& t)
        : tree(t)
    {
    }

    template <typename InnerJoiner>
    void CalculateAndPrintResults(std::ostream& out, const InnerJoiner& ij, const ValueType& ne) {
        std::array<std::tuple<int, float>, tree.GetLevelsCount()> arr;
        for (auto it = tree.begin(); it != tree.end(); ++it) {
            auto curLevel = it.GetState().level;
            if (it.IsLeave()) {
                arr[curLevel] = ij(arr[curLevel], it.GetState().row.GetValues());
            } else {
                nTupleUtils::PrintFirstK(it.GetState().row.GetKeys(), curLevel, out);
                out << std::endl;
                nTupleUtils::PrintTuple(arr[curLevel + 1], out);
                out << std::endl;
                out << std::endl;

                arr[curLevel] = ij(arr[curLevel], arr[curLevel + 1]);
                arr[curLevel + 1] = ne;
            }
        }
    }

private:
    Tree<Tuple, KeysIndexes, ValuesIndexes, Provider>& tree;
};
