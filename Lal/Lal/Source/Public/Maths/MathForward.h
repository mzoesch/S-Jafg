// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

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

using LVector2F         = TVector2<float>;
using LVector2D         = TVector2<double>;
using LVector3F         = TVector3<float>;
using LVector3D         = TVector3<double>;
using LVector4F         = TVector4<float>;
using LVector4D         = TVector4<double>;

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

using LRotatorF         = TRotator<float>;
using LRotatorD         = TRotator<double>;

using LPlaneF           = TPlane<float>;
using LPlaneD           = TPlane<double>;

using LMatrix3F         = TMatrix3<float>;
using LMatrix3D         = TMatrix3<double>;
using LMatrix4F         = TMatrix4<float>;
using LMatrix4D         = TMatrix4<double>;

using LTransformF       = TTransform<float>;
using LTransformD       = TTransform<double>;


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
typedef LRotatorF        LRotator;
typedef LPlaneF          LPlane;

template <typename T> using TMatrix = TMatrix4<T>;
typedef LMatrix4F        LMatrixF;
typedef LMatrix4D        LMatrixD;
typedef LMatrix4F        LMatrix4;
typedef LMatrixF         LMatrix;

typedef LTransformF      LTransform;
typedef Lu8Vector2       LSize8;
typedef Lu16Vector2      LSize16;
typedef Lu32Vector2      LSize32;
typedef Lu64Vector2      LSize64;
typedef LSize32          LSize;
typedef Li32Vector2      LPoint;

} /* ~Namespace Jafg */
