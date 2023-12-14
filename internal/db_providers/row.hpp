#pragma once

#include "../utils/tuple_utils/tuple_utils.hpp"

#include <utility>
#include <tuple>

namespace nSimpleDBFileProvider {

    template <typename Tuple, typename KeysIndexes, typename ValuesIndexes>
    class Record {
    public:
        typedef decltype(nTupleUtils::CutByIndexes<ValuesIndexes>::Do(std::declval<Tuple>())) ValueType;
        typedef decltype(nTupleUtils::CutByIndexes<KeysIndexes>::Do(std::declval<Tuple>())) KeysType;

        Record() = default;

        explicit Record(Tuple&& t)
            : data_(std::forward<Tuple>(t))
        {
        }

        explicit Record(const Tuple& t)
            : data_(t)
        {
        }

        KeysType GetKeys() const {
            return nTupleUtils::CutByIndexes<KeysIndexes>::Do(data_);
        }

        ValueType GetValues() const {
            return nTupleUtils::CutByIndexes<ValuesIndexes>::Do(data_);
        }

        const Tuple& GetRaw() const {
            return data_;
        }

    private:
        Tuple data_;
    };

}
