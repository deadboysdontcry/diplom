#pragma once

#include <array>
#include <string>
#include <string_view>
#include <ranges>
#include <sstream>
#include <iostream>

namespace nStringUtils {

    namespace {

        template <typename T>
        concept Arithmetic = std::is_arithmetic_v<T>;

        template <typename T>
        concept HasFromStringFunction = requires(const std::string& s) {
            { T::FromString(s) } -> std::same_as<T>;
        };

        template <typename T, typename Stream = std::istream>
        concept Other = requires(Stream& s, T& t) {
            { s >> t } -> std::same_as<Stream&>;
        }
        &&!Arithmetic<T> && !HasFromStringFunction<T>;

    }

    template <Arithmetic T>
    T GetFromString(const std::string& s) {
        T value{};
        if constexpr (std::is_same_v<T, int>) {
            value = static_cast<T>(std::stoi(s));
        } else if constexpr (std::is_same_v<T, long>) {
            value = static_cast<T>(std::stol(s));
        } else if constexpr (std::is_same_v<T, long long>) {
            value = static_cast<T>(std::stoll(s));
        } else if constexpr (std::is_same_v<T, unsigned long>) {
            value = static_cast<T>(std::stoul(s));
        } else if constexpr (std::is_same_v<T, unsigned long long>) {
            value = static_cast<T>(std::stoull(s));
        } else if constexpr (std::is_same_v<T, float>) {
            value = static_cast<T>(std::stof(s));
        } else if constexpr (std::is_same_v<T, double>) {
            value = static_cast<T>(std::stod(s));
        } else if constexpr (std::is_same_v<T, long double>) {
            value = static_cast<T>(std::stold(s));
        } else {
            throw std::invalid_argument("Unsupported type.");
        }

        return value;
    }

    template <HasFromStringFunction T>
    T GetFromString(const std::string& s) {
        return T::FromString(s);
    }

    template <Other T>
    T GetFromString(const std::string& s) {
        std::istringstream stringSream(s);
        T value;
        stringSream >> value;
        return value;
    }

    template <int maxSize>
    std::array<std::string, maxSize> ParseStringIntoArray(std::string_view s, char delimiter) {
        std::array<std::string, maxSize> splited;
        size_t i = 0;
        if (i == maxSize) {
            return splited;
        }

        for (auto cur = s.find(delimiter); cur != std::string_view::npos; cur = s.find(delimiter)) {
            splited[i++] = {s.begin(), s.begin() + cur};
            s = s.substr(cur + 1, s.size() - cur);
            if (i == maxSize) {
                return splited;
            }
        }
        splited[maxSize - 1] = {s.begin(), s.end()};

        return splited;
    }

}
