#pragma once

#include "../db_providers/simple_file_provider/simple_file_provider.hpp"
#include <initializer_list>
#include <optional>
#include <utility>
#include <vector>


template <typename Tuple, typename KeysIndexes, typename ValuesIndexes>
class Tree {

 private:
    typedef nSimpleDBFileProvider::Provider<Tuple, KeysIndexes, ValuesIndexes> ProviderType;
    typedef typename ProviderType::RowType RowType;

    struct State {
        int64_t level;
        int64_t nt;
        std::optional<RowType> row;
    };


 public:

    Tree(const std::string& fileName) : provider_(fileName, '|') {
    }

    template <typename KeyPridicate, typename ValueJoiner>
    typename RowType::ValueType Calculate(const KeyPridicate& kp, const ValueJoiner& vj, const typename RowType::ValueType& neutralElement) {
        typename RowType::ValueType result{neutralElement};
        constexpr int64_t maxLevel = std::tuple_size_v<typename RowType::KeysType> + 1;
        int64_t k;
        State curState;
        if (!provider_.HasNext()) {
            curState.level = maxLevel + 1;
            k = -1;
        } else {
            curState.level = 0;
            curState.nt = 0;
            curState.row = std::nullopt;
            k = maxLevel;
        }
        
        RowType nextRow;
        std::optional<RowType> curRow{std::nullopt};

        std::vector<State> stack;
        while (true) {
            while(curState.level <= maxLevel) {
                curState.row = curRow;
                stack.push_back(curState);
                ++curState.level;
            }    
            if (stack.empty()) {
                break;
            }

            curState = stack.back();
            stack.pop_back();

            if (curState.level == maxLevel) { // leave 

                std::cout << "leave information begin:\n";
                std::cout << curState.level << " " << curState.nt << "  {";
                if (curState.row.has_value()) {
                    nTupleUtils::PrintTuple(curState.row->GetRaw());
                }  
                std::cout << "}\n";
                std::cout << "leave information end\n";

                if (!provider_.HasNext()) {
                    curState.level = maxLevel + 1;
                    k = -1;
                } else {

                    nextRow = *provider_.GetNextRow();
                    if (kp(nextRow.GetKeys())) {
                        result = vj(result, nextRow.GetValues());
                    }

                    if (curState.row != std::nullopt) {
                        k = nTupleUtils::GetFirstNEIdx(curState.row->GetRaw(), nextRow.GetRaw());
                        // std::cout << "K: " << k << std::endl;
                        if (k == maxLevel) {
                            curRow.emplace(nextRow);
                            ++curState.nt;
                        } else {
                            curState.level = maxLevel + 1;
                        }
                    } else {
                        curRow.emplace(nextRow);
                        ++curState.nt;    
                    }
                }
            } else {
                
                std::cout << "inner vertex information begin:\n";
                std::cout << curState.level << " " << curState.nt << "\n";
                std::cout << "inner vertex information end\n";
            
                
                if (curState.level == k) {
                    curRow.emplace(nextRow);
                    ++curState.nt;
                } else {
                    curState.level = maxLevel + 1;
                }
            }
        }

        return result;
    }


private:
    ProviderType provider_;
};
