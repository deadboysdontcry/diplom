#pragma once

#include "tree.hpp"
#include "../db_providers/provider.hpp"
#include <stdexcept>


template <typename Tuple, typename KeysIndexes, typename ValuesIndexes, ProviderConcept<Tuple, KeysIndexes, ValuesIndexes> Provider>
class TreeHandler {
 private:
    typedef typename Tree<Tuple, KeysIndexes, ValuesIndexes, Provider>::ValueType ValueType;
 public:

    TreeHandler(Tree<Tuple, KeysIndexes, ValuesIndexes, Provider>& t) : tree(t) {
    }

    template<typename ValueAggregator>
    void CalculateAndCollectResultsInFile(const std::string& fileName, const ValueAggregator& va, const ValueType& ne) {
        std::ofstream out(fileName);
        std::vector<ValueType> valueByLevel(tree.GetLevelsCount() + 1, ne);
        for(auto it = tree.begin(); it != tree.end(); ++it) {
            auto curLevel = it.GetLevel();
            if (it.GetState().row.has_value()) { // if leave
                valueByLevel[curLevel] = va(valueByLevel[curLevel], it.GetState().row->GetValues());
            } else { // if inner 
                valueByLevel[curLevel] = va(valueByLevel[curLevel], valueByLevel[curLevel+1]);
                valueByLevel[curLevel+1] = ne;
            }
            if (it.IsSubtreeDone()) { // we processed all leafs in subtrees
                out << "all values with keys eual: ";
                nTupleUtils::PrintFirstK(it.LastRow().GetKeys(), curLevel, out);
                for (int i = curLevel; i < tree.GetLevelsCount() - 1; i++) {
                    out << "x|";
                }
                out << std::endl;
                out << "have aggregate vallue: ";
                nTupleUtils::PrintTuple(valueByLevel[curLevel], out);
                out << std::endl;
                out << std::endl;
            }
        }
    }

 private:
    Tree<Tuple, KeysIndexes, ValuesIndexes, Provider>& tree;
};
