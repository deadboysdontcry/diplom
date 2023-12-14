#pragma once
#include "row.hpp"
#include <utility>
#include <optional>

template <typename T, typename Tuple, typename KeysIndexes, typename ValuesIndexes>
concept ProviderConcept = requires(T provider) {
    { T(provider) } -> std::same_as<T>;
    { T() } -> std::same_as<T>;
    { provider.HasNext() } -> std::same_as<bool>;
    { provider.GetNextRow() } -> std::same_as<std::optional<nSimpleDBFileProvider::Record<Tuple, KeysIndexes, ValuesIndexes>>>;
};
