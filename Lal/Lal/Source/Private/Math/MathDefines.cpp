// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"

/*----------------------------------------------------------------------------
    Globals.
----------------------------------------------------------------------------*/

template<> const LVectorF LVectorF::ZeroVector           { LVectorF( 0.0f,  0.0f,  0.0f) };
template<> const LVectorF LVectorF::OneVector            { LVectorF( 1.0f,  1.0f,  1.0f) };
template<> const LVectorF LVectorF::UpVector             { LVectorF( 0.0f,  0.0f,  1.0f) };
template<> const LVectorF LVectorF::DownVector           { LVectorF( 0.0f,  0.0f, -1.0f) };
template<> const LVectorF LVectorF::ForwardVector        { LVectorF( 1.0f,  0.0f,  0.0f) };
template<> const LVectorF LVectorF::BackwardVector       { LVectorF(-1.0f,  0.0f,  0.0f) };
template<> const LVectorF LVectorF::RightVector          { LVectorF( 0.0f,  1.0f,  0.0f) };
template<> const LVectorF LVectorF::LeftVector           { LVectorF( 0.0f, -1.0f,  0.0f) };
template<> const LVectorF LVectorF::UnitVectorX          { LVectorF( 1.0f,  0.0f,  0.0f) };
template<> const LVectorF LVectorF::UnitVectorY          { LVectorF( 0.0f,  1.0f,  0.0f) };
template<> const LVectorF LVectorF::UnitVectorZ          { LVectorF( 0.0f,  0.0f,  1.0f) };

template<> const LVectorD LVectorD::ZeroVector           { LVectorD( 0.0,  0.0,  0.0) };
template<> const LVectorD LVectorD::OneVector            { LVectorD( 1.0,  1.0,  1.0) };
template<> const LVectorD LVectorD::UpVector             { LVectorD( 0.0,  0.0,  1.0) };
template<> const LVectorD LVectorD::DownVector           { LVectorD( 0.0,  0.0, -1.0) };
template<> const LVectorD LVectorD::ForwardVector        { LVectorD( 1.0,  0.0,  0.0) };
template<> const LVectorD LVectorD::BackwardVector       { LVectorD(-1.0,  0.0,  0.0) };
template<> const LVectorD LVectorD::RightVector          { LVectorD( 0.0,  1.0,  0.0) };
template<> const LVectorD LVectorD::LeftVector           { LVectorD( 0.0, -1.0,  0.0) };
template<> const LVectorD LVectorD::UnitVectorX          { LVectorD( 1.0,  0.0,  0.0) };
template<> const LVectorD LVectorD::UnitVectorY          { LVectorD( 0.0,  1.0,  0.0) };
template<> const LVectorD LVectorD::UnitVectorZ          { LVectorD( 0.0,  0.0,  1.0) };

template<> const LVector2F LVector2F::ZeroVector         { LVector2F( 0.0f,  0.0f) };
template<> const LVector2F LVector2F::OneVector          { LVector2F( 1.0f,  1.0f) };
template<> const LVector2F LVector2F::UnitVector         { LVector2F( 0.70710678118654752440084436210484903928483593768847403658833986899536623923105351942519376716382078636750692311545614851246241802f, 0.70710678118654752440084436210484903928483593768847403658833986899536623923105351942519376716382078636750692311545614851246241802f) };
template<> const LVector2F LVector2F::UnitVectorX        { LVector2F( 1.0f,  0.0f) };
template<> const LVector2F LVector2F::UnitVectorY        { LVector2F( 0.0f,  1.0f) };

template<> const LVector2D LVector2D::ZeroVector         { LVector2D( 0.0,  0.0) };
template<> const LVector2D LVector2D::OneVector          { LVector2D( 1.0,  1.0) };
template<> const LVector2D LVector2D::UnitVector         { LVector2D( 0.70710678118654752440084436210484903928483593768847403658833986899536623923105351942519376716382078636750692311545614851246241802, 0.70710678118654752440084436210484903928483593768847403658833986899536623923105351942519376716382078636750692311545614851246241802) };
template<> const LVector2D LVector2D::UnitVectorX        { LVector2D( 1.0,  0.0) };
template<> const LVector2D LVector2D::UnitVectorY        { LVector2D( 0.0,  1.0) };

template<> const LInt8Vector2 LInt8Vector2::ZeroVector   { LInt8Vector2(0, 0) };
template<> const LInt8Vector2 LInt8Vector2::OneVector    { LInt8Vector2(1, 1) };

template<> const LInt16Vector2 LInt16Vector2::ZeroVector { LInt16Vector2(0, 0) };
template<> const LInt16Vector2 LInt16Vector2::OneVector  { LInt16Vector2(1, 1) };

template<> const LInt32Vector2 LInt32Vector2::ZeroVector { LInt32Vector2(0, 0) };
template<> const LInt32Vector2 LInt32Vector2::OneVector  { LInt32Vector2(1, 1) };

template<> const LInt64Vector2 LInt64Vector2::ZeroVector { LInt64Vector2(0, 0) };
template<> const LInt64Vector2 LInt64Vector2::OneVector  { LInt64Vector2(1, 1) };
