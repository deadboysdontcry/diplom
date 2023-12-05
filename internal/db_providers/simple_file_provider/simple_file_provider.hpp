#pragma once

#include "row.hpp"
#include "../../utils/tuple_utils/tuple_utils.hpp"

#include <string>
#include <tuple>
#include <fstream>
#include <optional>
#include <array>

namespace nSimpleDBFileProvider {

template<typename Tuple, typename KeysIndexes, typename ValuesIndexes>
class Provider {
 public:

   typedef Row<Tuple, KeysIndexes, ValuesIndexes> RowType;

   Provider(std::string filePath, char rowDelimetr) : source_(filePath), rowDelimetr_(rowDelimetr) {
   }

   std::optional<RowType> GetNextRow() {
      if (!HasNext()) {
         return {};
      }
      std::string line;
      source_ >> line;
      auto tuple = nTupleUtils::ParseIntoTuple<Tuple>(line, rowDelimetr_);
      return Row<Tuple, KeysIndexes, ValuesIndexes>(std::move(tuple));
   } 

   bool HasNext() {
      return !source_.eof();
   }

   void Reset() {
      source_.seekg(0, std::ios::beg);
   }

 private:
    std::ifstream source_;   
    char rowDelimetr_;
};

}
