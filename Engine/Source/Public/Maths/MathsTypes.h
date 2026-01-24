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

template<maths::length_t C,maths::length_t R,typename T,maths::qual_t Q> using TMat = glm::mat<C,R,T,Q>;
using LMat2F = TMat<2,2,maths::single_precision,maths::defaultp>;
using LMat2D = TMat<2,2,maths::double_precision,maths::defaultp>;
using LMat3F = TMat<3,3,maths::single_precision,maths::defaultp>;
using LMat3D = TMat<3,3,maths::double_precision,maths::defaultp>;
using LMat4F = TMat<4,4,maths::single_precision,maths::defaultp>;
using LMat4D = TMat<4,4,maths::double_precision,maths::defaultp>;

//# Transformation object.
template<template<typename TReal,maths::qual_t>typename TRot,typename TReal,maths::qual_t Q>
struct TTrans final
{
    //# Translation.
    TVec3<TReal,Q> T;
    //# Rotation.
    TRot<TReal,Q>  R;
    //# Scale.
    TVec3<TReal,Q> S;
};

template<typename TReal,maths::qual_t Q> using TQuaTrans = TTrans<TQua,TReal,Q>;
template<typename TReal,maths::qual_t Q> using TVecTrans = TTrans<TVec3,TReal,Q>;
using LQuaTransformF = TQuaTrans<maths::single_precision,maths::defaultp>;
using LQuaTransformD = TQuaTrans<maths::double_precision,maths::defaultp>;
using LVecTransformF = TVecTrans<maths::single_precision,maths::defaultp>;
using LVecTransformD = TVecTrans<maths::double_precision,maths::defaultp>;

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
using LWorldTrans = TQuaTrans<LWorldReal,world_qual>;
