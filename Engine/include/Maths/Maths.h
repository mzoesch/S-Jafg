// Copyright mzoesch. All rights reserved.

#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_RIGHT_HANDED
#define GLM_FORCE_XYZW_ONLY
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

template<typename T>
concept color = requires(T t)
{
    typename T::value_type;
    { t.r } -> std::same_as<typename T::value_type&>;
    { t.g } -> std::same_as<typename T::value_type&>;
    { t.b } -> std::same_as<typename T::value_type&>;
    { t.a } -> std::same_as<typename T::value_type&>;
};
template<typename T>
inline constexpr bool is_color_v{color<T>};

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

template<maths::length_t L,typename TReal,maths::qual_t Q>
struct TRect final
{
    TVec<L,TReal,Q> Offset;
    TVec<L,TReal,Q> Extent;
};
template<typename TReal,maths::qual_t Q> using TRect1 = TRect<1,TReal,Q>;
using LRect1F = TRect1<maths::single_precision,maths::defaultp>;
using LRect1D = TRect1<maths::double_precision,maths::defaultp>;
template<typename TReal,maths::qual_t Q> using TRect2 = TRect<2,TReal,Q>;
using LRect2F = TRect2<maths::single_precision,maths::defaultp>;
using LRect2D = TRect2<maths::double_precision,maths::defaultp>;
template<typename TReal,maths::qual_t Q> using TRect3 = TRect<3,TReal,Q>;
using LRect3F = TRect3<maths::single_precision,maths::defaultp>;
using LRect3D = TRect3<maths::double_precision,maths::defaultp>;

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
    std::unreachable();
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
    std::unreachable();
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
    std::unreachable();
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
    std::unreachable();
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
inline constexpr bool aabb(TRect1<T,Q> const& a, TRect1<T,Q> const& b) noexcept
{
    return !(a.Offset.x > b.Offset.x + b.Extent.x
          || a.Offset.x + a.Extent.x < b.Offset.x
          );
}
template<typename T,qual_t Q>
inline constexpr bool aabb_point(TRect1<T,Q> const& a, TVec1<T,Q> const& p) noexcept
{
    return !(p.x < a.Offset.x
          || p.x > a.Offset.x + a.Extent.x
          );
}
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
template<typename T,qual_t Q>
inline constexpr bool aabb(TRect3<T,Q> const& a, TRect3<T,Q> const& b) noexcept
{
    return !(a.Offset.x > b.Offset.x + b.Extent.x
          || a.Offset.x + a.Extent.x < b.Offset.x
          || a.Offset.y > b.Offset.y + b.Extent.y
          || a.Offset.y + a.Extent.y < b.Offset.y
          || a.Offset.z > b.Offset.z + b.Extent.z
          || a.Offset.z + a.Extent.z < b.Offset.z
          );
}
template<typename T,qual_t Q>
inline constexpr bool aabb_point(TRect3<T,Q> const& a, TVec3<T,Q> const& p) noexcept
{
    return !(p.x < a.Offset.x
          || p.x > a.Offset.x + a.Extent.x
          || p.y < a.Offset.y
          || p.y > a.Offset.y + a.Extent.y
          || p.z < a.Offset.z
          || p.z > a.Offset.z + a.Extent.z
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
    auto P{glm::angleAxis(maths::pitch(rads), maths::right_vector<TVec3<T,Q>>)};
    auto Y{glm::angleAxis(-maths::yaw(rads),  maths::up_vector<TVec3<T,Q>>)};
    auto R{glm::angleAxis(maths::roll(rads),  maths::forward_vector<TVec3<T,Q>>)};
    return Y * P * R;
}
template<typename T> requires std::is_floating_point_v<T>
inline constexpr TQua<T,defaultp> rotator(T pitch, T yaw, T roll) noexcept
{
    auto P{glm::angleAxis(pitch, maths::right_vector<TVec3<T,world_qual>>)};
    auto Y{glm::angleAxis(-yaw,  maths::up_vector<TVec3<T,world_qual>>)};
    auto R{glm::angleAxis(roll,  maths::forward_vector<TVec3<T,world_qual>>)};
    return Y * P * R;
}
template<typename T> requires(!std::is_floating_point_v<T>)
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
template<typename T> requires(!std::is_floating_point_v<T>)
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

inline constexpr std::array<f32,256> srgb_to_linear_table{
    0.00000000000000000000000000000000,
    0.00030352698354883751513319523063,0.00060705396709767503026639046126,0.00091058095064651249118947706762,
    0.00121410793419535006053278092253,0.00151763491774418741303565028034,0.00182116190129302498237895413524,
    0.00212468888484186255172225799015,0.00242821586839070012106556184506,0.00273174285193953725672799670576,
    0.00303526983548837482607130056067,0.00334653576389916081537667658097,0.00367650732404743588524476116675,
    0.00402471701849630662112256018759,0.00439144203741029335447132453396,0.00477695348069372919402875155015,
    0.00518151670233838596013598021273,0.00560539162420272286252442839327,0.00604883302285705391071468639552,
    0.00651209079259447518733816551162,0.00699541018726538687405591687707,0.00749903204322617533694739222483,
    0.00802319298538499425799130904124,0.00856812561806930689245476173710,0.00913405870222078718201341729355,
    0.00972121732023784913823138964517,0.01032982302962693645087544069838,0.01096009400648824579083129293622,
    0.01161224517974388491248216581653,0.01228648835691587178076122910397,0.01298303234217301239949993174605,
    0.01370208304728968637387076512368,0.01444384359609254472878170361128,0.01520851442291270942674952237894,
    0.01599629336550963121243995601617,0.01680737575288738377965458425933,0.01764195448838407759284407916311,
    0.01850022012837969700993490107521,0.01938236095693572288989336982468,0.02028856305665240056246290123454,
    0.02121901037600355463541568212804,0.02217388479338738144219789205636,0.02315336617811040997860771994965,
    0.02415763244850475596758876406511,0.02518685962736163033715008907620,0.02624122189484989764451938754064,
    0.02732089163907489362537184263147,0.02842603950442079349669732835082,0.02955683443780880020890222681373,
    0.03071344373299363453289068104368,0.03189603307301153156672057775722,0.03310476657088505525461386014285,
    0.03433980680868217033596678788854,0.03560131487502034286496410686595,0.03688945040110003931177473646130,
    0.03820437159534650212178163997123,0.03954623527673283706063145359622,0.04091519690685319066147940247902,
    0.04231141062080967518710394870141,0.04373502925697346499500639538383,0.04518620438567554076225718517890,
    0.04666508633688009471951474438356,0.04817182422688941895172476392872,0.04970656598412723226054765746085,
    0.05126945837404323774721959239287,0.05286064702318024610683266928390,0.05448027644244236855630703075803,
    0.05612849004960009102749296516777,0.05780543019106722940625431306216,0.05951123816298119900691787620417,
    0.06124605423161760819894539054076,0.06301001765316767422309141011283,0.06480326669290577268345288075579,
    0.06662593864377289176736951503699,0.06847816984440016629953618121363,0.07036009569659587570278347357089,
    0.07227185068231747888667371171323,0.07421356838014962764571436082406,0.07618538148130785114808816160803,
    0.07818742180518632733932093969997,0.08021982031446832361876175809812,0.08228270712981479440006893355530,
    0.08437621154414881585736907254613,0.08650046203654976340047255689569,0.08865558628577294153494392503490,
    0.09084171118340768347199087884292,0.09305896284668745133128453517202,0.09530746663096470450149411135499,
    0.09758734714186245717826295731356,0.09989872824711389098784053430791,0.10224173308810131921564590129492,
    0.10461648409110418933565256338625,0.10702310297826761464889955277613,0.10946171077829933149239138856501,
    0.11193242783690560138154523883713,0.11443537382697373250284300638668,0.11697066775851083786363915351103,
    0.11953842798834561633736228714042,0.12213877222960187185485381178296,0.12477181756095048759114263248193,
    0.12743768043564743241979897447891,0.13013647669036429443956137674832,0.13286832155381797515580899471388,
    0.13563332965520566442130245832232,0.13843161503245182686328007548582,0.14126329114027164068900788151950,
    0.14412847085805777225253621054435,0.14702726649759498278591252073966,0.14995978981060856249918344929029,
    0.15292615199615017251844051315857,0.15592646370782739517757420344424,0.15896083506088040659953719568875,
    0.16202937563911098961533241435973,0.16513219450166760626785844578990,0.16826940018969074874810587516549,
    0.17144110073282259332216881375643,0.17464740365558503731868711383868,0.17788841598362911677888575923134,
    0.18116424424986021790573431644589,0.18447499450044099744694392484234,0.18782077230067786843825672349340,
    0.19120168274079138437393510230322,0.19461783044157579536381774687470,0.19806931955994885874439148665260,
    0.20155625379439706668094345332065,0.20507873639031692913903270891751,0.20863687014525575391665768165694,
    0.21223075741405522665061766929284,0.21586050011389926184257603836159,0.21952619972926920577371845411108,
    0.22322795731680850073175292891392,0.22696587351009836486248616438388,0.23074004852434915058267961285310,
    0.23455058216100521661928723915480,0.23839757381227100196952051192056,0.24228112246555486009036428640684,
    0.24620132670783548278770069828170,0.25015828472995343956242209060292,0.25415209433082674994608396445983,
    0.25818285292159581789661615403020,0.26225065752969622945300898209098,0.26635560480286246676229211516329,
    0.27049779101306581363672876250348,0.27467731206038464852881020306086,0.27889426347681040008197328461392,
    0.28314874042999210734805615174992,0.28744083772691747524774541489023,0.29177064981753586536683542362880,
    0.29613827079832111266455285658594,0.30054379441577649956229834060650,0.30498731406988627279375236867054,
    0.30946892281750854047928100953868,0.31398871337571754303041871025926,0.31854677812509185619305185355188,
    0.32314320911295074667535232038063,0.32777809805654217756298862695985,0.33245153634617935489714568575437,
    0.33716361504833036732975415361580,0.34191442490866091885592936705507,0.34670405635502959951210755207285,
    0.35153259950043935777586057156441,0.35640014414594350933285227256420,0.36130677978350950185770784628403,
    0.36625259559883949211922526956187,0.37123768047414912318870960916684,0.37626212299090650015287451424228,
    0.38132601143253014308598380921467,0.38642943378704902590570213760657,0.39157247774972325782272264405037,
    0.39675523072562685067410370720609,0.40197777983219579178708613653725,0.40724021190173670392553617602971,
    0.41254261348390375285788422843325,0.41788507084813747427531893663399,0.42326766998607168179802329177619,
    0.42869049661390662420146213662520,0.43415363617474894697423337674991,0.43965717384091879127439028707158,
    0.44520119451622786055366987056914,0.45078578283822345884601645593648,0.45641102318040466245818720381067,
    0.46207699965440707234876072106999,0.46778379611215897826426157735114,0.47353149614800954525506426762149,
    0.47932018310082680212502737049363,0.48514994005607037230731748422841,0.49102084984783561649734906495723,
    0.49693299506087040828816725479555,0.50288645803256870614461604418466,0.50888132085493376077778293620213,
    0.51491766537652139401615158931236,0.52099557320435430085581174353138,0.52711512570581309233830324956216,
    0.53327640401050524499027005731477,0.53947948901210718286591827563825,0.54572446137018659761963590426603,
    0.55201140151200012162746588728623,0.55834038963426790846966696335585,0.56471150570492922859955342573812,
    0.57112482946487308499428081631777,0.57758044042965062114802776704892,0.58407841789116410335935825060005,
    0.59061884091933691820486274082214,0.59720178836376336395375119536766,0.60382733885533779183418801039807,
    0.61049557080786476248590588511433,0.61720656241965110577751829623594,0.62396039167507610923024685689597,
    0.63075713634614682945311869843863,0.63759687399403264240760336178937,0.64447968197058214112615814883611,
    0.65140563741982415724152133407188,0.65837481727944846543465473587275,0.66538729828227205498336616074084,
    0.67244315695768752672023538252688,0.67954246963309383744444858166389,0.68668531243531349961273235749104,
    0.69387176129198990803814695027540,0.70110189193297312026942336160573,0.70837577989168676317888184712501,
    0.71569350050648072869563520725933,0.72305512892196932561716948839603,0.73046074009035366625397500683903,
    0.73791040877273084142018433340127,0.74540420954038744127956306328997,0.75294221677607786613606322134729,
    0.76052450467529242317255011585075,0.76815114724750699348732041471521,0.77582221831742359530181829541107,
    0.78353779152619351666686497992487,0.79129794033263023411706171827973,0.79910273801440900864889727017726,
    0.80695225766925160471032540954184,0.81484657221610123922772572768736,0.82278575439628354182275415951153,
    0.83076987677465463644210785787436,0.83879901174074000813618567917729,0.84687323150985804876000884178211,
    0.85499260812423383271152488305233,0.86315721345410234555117767740740,0.87136711919879716869985486482619,
    0.87962239688783172564257029080181,0.88792311788196631727743124429253,0.89626935337426638650271115693613,
    0.90466117439114956955137358818320,0.91309865179341920260469578352058,0.92158185627729460875912081974093,
    0.93011085837542373244701821022318,0.93868572845788800229627213411732,0.94730653673319986651790713949595,
    0.95597335324928611743899864450213,0.96468624789446510980894800013630,0.97344529039841254380860391393071,
    0.98225055033311714502985978469951,0.99110209711382979413940574886510,1.00000000000000000000000000000000,
    };

template<std::floating_point T>
inline constexpr T srgb_to_linear(T srgb, T gamma = 2.4) noexcept
{
    if (srgb > static_cast<T>(0.04045))
    {
        return std::pow((srgb + static_cast<T>(0.055)) / static_cast<T>(1.055), gamma);
    }
    return srgb * (static_cast<T>(1.0) / static_cast<T>(12.92));
}

template<std::floating_point T>
inline constexpr T srgb_to_linear_22(T srgb, T gamma = 2.4) noexcept
{
    return pow(srgb, gamma);
}

} /* ~Namespace maths */
