#pragma once

#include "../row.hpp"
#include "../../utils/tuple_utils/tuple_utils.hpp"

#include <string>
#include <tuple>
#include <fstream>
#include <optional>
#include <array>

namespace nSimpleDBFileProvider {

    template <typename Tuple, typename KeysIndexes, typename ValuesIndexes>
    class Provider {
    public:
        Provider() = default;
        Provider(const Provider& p)
            : filePath_(p.filePath_)
            , rowDelimetr_(p.rowDelimetr_)
            , source_(filePath_)
        {
        }
        Provider(std::string filePath, char rowDelimetr)
            : source_(filePath)
            , rowDelimetr_(rowDelimetr)
            , filePath_(filePath)
        {
        }

        std::optional<Record<Tuple, KeysIndexes, ValuesIndexes>> GetNextRow() {
            if (!HasNext()) {
                return {};
            }
            std::string line;
            std::getline(source_, line, '\n');
            auto tuple = nTupleUtils::ParseIntoTuple<Tuple>(line, rowDelimetr_);
            return Record<Tuple, KeysIndexes, ValuesIndexes>(std::move(tuple));
        }

        bool HasNext() const {
            return !source_.eof() && source_.is_open();
        }

    private:
        std::string filePath_;
        std::ifstream source_;
        char rowDelimetr_;
    };

}
