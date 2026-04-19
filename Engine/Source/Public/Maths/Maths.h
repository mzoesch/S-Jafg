// Copyright mzoesch. All rights reserved.

#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_RIGHT_HANDED
#include <glm/glm.hpp>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <glm/mat2x2.hpp>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>

#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <glm/gtx/hash.hpp>
#include <glm/gtx/string_cast.hpp>

namespace maths
{

using length_t = glm::length_t;
using qual_t = glm::qualifier;

using glm::qualifier;
using qualifier::highp;
using qualifier::mediump;
using qualifier::lowp;
using qualifier::packed;
using qualifier::defaultp;

typedef f32 single_precision;
typedef f64 double_precision;

} /* ~Namespace maths */

template<typename TReal,maths::qual_t Q> using TQua = glm::qua<TReal,Q>;
using LQuatF = TQua<maths::single_precision,maths::defaultp>;
using LQuatD = TQua<maths::double_precision,maths::defaultp>;

template<maths::length_t L,typename TReal,maths::qual_t Q> using TVec = glm::vec<L,TReal,Q>;
template<typename TReal,maths::qual_t Q> using TVec1 = TVec<1,TReal,Q>;
using LVec1F = TVec1<maths::single_precision,maths::defaultp>;
using LVec1D = TVec1<maths::double_precision,maths::defaultp>;
template<typename TReal,maths::qual_t Q> using TVec2 = TVec<2,TReal,Q>;
using LVec2F = TVec2<maths::single_precision,maths::defaultp>;
using LVec2D = TVec2<maths::double_precision,maths::defaultp>;
template<typename TReal,maths::qual_t Q> using TVec3 = TVec<3,TReal,Q>;
using LVec3F = TVec3<maths::single_precision,maths::defaultp>;
using LVec3D = TVec3<maths::double_precision,maths::defaultp>;
template<typename TReal,maths::qual_t Q> using TVec4 = TVec<4,TReal,Q>;
using LVec4F = TVec4<maths::single_precision,maths::defaultp>;
using LVec4D = TVec4<maths::double_precision,maths::defaultp>;

using LVec1u8 = TVec1<u8,maths::defaultp>;
using LVec1i8 = TVec1<i8,maths::defaultp>;
using LVec2u8 = TVec2<u8,maths::defaultp>;
using LVec2i8 = TVec2<i8,maths::defaultp>;
using LVec3u8 = TVec3<u8,maths::defaultp>;
using LVec3i8 = TVec3<i8,maths::defaultp>;
using LVec4u8 = TVec4<u8,maths::defaultp>;
using LVec4i8 = TVec4<i8,maths::defaultp>;

using LVec1u16 = TVec1<u16,maths::defaultp>;
using LVec1i16 = TVec1<i16,maths::defaultp>;
using LVec2u16 = TVec2<u16,maths::defaultp>;
using LVec2i16 = TVec2<i16,maths::defaultp>;
using LVec3u16 = TVec3<u16,maths::defaultp>;
using LVec3i16 = TVec3<i16,maths::defaultp>;
using LVec4u16 = TVec4<u16,maths::defaultp>;
using LVec4i16 = TVec4<i16,maths::defaultp>;

using LVec1u32 = TVec1<u32,maths::defaultp>;
using LVec1i32 = TVec1<i32,maths::defaultp>;
using LVec2u32 = TVec2<u32,maths::defaultp>;
using LVec2i32 = TVec2<i32,maths::defaultp>;
using LVec3u32 = TVec3<u32,maths::defaultp>;
using LVec3i32 = TVec3<i32,maths::defaultp>;
using LVec4u32 = TVec4<u32,maths::defaultp>;
using LVec4i32 = TVec4<i32,maths::defaultp>;

using LVec1u64 = TVec1<u64,maths::defaultp>;
using LVec1i64 = TVec1<i64,maths::defaultp>;
using LVec2u64 = TVec2<u64,maths::defaultp>;
using LVec2i64 = TVec2<i64,maths::defaultp>;
using LVec3u64 = TVec3<u64,maths::defaultp>;
using LVec3i64 = TVec3<i64,maths::defaultp>;
using LVec4u64 = TVec4<u64,maths::defaultp>;
using LVec4i64 = TVec4<i64,maths::defaultp>;

template<maths::length_t C,maths::length_t R,typename T,maths::qual_t Q>
using TMat = glm::mat<C,R,T,Q>;
template<typename T,maths::qual_t Q> using TMat2 = TMat<2,2,T,Q>;
template<typename T,maths::qual_t Q> using TMat3 = TMat<3,3,T,Q>;
template<typename T,maths::qual_t Q> using TMat4 = TMat<4,4,T,Q>;
using LMat2F = TMat2<maths::single_precision,maths::defaultp>;
using LMat2D = TMat2<maths::double_precision,maths::defaultp>;
using LMat3F = TMat3<maths::single_precision,maths::defaultp>;
using LMat3D = TMat3<maths::double_precision,maths::defaultp>;
using LMat4F = TMat4<maths::single_precision,maths::defaultp>;
using LMat4D = TMat4<maths::double_precision,maths::defaultp>;

//# Transformation object.
template<typename TReal,maths::qual_t Q>
struct TTrans final
{
    //# Translation.
    TVec3<TReal,Q> T;
    //# Rotation.
    TQua<TReal,Q>  R;
    //# Scale.
    TVec3<TReal,Q> S;
};
using LTransformF = TTrans<maths::single_precision,maths::defaultp>;
using LTransformD = TTrans<maths::double_precision,maths::defaultp>;

template<typename T,maths::qual_t Q>
struct TRect2 final
{
    TVec2<T,Q> Offset;
    TVec2<T,Q> Extent;
};
using LRect2F = TRect2<f32,maths::defaultp>;
using LRect2D = TRect2<f32,maths::defaultp>;

//#
//# Types used for world coordinates.
//#
inline constexpr maths::qualifier world_qual{maths::highp};
using LWorldReal  = maths::single_precision;
using LWorldQuat  = TQua<LWorldReal,world_qual>;
using LWorldVec1  = TVec1<LWorldReal,world_qual>;
using LWorldVec2  = TVec2<LWorldReal,world_qual>;
using LWorldVec3  = TVec3<LWorldReal,world_qual>;
using LWorldVec4  = TVec4<LWorldReal,world_qual>;
using LWorldTrans = TTrans<LWorldReal,world_qual>;

#define MATHS_CONSTANT(Constant)                                                                            \
    namespace detail                                                                                        \
    {                                                                                                       \
    template<typename T> struct JAFG_JOIN_OUTER_TWO(Constant, _t) {};                                       \
    } /* ~Namespace detail */                                                                               \
    template<typename T> inline constexpr T Constant{detail::JAFG_JOIN_OUTER_TWO(Constant, _t)<T>::value};

#define DETAIL_MATHS_FLT_CONSTANT(Constant, Type, Value)              \
    template<> struct JAFG_JOIN_OUTER_TWO(Constant, _t)<Type>         \
    {                                                                 \
        inline static constexpr Type value{static_cast<Type>(Value)}; \
    };

#define MATHS_FLT_CONSTANT_V(Constant, Value)                                                              \
    namespace detail                                                                                       \
    {                                                                                                      \
    template<typename T> struct JAFG_JOIN_OUTER_TWO(Constant, _t) {};                                      \
    DETAIL_MATHS_FLT_CONSTANT(Constant, f32, Value)                                                        \
    DETAIL_MATHS_FLT_CONSTANT(Constant, f64, Value)                                                        \
    template<typename T> inline constexpr T JAFG_JOIN_OUTER_TWO(Constant,_v)                               \
    {                                                                                                      \
        detail::JAFG_JOIN_OUTER_TWO(Constant, _t)<T>::value };                                             \
    } /* ~Namespace detail */                                                                              \
    inline constexpr f32 JAFG_JOIN_OUTER_TWO(Constant, _f){detail::JAFG_JOIN_OUTER_TWO(Constant,_v)<f32>}; \
    inline constexpr f64 JAFG_JOIN_OUTER_TWO(Constant, _d){detail::JAFG_JOIN_OUTER_TWO(Constant,_v)<f64>};

#define MATHS_SWIZZLE(Num, Name, ...)                                                            \
    namespace detail                                                                             \
    {                                                                                            \
    struct JAFG_JOIN_OUTER_TWO(Name, _fn)                                                        \
    {                                                                                            \
        template<length_t L, typename T, qual_t Q>                                               \
        inline constexpr auto operator()(TVec<L, T, Q> const& v) const noexcept -> TVec<Num,T,Q> \
        {                                                                                        \
            return __VA_ARGS__;                                                                  \
        }                                                                                        \
    };                                                                                           \
    } /* ~Namespace detail */                                                                    \
    inline constexpr detail::JAFG_JOIN_OUTER_TWO(Name, _fn) Name{};

#define MATHS_CONSTANT_VALUE(constant, type, ...)       \
    template<>                                          \
    struct JAFG_JOIN_OUTER_TWO(constant, _t)<type>      \
    {                                                   \
        inline static constexpr type value __VA_ARGS__; \
    };

namespace maths
{

///////////////////////////////////////////////////////////////////////////////
// Constants
//# Global zero vector constant (0, 0, 0, 0).
MATHS_CONSTANT(zero_vector)
//# Global one vector constant (1, 1, 1, 1).
MATHS_CONSTANT(one_vector)
//# Global forward vector constant (0, 0, -1).
MATHS_CONSTANT(forward_vector)
//# Global backward vector constant (0, 0, 1).
MATHS_CONSTANT(backward_vector)
//# Global right vector constant (1, 0, 0).
MATHS_CONSTANT(right_vector)
//# Global left vector constant (-1, 0, 0).
MATHS_CONSTANT(left_vector)
//# Global up vector constant (0, 1, 0).
MATHS_CONSTANT(up_vector)
//# Global down vector constant (0, -1, 0).
MATHS_CONSTANT(down_vector)
//# Global 2d unit vector constant along the 45-degree angle.
MATHS_CONSTANT(unit_vector_2d)
//# Global unit vector constant along the x-axis (1, 0, 0, 0).
MATHS_CONSTANT(unit_vector_x)
//# Global unit vector constant along the y-axis (0, 1, 0, 0).
MATHS_CONSTANT(unit_vector_y)
//# Global unit vector constant along the z-axis (0, 0, 1, 0).
MATHS_CONSTANT(unit_vector_z)
//# Global unit vector constant along the w-axis (0, 0, 0, 1).
MATHS_CONSTANT(unit_vector_w)

//# Global identity constant.
MATHS_CONSTANT(identity)

#undef MATHS_CONSTANT

MATHS_FLT_CONSTANT_V(small_number,        1e-8)
MATHS_FLT_CONSTANT_V(not_so_small_number, 1e-4)

MATHS_FLT_CONSTANT_V(pi,         3.1415926535897932384626433832795028841971693993751058209749445923078164)
MATHS_FLT_CONSTANT_V(half_pi,    1.5707963267948966192313216916397514420985846996875529104874722961539082)
MATHS_FLT_CONSTANT_V(two_pi,     6.2831853071795864769252867665590057683943387987502116419498891846156328)
MATHS_FLT_CONSTANT_V(pi_sqrt,    1.7724538509055160272981674833411451827975494561223871282138077898529112)
MATHS_FLT_CONSTANT_V(pi_squared, 9.8696044010893586188344909998761511353136994072407906264133493762200448)
MATHS_FLT_CONSTANT_V(quarter_circle_deg,        90.0)
MATHS_FLT_CONSTANT_V(quarter_circle_rad,        1.5707963267948966192313216916397514420985846996875529104874722961539082)
MATHS_FLT_CONSTANT_V(half_circle_deg,           180.0)
MATHS_FLT_CONSTANT_V(half_circle_rad,           3.1415926535897932384626433832795028841971693993751058209749445923078164)
MATHS_FLT_CONSTANT_V(three_quarters_circle_deg, 270.0)
MATHS_FLT_CONSTANT_V(three_quarters_circle_rad, 4.7123889803846898576939650749192543262957540990626587314624168884617246)
MATHS_FLT_CONSTANT_V(full_circle_deg,           360.0)
MATHS_FLT_CONSTANT_V(full_circle_rad,           6.2831853071795864769252867665590057683943387987502116419498891846156328)

MATHS_FLT_CONSTANT_V(from_percent, 0.01)
MATHS_FLT_CONSTANT_V(to_percent,   100.0)

//# Scalar conversion constants for metric units.
MATHS_FLT_CONSTANT_V(km2m,  1e+3)
MATHS_FLT_CONSTANT_V(km2dm, 1e+4)
MATHS_FLT_CONSTANT_V(km2cm, 1e+5)
MATHS_FLT_CONSTANT_V(km2mm, 1e+6)
MATHS_FLT_CONSTANT_V(m2km,  1e-3)
MATHS_FLT_CONSTANT_V(m2dm,  1e+1)
MATHS_FLT_CONSTANT_V(m2cm,  1e+2)
MATHS_FLT_CONSTANT_V(m2mm,  1e+3)
MATHS_FLT_CONSTANT_V(dm2km, 1e-4)
MATHS_FLT_CONSTANT_V(dm2m,  1e-1)
MATHS_FLT_CONSTANT_V(dm2cm, 1e+1)
MATHS_FLT_CONSTANT_V(dm2mm, 1e+2)
MATHS_FLT_CONSTANT_V(cm2km, 1e-5)
MATHS_FLT_CONSTANT_V(cm2m,  1e-2)
MATHS_FLT_CONSTANT_V(cm2dm, 1e-1)
MATHS_FLT_CONSTANT_V(cm2mm, 1e+1)
MATHS_FLT_CONSTANT_V(mm2km, 1e-6)
MATHS_FLT_CONSTANT_V(mm2m,  1e-3)
MATHS_FLT_CONSTANT_V(mm2dm, 1e-2)
MATHS_FLT_CONSTANT_V(mm2cm, 1e-1)

//# Scalar conversion constants for squared metric units.
MATHS_FLT_CONSTANT_V(km2_to_m2,  1e+6)
MATHS_FLT_CONSTANT_V(km2_to_dm2, 1e+8)
MATHS_FLT_CONSTANT_V(km2_to_cm2, 1e+10)
MATHS_FLT_CONSTANT_V(km2_to_mm2, 1e+12)
MATHS_FLT_CONSTANT_V(m2_to_km2,  1e-6)
MATHS_FLT_CONSTANT_V(m2_to_dm2,  1e+2)
MATHS_FLT_CONSTANT_V(m2_to_cm2,  1e+4)
MATHS_FLT_CONSTANT_V(m2_to_mm2,  1e+6)
MATHS_FLT_CONSTANT_V(dm2_to_km2, 1e-8)
MATHS_FLT_CONSTANT_V(dm2_to_m2,  1e-2)
MATHS_FLT_CONSTANT_V(dm2_to_cm2, 1e+2)
MATHS_FLT_CONSTANT_V(dm2_to_mm2, 1e+4)
MATHS_FLT_CONSTANT_V(cm2_to_km2, 1e-10)
MATHS_FLT_CONSTANT_V(cm2_to_m2,  1e-4)
MATHS_FLT_CONSTANT_V(cm2_to_dm2, 1e-2)
MATHS_FLT_CONSTANT_V(cm2_to_mm2, 1e+2)
MATHS_FLT_CONSTANT_V(mm2_to_km2, 1e-12)
MATHS_FLT_CONSTANT_V(mm2_to_m2,  1e-6)
MATHS_FLT_CONSTANT_V(mm2_to_dm2, 1e-4)
MATHS_FLT_CONSTANT_V(mm2_to_cm2, 1e-2)

//# Scalar conversion constants for cubed metric units.
MATHS_FLT_CONSTANT_V(km3_to_m3,  1e+9)
MATHS_FLT_CONSTANT_V(km3_to_dm3, 1e+12)
MATHS_FLT_CONSTANT_V(km3_to_cm3, 1e+15)
MATHS_FLT_CONSTANT_V(km3_to_mm3, 1e+18)
MATHS_FLT_CONSTANT_V(m3_to_km3,  1e-9)
MATHS_FLT_CONSTANT_V(m3_to_dm3,  1e+3)
MATHS_FLT_CONSTANT_V(m3_to_cm3,  1e+6)
MATHS_FLT_CONSTANT_V(m3_to_mm3,  1e+9)
MATHS_FLT_CONSTANT_V(dm3_to_km3, 1e-12)
MATHS_FLT_CONSTANT_V(dm3_to_m3,  1e-3)
MATHS_FLT_CONSTANT_V(dm3_to_cm3, 1e+3)
MATHS_FLT_CONSTANT_V(dm3_to_mm3, 1e+6)
MATHS_FLT_CONSTANT_V(cm3_to_km3, 1e-15)
MATHS_FLT_CONSTANT_V(cm3_to_m3,  1e-6)
MATHS_FLT_CONSTANT_V(cm3_to_dm3, 1e-3)
MATHS_FLT_CONSTANT_V(cm3_to_mm3, 1e+3)
MATHS_FLT_CONSTANT_V(mm3_to_km3, 1e-18)
MATHS_FLT_CONSTANT_V(mm3_to_m3,  1e-9)
MATHS_FLT_CONSTANT_V(mm3_to_dm3, 1e-6)
MATHS_FLT_CONSTANT_V(mm3_to_cm3, 1e-3)

//# Time conversion constants.
MATHS_FLT_CONSTANT_V(ns2mus, 1e-3)
MATHS_FLT_CONSTANT_V(ns2ms,  1e-6)
MATHS_FLT_CONSTANT_V(ns2s,   1e-9)
MATHS_FLT_CONSTANT_V(ns2m,   1.6667e-11)
MATHS_FLT_CONSTANT_V(ns2h,   2.7778e-13)
MATHS_FLT_CONSTANT_V(mus2ns, 1e+3)
MATHS_FLT_CONSTANT_V(mus2ms, 1e-3)
MATHS_FLT_CONSTANT_V(mus2S,  1e-6)
MATHS_FLT_CONSTANT_V(mus2M,  1.6667e-8)
MATHS_FLT_CONSTANT_V(mus2H,  2.7778e-10)
MATHS_FLT_CONSTANT_V(ms2ns,  1e+6)
MATHS_FLT_CONSTANT_V(ms2mus, 1e+3)
MATHS_FLT_CONSTANT_V(ms2s,   1e-3)
MATHS_FLT_CONSTANT_V(ms2m,   1.6667e-5)
MATHS_FLT_CONSTANT_V(ms2h,   2.7778e-7)
MATHS_FLT_CONSTANT_V(s2ns,   1e+9)
MATHS_FLT_CONSTANT_V(s2mus,  1e+6)
MATHS_FLT_CONSTANT_V(s2ms,   1e+3)
MATHS_FLT_CONSTANT_V(s2m,    1.f / 60.)
MATHS_FLT_CONSTANT_V(s2h,    1.f / 3600.)
MATHS_FLT_CONSTANT_V(m2NS,   6e+10)
MATHS_FLT_CONSTANT_V(m2mus,  6e+7)
MATHS_FLT_CONSTANT_V(m2MS,   60000.0)
MATHS_FLT_CONSTANT_V(m2s,    60.0)
MATHS_FLT_CONSTANT_V(m2h,    0.016666666666666666666666666666667)
MATHS_FLT_CONSTANT_V(h2ns,   3.6e+12)
MATHS_FLT_CONSTANT_V(h2mus,  3.6e+9)
MATHS_FLT_CONSTANT_V(h2ms,   3.6e+6)
MATHS_FLT_CONSTANT_V(h2s,    3600.0)
MATHS_FLT_CONSTANT_V(h2m,    60.0)

#undef MATHS_FLT_CONSTANT_V

///////////////////////////////////////////////////////////////////////////////
// Swizzle
MATHS_SWIZZLE(2, xy, {v.x,v.y})
MATHS_SWIZZLE(2, xz, {v.x,v.z})
MATHS_SWIZZLE(2, xw, {v.x,v.w})
MATHS_SWIZZLE(2, yx, {v.y,v.x})
MATHS_SWIZZLE(2, yz, {v.y,v.z})
MATHS_SWIZZLE(2, yw, {v.y,v.w})
MATHS_SWIZZLE(2, zx, {v.z,v.x})
MATHS_SWIZZLE(2, zy, {v.z,v.y})
MATHS_SWIZZLE(2, zw, {v.z,v.w})
MATHS_SWIZZLE(2, wx, {v.w,v.x})
MATHS_SWIZZLE(2, wy, {v.w,v.y})
MATHS_SWIZZLE(2, wz, {v.w,v.z})

MATHS_SWIZZLE(3, xyz, {v.x,v.y,v.z})
MATHS_SWIZZLE(3, xyw, {v.x,v.y,v.w})
MATHS_SWIZZLE(3, xzy, {v.x,v.z,v.y})
MATHS_SWIZZLE(3, xzw, {v.x,v.z,v.w})
MATHS_SWIZZLE(3, xwy, {v.x,v.w,v.y})
MATHS_SWIZZLE(3, xwz, {v.x,v.w,v.z})
MATHS_SWIZZLE(3, yxz, {v.y,v.x,v.z})
MATHS_SWIZZLE(3, yxw, {v.y,v.x,v.w})
MATHS_SWIZZLE(3, yzx, {v.y,v.z,v.x})
MATHS_SWIZZLE(3, yzw, {v.y,v.z,v.w})
MATHS_SWIZZLE(3, ywx, {v.y,v.w,v.x})
MATHS_SWIZZLE(3, ywz, {v.y,v.w,v.z})
MATHS_SWIZZLE(3, zyx, {v.z,v.y,v.x})
MATHS_SWIZZLE(3, zyw, {v.z,v.y,v.w})
MATHS_SWIZZLE(3, zxy, {v.z,v.x,v.y})
MATHS_SWIZZLE(3, zxw, {v.z,v.x,v.w})
MATHS_SWIZZLE(3, zwy, {v.z,v.w,v.y})
MATHS_SWIZZLE(3, zwx, {v.z,v.w,v.x})
MATHS_SWIZZLE(3, wyz, {v.w,v.y,v.z})
MATHS_SWIZZLE(3, wyx, {v.w,v.y,v.x})
MATHS_SWIZZLE(3, wzy, {v.w,v.z,v.y})
MATHS_SWIZZLE(3, wzx, {v.w,v.z,v.x})
MATHS_SWIZZLE(3, wxy, {v.w,v.x,v.y})
MATHS_SWIZZLE(3, wxz, {v.w,v.x,v.z})

MATHS_SWIZZLE(4, xyzw, {v.x,v.y,v.z,v.w})
MATHS_SWIZZLE(4, xywz, {v.x,v.y,v.w,v.z})
MATHS_SWIZZLE(4, xzyw, {v.x,v.z,v.y,v.w})
MATHS_SWIZZLE(4, xzwy, {v.x,v.z,v.w,v.y})
MATHS_SWIZZLE(4, xwyz, {v.x,v.w,v.y,v.z})
MATHS_SWIZZLE(4, xwzy, {v.x,v.w,v.z,v.y})
MATHS_SWIZZLE(4, yxzw, {v.y,v.x,v.z,v.w})
MATHS_SWIZZLE(4, yxwz, {v.y,v.x,v.w,v.z})
MATHS_SWIZZLE(4, yzxw, {v.y,v.z,v.x,v.w})
MATHS_SWIZZLE(4, yzwx, {v.y,v.z,v.w,v.x})
MATHS_SWIZZLE(4, ywyz, {v.y,v.w,v.x,v.z})
MATHS_SWIZZLE(4, ywzx, {v.y,v.w,v.z,v.x})
MATHS_SWIZZLE(4, zyxw, {v.z,v.y,v.x,v.w})
MATHS_SWIZZLE(4, zywx, {v.z,v.y,v.w,v.x})
MATHS_SWIZZLE(4, zxyw, {v.z,v.x,v.y,v.w})
MATHS_SWIZZLE(4, zxwy, {v.z,v.x,v.w,v.y})
MATHS_SWIZZLE(4, zwyx, {v.z,v.w,v.y,v.x})
MATHS_SWIZZLE(4, zwxy, {v.z,v.w,v.x,v.y})
MATHS_SWIZZLE(4, wyzx, {v.w,v.y,v.z,v.x})
MATHS_SWIZZLE(4, wyxz, {v.w,v.y,v.x,v.z})
MATHS_SWIZZLE(4, wzyx, {v.w,v.z,v.y,v.x})
MATHS_SWIZZLE(4, wzxy, {v.w,v.z,v.x,v.y})
MATHS_SWIZZLE(4, wxyz, {v.w,v.x,v.y,v.z})
MATHS_SWIZZLE(4, wxzy, {v.w,v.x,v.z,v.y})

#undef MATHS_SWIZZLE

namespace detail
{

MATHS_CONSTANT_VALUE(zero_vector, LVec1F, {0})
MATHS_CONSTANT_VALUE(zero_vector, LVec1D, {0})
MATHS_CONSTANT_VALUE(zero_vector, LVec2F, {0})
MATHS_CONSTANT_VALUE(zero_vector, LVec2D, {0})
MATHS_CONSTANT_VALUE(zero_vector, LVec3F, {0})
MATHS_CONSTANT_VALUE(zero_vector, LVec3D, {0})
MATHS_CONSTANT_VALUE(zero_vector, LVec4F, {0})
MATHS_CONSTANT_VALUE(zero_vector, LVec4D, {0})

MATHS_CONSTANT_VALUE(zero_vector, LVec1u8, {0})
MATHS_CONSTANT_VALUE(zero_vector, LVec1i8, {0})
MATHS_CONSTANT_VALUE(zero_vector, LVec2u8, {0})
MATHS_CONSTANT_VALUE(zero_vector, LVec2i8, {0})
MATHS_CONSTANT_VALUE(zero_vector, LVec3u8, {0})
MATHS_CONSTANT_VALUE(zero_vector, LVec3i8, {0})
MATHS_CONSTANT_VALUE(zero_vector, LVec4u8, {0})
MATHS_CONSTANT_VALUE(zero_vector, LVec4i8, {0})

MATHS_CONSTANT_VALUE(zero_vector, LVec1u16, {0})
MATHS_CONSTANT_VALUE(zero_vector, LVec1i16, {0})
MATHS_CONSTANT_VALUE(zero_vector, LVec2u16, {0})
MATHS_CONSTANT_VALUE(zero_vector, LVec2i16, {0})
MATHS_CONSTANT_VALUE(zero_vector, LVec3u16, {0})
MATHS_CONSTANT_VALUE(zero_vector, LVec3i16, {0})
MATHS_CONSTANT_VALUE(zero_vector, LVec4u16, {0})
MATHS_CONSTANT_VALUE(zero_vector, LVec4i16, {0})

MATHS_CONSTANT_VALUE(zero_vector, LVec1u32, {0})
MATHS_CONSTANT_VALUE(zero_vector, LVec1i32, {0})
MATHS_CONSTANT_VALUE(zero_vector, LVec2u32, {0})
MATHS_CONSTANT_VALUE(zero_vector, LVec2i32, {0})
MATHS_CONSTANT_VALUE(zero_vector, LVec3u32, {0})
MATHS_CONSTANT_VALUE(zero_vector, LVec3i32, {0})
MATHS_CONSTANT_VALUE(zero_vector, LVec4u32, {0})
MATHS_CONSTANT_VALUE(zero_vector, LVec4i32, {0})

MATHS_CONSTANT_VALUE(zero_vector, LVec1u64, {0})
MATHS_CONSTANT_VALUE(zero_vector, LVec1i64, {0})
MATHS_CONSTANT_VALUE(zero_vector, LVec2u64, {0})
MATHS_CONSTANT_VALUE(zero_vector, LVec2i64, {0})
MATHS_CONSTANT_VALUE(zero_vector, LVec3u64, {0})
MATHS_CONSTANT_VALUE(zero_vector, LVec3i64, {0})
MATHS_CONSTANT_VALUE(zero_vector, LVec4u64, {0})
MATHS_CONSTANT_VALUE(zero_vector, LVec4i64, {0})

MATHS_CONSTANT_VALUE(one_vector, LVec1F, {1})
MATHS_CONSTANT_VALUE(one_vector, LVec1D, {1})
MATHS_CONSTANT_VALUE(one_vector, LVec2F, {1})
MATHS_CONSTANT_VALUE(one_vector, LVec2D, {1})
MATHS_CONSTANT_VALUE(one_vector, LVec3F, {1})
MATHS_CONSTANT_VALUE(one_vector, LVec3D, {1})
MATHS_CONSTANT_VALUE(one_vector, LVec4F, {1})
MATHS_CONSTANT_VALUE(one_vector, LVec4D, {1})

MATHS_CONSTANT_VALUE(one_vector, LVec1u8, {1})
MATHS_CONSTANT_VALUE(one_vector, LVec1i8, {1})
MATHS_CONSTANT_VALUE(one_vector, LVec2u8, {1})
MATHS_CONSTANT_VALUE(one_vector, LVec2i8, {1})
MATHS_CONSTANT_VALUE(one_vector, LVec3u8, {1})
MATHS_CONSTANT_VALUE(one_vector, LVec3i8, {1})
MATHS_CONSTANT_VALUE(one_vector, LVec4u8, {1})
MATHS_CONSTANT_VALUE(one_vector, LVec4i8, {1})

MATHS_CONSTANT_VALUE(one_vector, LVec1u16, {1})
MATHS_CONSTANT_VALUE(one_vector, LVec1i16, {1})
MATHS_CONSTANT_VALUE(one_vector, LVec2u16, {1})
MATHS_CONSTANT_VALUE(one_vector, LVec2i16, {1})
MATHS_CONSTANT_VALUE(one_vector, LVec3u16, {1})
MATHS_CONSTANT_VALUE(one_vector, LVec3i16, {1})
MATHS_CONSTANT_VALUE(one_vector, LVec4u16, {1})
MATHS_CONSTANT_VALUE(one_vector, LVec4i16, {1})

MATHS_CONSTANT_VALUE(one_vector, LVec1u32, {1})
MATHS_CONSTANT_VALUE(one_vector, LVec1i32, {1})
MATHS_CONSTANT_VALUE(one_vector, LVec2u32, {1})
MATHS_CONSTANT_VALUE(one_vector, LVec2i32, {1})
MATHS_CONSTANT_VALUE(one_vector, LVec3u32, {1})
MATHS_CONSTANT_VALUE(one_vector, LVec3i32, {1})
MATHS_CONSTANT_VALUE(one_vector, LVec4u32, {1})
MATHS_CONSTANT_VALUE(one_vector, LVec4i32, {1})

MATHS_CONSTANT_VALUE(one_vector, LVec1u64, {1})
MATHS_CONSTANT_VALUE(one_vector, LVec1i64, {1})
MATHS_CONSTANT_VALUE(one_vector, LVec2u64, {1})
MATHS_CONSTANT_VALUE(one_vector, LVec2i64, {1})
MATHS_CONSTANT_VALUE(one_vector, LVec3u64, {1})
MATHS_CONSTANT_VALUE(one_vector, LVec3i64, {1})
MATHS_CONSTANT_VALUE(one_vector, LVec4u64, {1})
MATHS_CONSTANT_VALUE(one_vector, LVec4i64, {1})

MATHS_CONSTANT_VALUE(forward_vector, LVec3F, {0, 0, -1})
MATHS_CONSTANT_VALUE(forward_vector, LVec3D, {0, 0, -1})
MATHS_CONSTANT_VALUE(backward_vector, LVec3F, {0, 0, 1})
MATHS_CONSTANT_VALUE(backward_vector, LVec3D, {0, 0, 1})
MATHS_CONSTANT_VALUE(right_vector, LVec3F, {1, 0, 0})
MATHS_CONSTANT_VALUE(right_vector, LVec3D, {1, 0, 0})
MATHS_CONSTANT_VALUE(left_vector, LVec3F, {-1, 0, 0})
MATHS_CONSTANT_VALUE(left_vector, LVec3D, {-1, 0, 0})
MATHS_CONSTANT_VALUE(up_vector, LVec3F, {0, 1, 0})
MATHS_CONSTANT_VALUE(up_vector, LVec3D, {0, 1, 0})
MATHS_CONSTANT_VALUE(down_vector, LVec3F, {0, -1, 0})
MATHS_CONSTANT_VALUE(down_vector, LVec3D, {0, -1, 0})

MATHS_CONSTANT_VALUE(unit_vector_2d, LVec2F, {0.70710678118654752440084436210484903928483593768847403658833986899536623923105351942519376716382078636750692311545614851246241802})
MATHS_CONSTANT_VALUE(unit_vector_2d, LVec2D, {0.70710678118654752440084436210484903928483593768847403658833986899536623923105351942519376716382078636750692311545614851246241802})

MATHS_CONSTANT_VALUE(unit_vector_x, LVec1F, {1})
MATHS_CONSTANT_VALUE(unit_vector_x, LVec1D, {1})
MATHS_CONSTANT_VALUE(unit_vector_x, LVec2F, {1, 0})
MATHS_CONSTANT_VALUE(unit_vector_x, LVec2D, {1, 0})
MATHS_CONSTANT_VALUE(unit_vector_x, LVec3F, {1, 0, 0})
MATHS_CONSTANT_VALUE(unit_vector_x, LVec3D, {1, 0, 0})
MATHS_CONSTANT_VALUE(unit_vector_x, LVec4F, {1, 0, 0, 0})
MATHS_CONSTANT_VALUE(unit_vector_x, LVec4D, {1, 0, 0, 0})

MATHS_CONSTANT_VALUE(unit_vector_y, LVec2F, {0, 1})
MATHS_CONSTANT_VALUE(unit_vector_y, LVec2D, {0, 1})
MATHS_CONSTANT_VALUE(unit_vector_y, LVec3F, {0, 1, 0})
MATHS_CONSTANT_VALUE(unit_vector_y, LVec3D, {0, 1, 0})
MATHS_CONSTANT_VALUE(unit_vector_y, LVec4F, {0, 1, 0, 0})
MATHS_CONSTANT_VALUE(unit_vector_y, LVec4D, {0, 1, 0, 0})

MATHS_CONSTANT_VALUE(unit_vector_z, LVec3F, {0, 0, 1})
MATHS_CONSTANT_VALUE(unit_vector_z, LVec3D, {0, 0, 1})
MATHS_CONSTANT_VALUE(unit_vector_z, LVec4F, {0, 0, 1, 0})
MATHS_CONSTANT_VALUE(unit_vector_z, LVec4D, {0, 0, 1, 0})

MATHS_CONSTANT_VALUE(unit_vector_w, LVec4F, {0, 0, 0, 1})
MATHS_CONSTANT_VALUE(unit_vector_w, LVec4D, {0, 0, 0, 1})

MATHS_CONSTANT_VALUE(identity, LQuatF, {1, 0, 0, 0})
MATHS_CONSTANT_VALUE(identity, LQuatD, {1, 0, 0, 0})

MATHS_CONSTANT_VALUE(identity, LMat4F, {1})
MATHS_CONSTANT_VALUE(identity, LMat4D, {1})

MATHS_CONSTANT_VALUE(identity, LTransformF, {
    .T = maths::zero_vector<decltype(LTransformF::T)>,
    .R = maths::identity<decltype(LTransformF::R)>,
    .S = maths::one_vector<decltype(LTransformF::S)>
    })
MATHS_CONSTANT_VALUE(identity, LTransformD, {
    .T = maths::zero_vector<decltype(LTransformD::T)>,
    .R = maths::identity<decltype(LTransformD::R)>,
    .S = maths::one_vector<decltype(LTransformD::S)>
    })

#undef MATHS_CONSTANT_VALUE

} /* ~Namespace detail */

///////////////////////////////////////////////////////////////////////////////
// Common
using glm::abs;
using glm::sign;
// TODO: sing_no_zero

using glm::floor;
using glm::trunc;
using glm::round;
template<glm::length_t L, typename T, glm::qualifier Q>
NODISCARD FORCEINLINE glm::vec<L, T, Q> round_even(glm::vec<L, T, Q> const& x) { return glm::roundEven(x); }
using glm::ceil;
using glm::fract;
using glm::mod;
using glm::modf;
using glm::min;
using glm::max;
using glm::clamp;

using glm::mix;

using glm::step;
using glm::smoothstep;

//# If a and b are equal.
inline constexpr bool eq(std::floating_point auto a, decltype(a) b) noexcept
{
    return a == b;
}
template<length_t L, typename T, qual_t Q>
inline constexpr bool eq(TVec<L,T,Q> const& a, TVec<L,T,Q> const& b) noexcept
{
    if constexpr (L == 1) { return maths::eq(a.x, b.x); }
    if constexpr (L == 2) { return maths::eq(a.x, b.x) && maths::eq(a.y, b.y); }
    if constexpr (L == 3) { return maths::eq(a.x, b.x) && maths::eq(a.y, b.y) && maths::eq(a.z, b.z); }
    if constexpr (L == 4) { return maths::eq(a.x, b.x) && maths::eq(a.y, b.y) && maths::eq(a.z, b.z) && maths::eq(a.w, b.w); }
    unreachable()
}

//# If a is equal to zero.
inline constexpr bool eq_zero(std::floating_point auto a) noexcept
{
    return a == 0;
}
template<length_t L, typename T, qual_t Q>
inline constexpr bool eq_zero(TVec<L,T,Q> const& a) noexcept
{
    if constexpr (L == 1) { return a.x == 0; }
    if constexpr (L == 2) { return a.x == 0 && a.y == 0; }
    if constexpr (L == 3) { return a.x == 0 && a.y == 0 && a.z == 0; }
    if constexpr (L == 4) { return a.x == 0 && a.y == 0 && a.z == 0 && a.w == 0; }
    unreachable()
}

//# If a and b are equal under consideration of the tolerance value epsilon.
#if JAFG_WITH_MSVC
inline constexpr bool eq_e(std::floating_point auto a, decltype(a) b, f64 e = small_number_d) noexcept
{
    return maths::abs(a - b) < static_cast<decltype(a)>(e);
}
#else /* JAFG_WITH_MSVC */
inline constexpr bool eq_e(std::floating_point auto a, decltype(a) b, decltype(a) e = static_cast<decltype(e)>(small_number_d)) noexcept
{
    return maths::abs(a - b) < e;
}
#endif /* !JAFG_WITH_MSVC */
template<length_t L, typename T, qual_t Q>
inline constexpr bool eq_e(TVec<L,T,Q> const& a, TVec<L,T,Q> const& b, T e = static_cast<T>(small_number_d)) noexcept
{
    if constexpr (L == 1) { return maths::eq_e(a.x, b.x, e); }
    if constexpr (L == 2) { return maths::eq_e(a.x, b.x, e) && maths::eq_e(a.y, b.y, e); }
    if constexpr (L == 3) { return maths::eq_e(a.x, b.x, e) && maths::eq_e(a.y, b.y, e) && maths::eq_e(a.z, b.z, e); }
    if constexpr (L == 4) { return maths::eq_e(a.x, b.x, e) && maths::eq_e(a.y, b.y, e) && maths::eq_e(a.z, b.z, e) && maths::eq_e(a.w, b.w, e); }
    unreachable()
}

//# If a is equal to zero under consideration of the tolerance value epsilon.
#if JAFG_WITH_MSVC
inline constexpr bool eq_zero_e(std::floating_point auto a, f64 e = small_number_d) noexcept
{
    return maths::abs(a) < static_cast<decltype(a)>(e);
}
#else /* JAFG_WITH_MSVC */
inline constexpr bool eq_zero_e(std::floating_point auto a, decltype(a) e = static_cast<decltype(e)>(small_number_d)) noexcept
{
    return maths::abs(a) < e;
}
#endif /* !JAFG_WITH_MSVC */

template<length_t L, typename T, qual_t Q>
inline constexpr bool eq_zero_e(TVec<L,T,Q> const& a, T e = static_cast<T>(small_number_d)) noexcept
{
    if constexpr (L == 1) { return maths::abs(a.x) < e; }
    if constexpr (L == 2) { return maths::abs(a.x) < e && maths::abs(a.y) < e; }
    if constexpr (L == 3) { return maths::abs(a.x) < e && maths::abs(a.y) < e && maths::abs(a.z) < e; }
    if constexpr (L == 4) { return maths::abs(a.x) < e && maths::abs(a.y) < e && maths::abs(a.z) < e && maths::abs(a.w) < e; }
    unreachable()
}

///////////////////////////////////////////////////////////////////////////////
// Exponential
inline constexpr decltype(auto) squared(auto&& value) noexcept { return value * value; }
inline constexpr decltype(auto) cubed(auto&& value) noexcept   { return value * value * value; }
inline constexpr decltype(auto) quartic(auto&& value) noexcept { return value * value * value * value; }
inline constexpr decltype(auto) quintic(auto&& value) noexcept { return value * value * value * value * value; }
inline constexpr decltype(auto) sextic(auto&& value) noexcept  { return value * value * value * value * value * value; }
//# For generic pow. But should be avoided when dealing with exponents less than seven due to performance.
using glm::pow;

using std::log10;
using glm::log2;
using glm::exp;
using glm::log;
using glm::sqrt;
using glm::inversesqrt;

///////////////////////////////////////////////////////////////////////////////
// Trigonometric
using glm::radians;
using glm::degrees;
using glm::sin;
using glm::cos;
using glm::tan;
using glm::asin;
using glm::acos;
using glm::atan;
using glm::sinh;
using glm::cosh;
using glm::tanh;
using glm::asinh;
using glm::acosh;
using glm::atanh;

///////////////////////////////////////////////////////////////////////////////
// Geometric
using glm::dot;
using glm::distance;
using glm::cross;
using glm::normalize;
template<length_t L, typename T,qual_t Q>
inline constexpr T squared_magnitude(TVec<L,T,Q> const& v) noexcept { return maths::dot(v, v); }
template<length_t L, typename T,qual_t Q>
inline constexpr T magnitude(TVec<L,T,Q> const& v) noexcept { return maths::sqrt(maths::squared_magnitude(v)); }

template<typename T,qual_t Q>
inline constexpr bool aabb(TRect2<T,Q> const& a, TRect2<T,Q> const& b) noexcept
{
    return !(a.Offset.x > b.Offset.x + b.Extent.x
          || a.Offset.x + a.Extent.x < b.Offset.x
          || a.Offset.y > b.Offset.y + b.Extent.y
          || a.Offset.y + a.Extent.y < b.Offset.y
          );
}
template<typename T,qual_t Q>
inline constexpr bool aabb_point(TRect2<T,Q> const& a, TVec2<T,Q> const& p) noexcept
{
    return !(p.x < a.Offset.x
          || p.x > a.Offset.x + a.Extent.x
          || p.y < a.Offset.y
          || p.y > a.Offset.y + a.Extent.y
          );
}

///////////////////////////////////////////////////////////////////////////////
// Matrix
using glm::translate;
using glm::rotate;
using glm::mat4_cast;
using glm::scale;
using glm::shear;
using glm::transpose;
using glm::determinant;
using glm::inverse;
template<typename T,qual_t Q>
inline constexpr TMat4<T,Q> model(TTrans<T,Q> const& trans) noexcept
{
    return maths::translate(maths::identity<TMat4<T,Q>>, trans.T)
        *  maths::mat4_cast(trans.R)
        *  maths::scale(maths::identity<TMat4<T,Q>>, trans.S);
}

///////////////////////////////////////////////////////////////////////////////
// Quaternion
//# Rotations from a quaternion.
using glm::pitch;
using glm::yaw;
using glm::roll;
//# Rotations from a vector.
template<typename T,qual_t Q> inline constexpr T pitch(TVec3<T,Q> const& v) noexcept { return v.x; }
template<typename T,qual_t Q> inline constexpr T yaw(TVec3<T,Q> const& v)   noexcept { return v.y; }
template<typename T,qual_t Q> inline constexpr T roll(TVec3<T,Q> const& v)  noexcept { return v.z; }
//# Rotations from a vector. Returns mutable l-values.
template<typename T,qual_t Q> inline constexpr T& pitch_l(TVec3<T,Q>* v) noexcept { check(v) return v->x; }
template<typename T,qual_t Q> inline constexpr T& yaw_l(TVec3<T,Q>* v)   noexcept { check(v) return v->y; }
template<typename T,qual_t Q> inline constexpr T& roll_l(TVec3<T,Q>* v)  noexcept { check(v) return v->z; }
//# Quaternions from Euler angles.
template<typename T,qual_t Q>
inline constexpr TQua<T,Q> rotator(TVec3<T,Q> const& rads) noexcept
{
    auto P{glm::angleAxis(maths::pitch(rads), maths::right_vector<LWorldVec3>)};
    auto Y{glm::angleAxis(-maths::yaw(rads),  maths::up_vector<LWorldVec3>)};
    auto R{glm::angleAxis(maths::roll(rads),  maths::forward_vector<LWorldVec3>)};
    return Y * P * R;
}
template<typename T> requires std::is_floating_point_v<T>
inline constexpr TQua<T,defaultp> rotator(T pitch, T yaw, T roll) noexcept
{
    auto P{glm::angleAxis(pitch, maths::right_vector<LWorldVec3>)};
    auto Y{glm::angleAxis(-yaw,  maths::up_vector<LWorldVec3>)};
    auto R{glm::angleAxis(roll,  maths::forward_vector<LWorldVec3>)};
    return Y * P * R;
}
template<typename T> requires (!std::is_floating_point_v<T>)
inline constexpr TQua<LWorldReal,defaultp> rotator(T pitch, T yaw, T roll) noexcept
{
    return maths::rotator<LWorldReal>(static_cast<LWorldReal>(pitch), static_cast<LWorldReal>(yaw), static_cast<LWorldReal>(roll));
}
template<typename T,qual_t Q>
inline constexpr TQua<T,Q> rotator_deg(TVec3<T,Q> const& degs) noexcept
{
    return maths::rotator(maths::radians(degs));
}
template<typename T> requires std::is_floating_point_v<T>
inline constexpr TQua<T,defaultp> rotator_deg(T pitch, T yaw, T roll) noexcept
{
    return maths::rotator(maths::radians(pitch), maths::radians(yaw), maths::radians(roll));
}
template<typename T> requires (!std::is_floating_point_v<T>)
inline constexpr TQua<LWorldReal,defaultp> rotator_deg(T pitch, T yaw, T roll) noexcept
{
    return maths::rotator_deg<LWorldReal>(static_cast<LWorldReal>(pitch), static_cast<LWorldReal>(yaw), static_cast<LWorldReal>(roll));
}
//# Euler angles from a quaternion.
template<typename T,qual_t Q>
inline constexpr TVec3<T,Q> euler_angles(TQua<T,Q> const& q) noexcept
{
    auto Result{glm::eulerAngles(q)};
    return TVec3<T,Q>{Result.x,-Result.y,Result.z};
}
template<typename T,qual_t Q>
inline constexpr TVec3<T,Q> euler_angles_deg(TQua<T,Q> const& q) noexcept
{
    return maths::degrees(maths::euler_angles(q));
}
template<typename T, qualifier Q>
inline constexpr TQua<T,Q> angle_axis(T a, TVec3<T,Q> const& v) noexcept { return glm::angleAxis(a, v); }

///////////////////////////////////////////////////////////////////////////////
// Conversions
using glm::to_string;

} /* ~Namespace maths */
