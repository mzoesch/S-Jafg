// Copyright mzoesch. All rights reserved.

#pragma once

#include "Maths/MathsTypes.h"

#define MATHS_CONSTANT(Constant)                                                                            \
    namespace detail                                                                                        \
    {                                                                                                       \
    template<typename T> struct JAFG_JOIN_OUTER_TWO(Constant, _t) {};                                       \
    } /* ~Namespace detail */                                                                               \
    template<typename T> inline constexpr T Constant = detail::JAFG_JOIN_OUTER_TWO(Constant, _t)<T>::value;

#define DETAIL_MATHS_FLT_CONSTANT(Constant, Type, Value)         \
    template<> struct JAFG_JOIN_OUTER_TWO(Constant, _t)<Type>         \
    {                                                                 \
        inline static constexpr Type value{static_cast<Type>(Value)}; \
    };

#define MATHS_FLT_CONSTANT_V(Constant, Value) \
    namespace detail                                                                                      \
    {                                                                                                     \
    template<typename T> struct JAFG_JOIN_OUTER_TWO(Constant, _t) {};                                     \
    DETAIL_MATHS_FLT_CONSTANT(Constant, f32, Value)                                                       \
    DETAIL_MATHS_FLT_CONSTANT(Constant, f64, Value)                                                       \
    template<typename T> inline constexpr T JAFG_JOIN_OUTER_TWO(Constant,_v)                              \
    {                                                                                                     \
        detail::JAFG_JOIN_OUTER_TWO(Constant, _t)<T>::value };                                            \
    } /* ~Namespace detail */                                                                             \
    inline constexpr f32 JAFG_JOIN_OUTER_TWO(Constant, _f){detail::JAFG_JOIN_OUTER_TWO(Constant,_v)<f32>}; \
    inline constexpr f64 JAFG_JOIN_OUTER_TWO(Constant, _d){detail::JAFG_JOIN_OUTER_TWO(Constant,_v)<f64>};

#define MATHS_SWIZZLE(Num, Name, ...)                                                            \
    struct JAFG_JOIN_OUTER_TWO(Name, _fn)                                                        \
    {                                                                                            \
        template<length_t L, typename T, qual_t Q>                                               \
        inline constexpr auto operator()(TVec<L, T, Q> const& v) const noexcept -> TVec<Num,T,Q> \
        {                                                                                        \
            return __VA_ARGS__;                                                                  \
        }                                                                                        \
    };

namespace maths::detail
{

MATHS_SWIZZLE(2, xy, {v.x,v.y})
MATHS_SWIZZLE(2, xz, {v.x,v.z})
MATHS_SWIZZLE(2, yx, {v.y,v.x})
MATHS_SWIZZLE(2, yz, {v.y,v.z})
MATHS_SWIZZLE(3, xyz, {v.x,v.y,v.z})
MATHS_SWIZZLE(3, xzy, {v.x,v.z,v.y})
MATHS_SWIZZLE(3, yxz, {v.y,v.x,v.z})
MATHS_SWIZZLE(3, yzx, {v.y,v.z,v.x})
MATHS_SWIZZLE(3, zxy, {v.z,v.x,v.y})
MATHS_SWIZZLE(3, zyx, {v.z,v.y,v.x})

} /* ~Namespace maths::detail */
