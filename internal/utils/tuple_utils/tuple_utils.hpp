#pragma once

#include "../string_utils/string_utils.hpp"

#include <string_view>
#include <ranges>
#include <iostream>
#include <tuple>

namespace nTupleUtils {

    template <typename T, typename Array>
    T GetFromStringFromArray(Array const& a, int i)
    {   
        return nStringUtils::GetFromString<T>(a[i]);
    }

///////////////////////////////////////////////////

    // coppied from FileTuples.cpp
    namespace {
        template<int ... Idxs> struct IndexesTuple
        {
        };

        template<int Num, typename Tp = IndexesTuple<>>
        struct IndexTupleBuilder;

        template<int Num, int ... Idxs>
        struct IndexTupleBuilder<Num, IndexesTuple<Idxs ...>> : IndexTupleBuilder<Num - 1, IndexesTuple<Idxs ..., sizeof ... (Idxs)>>
        {
        };

        template<int ... Idxs>
        struct IndexTupleBuilder<0, IndexesTuple<Idxs ...>>
        {
            typedef IndexesTuple<Idxs...> Indexes;
        };

            template <typename Array, typename IdxsTuple, typename ... Types> struct ReadToTuple;

            template <typename Array, int ... Idxs, typename... Types>
            struct ReadToTuple<Array, IndexesTuple<Idxs...>, Types ...>
            {
            public:
            std::tuple<Types...> t;

            public:
            ReadToTuple(Array const& a, IndexesTuple<Idxs...> idx) : t(GetFromStringFromArray<Types>(a, Idxs)...) {  }

            decltype(auto) Read() { return t; }
            };


        template <typename Tuple, typename  Array, typename Indexs> struct callReadToTuple;

        template <typename... Types, typename Array, typename Indexs>
        struct callReadToTuple<std::tuple<Types...>, Array, Indexs>
        {
            callReadToTuple(Array& aFs, Indexs& aInd) : r(aFs, aInd) { }
            ReadToTuple<Array, Indexs, Types...> r; //(aFs, IndexTupleBuilder<n>::Indexes());

        };
    }

////////////////////////////////////////////////////////////////////

    template <typename Tuple>
    auto ParseIntoTuple(std::string_view s, char delimiter) {

        constexpr size_t tupleSize = std::tuple_size_v<Tuple>;
        auto splited = nStringUtils::ParseStringIntoArray<tupleSize>(s, delimiter);
        auto idxs = typename IndexTupleBuilder<tupleSize>::Indexes();
        callReadToTuple<Tuple, decltype(splited), typename IndexTupleBuilder<tupleSize>::Indexes> xCall(splited, idxs);

	    return xCall.r.Read();
    }

//////////////////////////////////////////////////////////////////
    template <int... idxs>
    struct TupleIndexes
    {
    };

    template <typename KeyInds> struct CutByIndexes;

    template <int... idxs>
    struct CutByIndexes<TupleIndexes<idxs...>>
    {
        template <typename Tuple>
        static auto Do(Tuple&& t)
        {
            return std::make_tuple(std::get<idxs>(std::forward<Tuple>(t))...);
        }
    };

////////////////////////////////////////

    template <size_t I = 0, typename Tuple>
    size_t GetFirstNEIdx(const Tuple& t1, const Tuple& t2) {
        if constexpr (I == std::tuple_size_v<Tuple>) {
            return I;
        }  else {
            if (std::get<I>(t1) != std::get<I>(t2)) {
                return I; 
            }
            return GetFirstNEIdx<I + 1>(t1, t2); 
        }
    }
//////////////////////////////////////////
    template <std::size_t I = 0, typename Tuple>
    void PrintTuple(const Tuple& t) {
    if constexpr (I < std::tuple_size_v<Tuple>) {
        std::cout << std::get<I>(t) << ' ';
        PrintTuple<I + 1>(t);
    }
}

}
