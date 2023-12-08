#pragma once

#include "../db_providers/simple_file_provider/simple_file_provider.hpp"
#include "../db_providers/provider.hpp"
#include <initializer_list>
#include <optional>
#include <utility>
#include <vector>
#include <stdexcept>
#include <iterator>

template <typename Tuple, typename KeysIndexes, typename ValuesIndexes, ProviderConcept<Tuple, KeysIndexes, ValuesIndexes> Provider>
class Tree {

public:

    typedef nSimpleDBFileProvider::Row<Tuple, KeysIndexes, ValuesIndexes> RowType;
    typedef typename RowType::ValueType ValueType;
    typedef typename RowType::KeysType KeysType;
    class Iterator {
     private:
        Iterator(const Provider& p, const Tree* const t) : provider(p), t(t) {
            curState.level = 0;
            if (provider.HasNext()) {
                curRow = *provider.GetNextRow();
                while(curState.level <= maxLevel) {
                    if (curState.level == maxLevel) {
                        curState.row = curRow;
                    }
                    context.push_back(curState);
                    ++curState.level;
                }
            }
        }

        Iterator(const Tree* const t) : t(t) {
        }
     public:

        struct State {
            int64_t level;
            std::optional<RowType> row;

            bool operator==(const State& s2) const {
                return level == s2.level;
            }

            bool operator!=(const State& s2) const {
                return !(*this == s2);
            }

        };

        Iterator& operator++() {

            while(curState.level <= maxLevel) {
                if (curState.level == maxLevel) {
                        curState.row = curRow;
                }
                context.push_back(curState);
                ++curState.level;
            }    
            if (context.empty()) {
                throw std::out_of_range("out of range");
            }

            curState = context.back();
            context.pop_back();

            if (curState.level == maxLevel) { // leave 

                if (!provider.HasNext()) {
                    curState.level = maxLevel + 1;
                    k = -1;
                } else {

                    nextRow = *provider.GetNextRow();

                    if (curState.row != std::nullopt) {
                        k = nTupleUtils::GetFirstNEIdx(curState.row->GetRaw(), nextRow.GetRaw());
                        if (k == maxLevel) {
                            curRow = nextRow;
                        } else {
                            curState.level = maxLevel + 1; 
                        }
                    } else {
                        curRow = nextRow;
                    }
                }
            } else {
                if (curState.level == k) {
                    curRow = nextRow;
                    while(curState.level <= maxLevel) {
                        if (curState.level == maxLevel) {
                                curState.row = curRow;
                        }
                        context.push_back(curState);
                        ++curState.level;
                    }    
                } else {
                    curState.level = maxLevel + 1;
                }
            }
            return *this;
        }

        State GetState() const {
            if (context.size() == 0 && !provider.HasNext()) {
                throw std::out_of_range("end iter dereference");
            }    
            return context.back();
        }

        RowType LastRow() {
            return curRow;
        }

        bool operator==(const Iterator& it) const {
            return t == it.t && context == it.context && provider.HasNext() == it.provider.HasNext();
        }

        bool operator!=(const Iterator& it) const {
            return !(*this == it);
        }

        size_t GetLevel() {
            if (context.size() != 0) {
                return context.back().level;
            }
            return -1;
        }

        bool IsSubtreeDone() {
            if (context.empty()) {
                return true;
            }

            return context.back().level != k && context.back().level != maxLevel;
        }

     private:
        friend Tree;
        const Tree* const t;
        Provider provider;
        int64_t maxLevel = std::tuple_size_v<typename RowType::KeysType> + 1;
        State curState;
        RowType nextRow;
        RowType curRow;
        std::vector<State> context;
        int k;
    };


 public:

    size_t GetLevelsCount() {
        return std::tuple_size_v<typename RowType::KeysType> + 1;
    }

    Iterator begin() {
        return Iterator(baseProvider, this);
    }

    Iterator end() {
        return Iterator(this);
    }
    
    Tree(Provider p) : baseProvider(p) {
    }


private:
    Provider baseProvider;
};
