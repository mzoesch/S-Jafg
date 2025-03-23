// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

namespace Matrix
{

enum Type : uint8
{
    Identity, Zero
};

} /* ~Namespace Matrix */

namespace EVectorAxis
{

enum Type : uint8
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
LAL_API LSimpleString LexToString(const EVectorAxis::Type InAxis);


/*----------------------------------------------------------------------------
    Type Forwards.
----------------------------------------------------------------------------*/

template <typename T> struct TVector;
template <typename T> struct TVector2;
template <typename T> struct TVector4;
template <typename T> struct TIntVector;
template <typename T> struct TIntVector2;
template <typename T> struct TRotator;
template <typename T> struct TPlane;
template <typename T> struct TMatrix;
template <typename T> struct TMatrix3;
template <typename T> struct TTransform;


/*----------------------------------------------------------------------------
    Concrete Type Forwards.
----------------------------------------------------------------------------*/

using LVectorF          = TVector<float>;
using LVectorD          = TVector<double>;
using LVector2F         = TVector2<float>;
using LVector2D         = TVector2<double>;
using LVector4F         = TVector4<float>;
using LVector4D         = TVector4<double>;

using LInt8Vector       = TIntVector<int8>;
using LInt16Vector      = TIntVector<int16>;
using LInt32Vector      = TIntVector<int32>;
using LInt64Vector      = TIntVector<int64>;

using LInt8Vector2      = TIntVector2<int8>;
using LInt16Vector2     = TIntVector2<int16>;
using LInt32Vector2     = TIntVector2<int32>;
using LInt64Vector2     = TIntVector2<int64>;
using LuInt8Vector2     = TIntVector2<uint8>;
using LuInt16Vector2    = TIntVector2<uint16>;
using LuInt32Vector2    = TIntVector2<uint32>;
using LuInt64Vector2    = TIntVector2<uint64>;

using LRotatorF         = TRotator<float>;
using LRotatorD         = TRotator<double>;

using LPlaneF           = TPlane<float>;
using LPlaneD           = TPlane<double>;

using LMatrixF          = TMatrix<float>;
using LMatrixD          = TMatrix<double>;
using LMatrix3F         = TMatrix3<float>;
using LMatrix3D         = TMatrix3<double>;

using LTransformF       = TTransform<float>;
using LTransformD       = TTransform<double>;


/*----------------------------------------------------------------------------
    Aliases.
----------------------------------------------------------------------------*/

/*
 * Uses floats by default.
 * Uses integers with 32 bits of precision by default.
 */

typedef LVectorF         LVector3F;
typedef LVectorD         LVector3D;
typedef LVectorF         LVector;
typedef LVector3F        LVector3;
typedef LVector2F        LVector2;
typedef LVector4F        LVector4;
typedef LInt32Vector     LIntVector;
typedef LInt32Vector2    LIntVector2;
typedef LRotatorF        LRotator;
typedef LPlaneF          LPlane;
typedef LMatrixF         LMatrix4;
typedef LMatrixD         LMatrix4D;
typedef LMatrixF         LMatrix;
typedef LMatrix3F        LMatrix3;
typedef LTransformF      LTransform;
typedef LuInt8Vector2    LSize8;
typedef LuInt16Vector2   LSize16;
typedef LuInt32Vector2   LSize32;
typedef LuInt64Vector2   LSize64;
typedef LSize32          LSize;
typedef LInt32Vector2    LPoint;

} /* ~Namespace Jafg */
