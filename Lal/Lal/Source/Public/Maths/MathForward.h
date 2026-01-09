// Copyright mzoesch. All rights reserved.

#pragma once

namespace Matrix
{

enum Type : u8
{
    Identity, Zero
};

} /* ~Namespace Matrix */

namespace EVectorAxis
{

enum Type : u8
{
    X = 0x00 << 0,
    Y = 0x01 << 1,
    Z = 0x01 << 2,
    W = 0x01 << 3,
    V = 0x01 << 4,
    U = 0x01 << 5,
    T = 0x01 << 6,
    S = 0x01 << 7,
};

} /* ~Namespace EVectorAxis */
ENUM_CLASS_FLAGS(EVectorAxis::Type)
LAL_API LString LexToString(const EVectorAxis::Type InAxis);


/*----------------------------------------------------------------------------
    Type Forwards.
----------------------------------------------------------------------------*/

template <typename T> struct TVector2;
template <typename T> struct TVector3;
template <typename T> struct TVector4;
template <typename T> struct TIntVector;
template <typename T> struct TIntVector2;
template <typename T> struct TRotator;
template <typename T> struct TPlane;
template <typename T> struct TMatrix3;
template <typename T> struct TMatrix4;
template <typename T> struct TTransform;


/*----------------------------------------------------------------------------
    Concrete Type Forwards.
----------------------------------------------------------------------------*/

using LVector2F         = TVector2<f32>;
using LVector2D         = TVector2<f64>;
using LVector3F         = TVector3<f32>;
using LVector3D         = TVector3<f64>;
using LVector4F         = TVector4<f32>;
using LVector4D         = TVector4<f64>;

using Li8Vector         = TIntVector<i8>;
using Li16Vector        = TIntVector<i16>;
using Li32Vector        = TIntVector<i32>;
using Li64Vector        = TIntVector<i64>;

using Li8Vector2        = TIntVector2<i8>;
using Li16Vector2       = TIntVector2<i16>;
using Li32Vector2       = TIntVector2<i32>;
using Li64Vector2       = TIntVector2<i64>;
using Lu8Vector2        = TIntVector2<u8>;
using Lu16Vector2       = TIntVector2<u16>;
using Lu32Vector2       = TIntVector2<u32>;
using Lu64Vector2       = TIntVector2<u64>;

using LRotatorF         = TRotator<f32>;
using LRotatorD         = TRotator<f64>;

using LPlaneF           = TPlane<f32>;
using LPlaneD           = TPlane<f64>;

using LMatrix3F         = TMatrix3<f32>;
using LMatrix3D         = TMatrix3<f64>;
using LMatrix4F         = TMatrix4<f32>;
using LMatrix4D         = TMatrix4<f64>;

using LTransformF       = TTransform<f32>;
using LTransformD       = TTransform<f64>;


/*----------------------------------------------------------------------------
    Aliases.
----------------------------------------------------------------------------*/

/*
 * Uses floats by default.
 * Uses integers with 32 bits of precision by default.
 */

typedef LVector2F        LVector2;

template <typename T> using TVector = TVector3<T>;
typedef LVector3F        LVectorF;
typedef LVector3D        LVectorD;
typedef LVector3F        LVector3;
typedef LVectorF         LVector;

typedef LVector4F        LVector4;
typedef Li32Vector       LIntVector;
typedef Li32Vector2      LIntVector2;
typedef Lu32Vector2      LUIntVector2;
typedef LRotatorF        LRotator;
typedef LPlaneF          LPlane;

template <typename T> using TMatrix = TMatrix4<T>;
typedef LMatrix4F        LMatrixF;
typedef LMatrix4D        LMatrixD;
typedef LMatrix4F        LMatrix4;
typedef LMatrixF         LMatrix;

typedef LTransformF      LTransform;
typedef Li32Vector2      LPoint;
