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

using LRotatorF         = TRotator<float>;
using LRotatorD         = TRotator<double>;

using LPlaneF           = TPlane<float>;
using LPlaneD           = TPlane<double>;

using LMatrixF          = TMatrix<float>;
using LMatrixD          = TMatrix<double>;

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
typedef LMatrixF         LMatrix;
typedef LTransformF      LTransform;

} /* ~Namespace Jafg */
