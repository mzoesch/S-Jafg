// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{


/*----------------------------------------------------------------------------
    Type Forwards.
----------------------------------------------------------------------------*/

template <typename T> struct TVector;
template <typename T> struct TVector2;
template <typename T> struct TIntVector;
template <typename T> struct TIntVector2;
template <typename T> struct TRotator;


/*----------------------------------------------------------------------------
    Concrete Type Forwards.
----------------------------------------------------------------------------*/

using LVectorF          = TVector<float>;
using LVectorD          = TVector<double>;
using LVector2F         = TVector2<float>;
using LVector2D         = TVector2<double>;

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


/*----------------------------------------------------------------------------
    Aliases.
----------------------------------------------------------------------------*/

/*
 * Uses floats by default.
 * Uses integers with 32 bits of precision by default.
 */

typedef LVectorF         LVector;
typedef LVector2F        LVector2;
typedef LInt32Vector     LIntVector;
typedef LInt32Vector2    LIntVector2;
typedef LRotatorF        LRotator;

} /* ~Namespace Jafg */
