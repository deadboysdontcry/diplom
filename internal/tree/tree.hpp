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
    typedef nSimpleDBFileProvider::Record<Tuple, KeysIndexes, ValuesIndexes> RecordType;
    typedef typename RecordType::ValueType ValueType;
    typedef typename RecordType::KeysType KeysType;


    class Iterator {
    private:
        struct State {
            int64_t level;
            RecordType row;

            bool operator==(const State& s2) const {
                return level == s2.level;
            }

            bool operator!=(const State& s2) const {
                return !(*this == s2);
            }
        };

        Iterator(const Provider& p, const Tree* const t)
            : provider(p)
            , t(t)
        {
            curState.level = 0;
            if (provider.HasNext()) {
                curRow = *provider.GetNextRow();
                TryGoDeep();
            }
        }

        Iterator(const Tree* const t)
            : t(t)
        {
        }

        void TryGoDeep() {
            while (curState.level <= maxLevel) {
                curState.row = curRow;
                context.push_back(curState);
                ++curState.level;
            }
        }

        void MakeStep() {
            TryGoDeep();
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
                    k = nTupleUtils::GetFirstNEIdx(curState.row.GetRaw(), nextRow.GetRaw());
                    if (k == maxLevel) {
                        curRow = nextRow;
                    } else {
                        curState.level = maxLevel + 1;
                    }
                }
            } else {
                if (curState.level == k) {
                    curRow = nextRow;
                    TryGoDeep();
                } else {
                    curState.level = maxLevel + 1;
                }
            }
        }

        bool IsValidState() {
            if (context.empty()) { // equals to .end()
                return true;
            }
            if (IsLeave()) {
                return true;
            }
            if (context.back().level != k) { // is inner
                return true;
            }
            return false;
        }

    public:
        Iterator& operator++() {
            MakeStep();
            while (!IsValidState()) {
                MakeStep();
            }
            return *this;
        }

        const RecordType& GetState() const {
            if (context.size() == 0 && !provider.HasNext()) {
                throw std::out_of_range("end iter dereference");
            }
            return context.back().row;
        }

        bool operator==(const Iterator& it) const {
            return t == it.t && context == it.context && provider.HasNext() == it.provider.HasNext();
        }

        bool operator!=(const Iterator& it) const {
            return !(*this == it);
        }

        bool IsLeave() {
            if (context.empty()) {
                return false;
            }
            if (context.back().level == maxLevel) {
                return true;
            }
            return false;
        }

        int GetLevel() const {
            return context.back().level;
        }

    private:
        friend Tree;
        const Tree* const t;
        Provider provider;
        int64_t maxLevel = std::tuple_size_v<typename RecordType::KeysType> + 1;
        State curState;
        RecordType nextRow;
        RecordType curRow;
        std::vector<State> context;
        int k;
    };

public:
    constexpr size_t GetLevelsCount() {
        return std::tuple_size_v<typename RecordType::KeysType> + 2;
    }

    Iterator begin() {
        return Iterator(baseProvider, this);
    }

    Iterator end() {
        return Iterator(this);
    }

    Tree(Provider p)
        : baseProvider(p)
    {
    }

private:
    Provider baseProvider;
};
