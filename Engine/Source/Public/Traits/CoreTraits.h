// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

template<typename T>
struct TIsCompleteType : std::false_type { };

template<typename T> requires requires { sizeof(T); }
struct TIsCompleteType<T> : std::true_type { };

template<typename T>
inline constexpr bool TIsCompleteType_v = TIsCompleteType<T>::value;

} /* ~Namespace Jafg */
