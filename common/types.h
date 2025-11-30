//
// Created by Piotr Pszczolkowski on 30/11/2025.
//
#pragma once

/*------- include files:
-------------------------------------------------------------------*/
#include <string>
#include <string_view>
#include <vector>
#include <span>
#include <optional>
#include <variant>
#include <array>
#include <expected>
#include <unordered_set>
#include <unordered_map>

template<typename T>
concept Stringable =
        std::is_class_v<std::string> ||
        std::is_class_v<std::string_view>;

template<typename T>
concept Vectorizable =
        std::is_class_v<std::vector<T> > ||
        std::is_class_v<std::span<T> >;

static constexpr auto Yes = true;
static constexpr auto No = false;

using i8 = char;
using u8 = unsigned char;
using i16 = int16_t;
using u16 = uint16_t;
using i32 = int32_t;
using u32 = uint32_t;
using i64 = int64_t;
using u64 = uint64_t;
using f32 = float;
using f64 = double;
using uint = unsigned int;

using String = std::string;
using StringView = std::string_view;
using Bytes = std::vector<char>;
using BytesView = std::span<char>;
using BytesViewConst = std::span<const char>;
template<typename T, std::size_t N> using Array = std::array<T, N>;
template<typename T> using Set = std::unordered_set<T>;
template<typename K, typename V> using Map = std::unordered_map<K, V>;
template<typename T> using Vector = std::vector<T>;
template<typename T> using Span = std::span<T>;
template<typename T> using Option = std::optional<T>;
template<typename T, typename E> using Result = std::expected<T, E>;
template<typename T> using Failure = std::unexpected<T>;
template<typename T, typename U> using Pair = std::pair<T, U>;
template<typename... T> using Variant = std::variant<T...>;