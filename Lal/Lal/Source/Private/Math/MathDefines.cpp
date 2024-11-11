// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"

/*----------------------------------------------------------------------------
    Globals.
----------------------------------------------------------------------------*/

namespace Jafg
{

template<> const LVectorF LVectorF::ZeroVector                      { LVectorF( 0.0f,  0.0f,  0.0f) };
template<> const LVectorF LVectorF::OneVector                       { LVectorF( 1.0f,  1.0f,  1.0f) };
template<> const LVectorF LVectorF::UpVector                        { LVectorF( 0.0f,  0.0f,  1.0f) };
template<> const LVectorF LVectorF::DownVector                      { LVectorF( 0.0f,  0.0f, -1.0f) };
template<> const LVectorF LVectorF::ForwardVector                   { LVectorF( 1.0f,  0.0f,  0.0f) };
template<> const LVectorF LVectorF::BackwardVector                  { LVectorF(-1.0f,  0.0f,  0.0f) };
template<> const LVectorF LVectorF::RightVector                     { LVectorF( 0.0f,  1.0f,  0.0f) };
template<> const LVectorF LVectorF::LeftVector                      { LVectorF( 0.0f, -1.0f,  0.0f) };
template<> const LVectorF LVectorF::UnitVectorX                     { LVectorF( 1.0f,  0.0f,  0.0f) };
template<> const LVectorF LVectorF::UnitVectorY                     { LVectorF( 0.0f,  1.0f,  0.0f) };
template<> const LVectorF LVectorF::UnitVectorZ                     { LVectorF( 0.0f,  0.0f,  1.0f) };

template<> const LVectorD LVectorD::ZeroVector                      { LVectorD( 0.0,  0.0,  0.0) };
template<> const LVectorD LVectorD::OneVector                       { LVectorD( 1.0,  1.0,  1.0) };
template<> const LVectorD LVectorD::UpVector                        { LVectorD( 0.0,  0.0,  1.0) };
template<> const LVectorD LVectorD::DownVector                      { LVectorD( 0.0,  0.0, -1.0) };
template<> const LVectorD LVectorD::ForwardVector                   { LVectorD( 1.0,  0.0,  0.0) };
template<> const LVectorD LVectorD::BackwardVector                  { LVectorD(-1.0,  0.0,  0.0) };
template<> const LVectorD LVectorD::RightVector                     { LVectorD( 0.0,  1.0,  0.0) };
template<> const LVectorD LVectorD::LeftVector                      { LVectorD( 0.0, -1.0,  0.0) };
template<> const LVectorD LVectorD::UnitVectorX                     { LVectorD( 1.0,  0.0,  0.0) };
template<> const LVectorD LVectorD::UnitVectorY                     { LVectorD( 0.0,  1.0,  0.0) };
template<> const LVectorD LVectorD::UnitVectorZ                     { LVectorD( 0.0,  0.0,  1.0) };

template<> const LVector2F LVector2F::ZeroVector                    { LVector2F( 0.0f,  0.0f) };
template<> const LVector2F LVector2F::OneVector                     { LVector2F( 1.0f,  1.0f) };
template<> const LVector2F LVector2F::UnitVector
{ LVector2F(
0.70710678118654752440084436210484903928483593768847403658833986899536623923105351942519376716382078636750692311545614851246241802f,
0.70710678118654752440084436210484903928483593768847403658833986899536623923105351942519376716382078636750692311545614851246241802f
) };
template<> const LVector2F LVector2F::UnitVectorX                   { LVector2F( 1.0f,  0.0f) };
template<> const LVector2F LVector2F::UnitVectorY                   { LVector2F( 0.0f,  1.0f) };

template<> const LVector2D LVector2D::ZeroVector                    { LVector2D( 0.0,  0.0) };
template<> const LVector2D LVector2D::OneVector                     { LVector2D( 1.0,  1.0) };
template<> const LVector2D LVector2D::UnitVector
{ LVector2D(
0.70710678118654752440084436210484903928483593768847403658833986899536623923105351942519376716382078636750692311545614851246241802,
0.70710678118654752440084436210484903928483593768847403658833986899536623923105351942519376716382078636750692311545614851246241802
) };
template<> const LVector2D LVector2D::UnitVectorX                   { LVector2D( 1.0,  0.0) };
template<> const LVector2D LVector2D::UnitVectorY                   { LVector2D( 0.0,  1.0) };

template<> const LVector4F LVector4F::ZeroVector                    { LVector4F( 0.0f, 0.0f, 0.0f, 0.0f) };
template<> const LVector4F LVector4F::IdentityVector                { LVector4F( 0.0f, 0.0f, 0.0f, 1.0f) };
template<> const LVector4F LVector4F::OneVector                     { LVector4F( 1.0f, 1.0f, 1.0f, 1.0f) };
template<> const LVector4F LVector4F::UnitVectorX                   { LVector4F( 1.0f, 0.0f, 0.0f, 0.0f) };
template<> const LVector4F LVector4F::UnitVectorY                   { LVector4F( 0.0f, 1.0f, 0.0f, 0.0f) };
template<> const LVector4F LVector4F::UnitVectorZ                   { LVector4F( 0.0f, 0.0f, 1.0f, 0.0f) };
template<> const LVector4F LVector4F::UnitVectorW                   { LVector4F( 0.0f, 0.0f, 0.0f, 1.0f) };

template<> const LVector4D LVector4D::ZeroVector                    { LVector4D( 0.0, 0.0, 0.0, 0.0) };
template<> const LVector4D LVector4D::IdentityVector                { LVector4D( 0.0, 0.0, 0.0, 1.0) };
template<> const LVector4D LVector4D::OneVector                     { LVector4D( 1.0, 1.0, 1.0, 1.0) };
template<> const LVector4D LVector4D::UnitVectorX                   { LVector4D( 1.0, 0.0, 0.0, 0.0) };
template<> const LVector4D LVector4D::UnitVectorY                   { LVector4D( 0.0, 1.0, 0.0, 0.0) };
template<> const LVector4D LVector4D::UnitVectorZ                   { LVector4D( 0.0, 0.0, 1.0, 0.0) };
template<> const LVector4D LVector4D::UnitVectorW                   { LVector4D( 0.0, 0.0, 0.0, 1.0) };

template<> const LInt8Vector LInt8Vector::ZeroVector                { LInt8Vector( 0,  0,  0) };
template<> const LInt8Vector LInt8Vector::OneVector                 { LInt8Vector( 1,  1,  1) };
template<> const LInt8Vector LInt8Vector::UpVector                  { LInt8Vector( 0,  0,  1) };
template<> const LInt8Vector LInt8Vector::DownVector                { LInt8Vector( 0,  0, -1) };
template<> const LInt8Vector LInt8Vector::ForwardVector             { LInt8Vector( 1,  0,  0) };
template<> const LInt8Vector LInt8Vector::BackwardVector            { LInt8Vector(-1,  0,  0) };
template<> const LInt8Vector LInt8Vector::RightVector               { LInt8Vector( 0,  1,  0) };
template<> const LInt8Vector LInt8Vector::LeftVector                { LInt8Vector( 0, -1,  0) };
template<> const LInt8Vector LInt8Vector::UnitVectorX               { LInt8Vector( 1,  0,  0) };
template<> const LInt8Vector LInt8Vector::UnitVectorY               { LInt8Vector( 0,  1,  0) };
template<> const LInt8Vector LInt8Vector::UnitVectorZ               { LInt8Vector( 0,  0,  1) };

template<> const LInt16Vector LInt16Vector::ZeroVector              { LInt16Vector( 0,  0,  0) };
template<> const LInt16Vector LInt16Vector::OneVector               { LInt16Vector( 1,  1,  1) };
template<> const LInt16Vector LInt16Vector::UpVector                { LInt16Vector( 0,  0,  1) };
template<> const LInt16Vector LInt16Vector::DownVector              { LInt16Vector( 0,  0, -1) };
template<> const LInt16Vector LInt16Vector::ForwardVector           { LInt16Vector( 1,  0,  0) };
template<> const LInt16Vector LInt16Vector::BackwardVector          { LInt16Vector(-1,  0,  0) };
template<> const LInt16Vector LInt16Vector::RightVector             { LInt16Vector( 0,  1,  0) };
template<> const LInt16Vector LInt16Vector::LeftVector              { LInt16Vector( 0, -1,  0) };
template<> const LInt16Vector LInt16Vector::UnitVectorX             { LInt16Vector( 1,  0,  0) };
template<> const LInt16Vector LInt16Vector::UnitVectorY             { LInt16Vector( 0,  1,  0) };
template<> const LInt16Vector LInt16Vector::UnitVectorZ             { LInt16Vector( 0,  0,  1) };

template<> const LInt32Vector LInt32Vector::ZeroVector              { LInt32Vector( 0,  0,  0) };
template<> const LInt32Vector LInt32Vector::OneVector               { LInt32Vector( 1,  1,  1) };
template<> const LInt32Vector LInt32Vector::UpVector                { LInt32Vector( 0,  0,  1) };
template<> const LInt32Vector LInt32Vector::DownVector              { LInt32Vector( 0,  0, -1) };
template<> const LInt32Vector LInt32Vector::ForwardVector           { LInt32Vector( 1,  0,  0) };
template<> const LInt32Vector LInt32Vector::BackwardVector          { LInt32Vector(-1,  0,  0) };
template<> const LInt32Vector LInt32Vector::RightVector             { LInt32Vector( 0,  1,  0) };
template<> const LInt32Vector LInt32Vector::LeftVector              { LInt32Vector( 0, -1,  0) };
template<> const LInt32Vector LInt32Vector::UnitVectorX             { LInt32Vector( 1,  0,  0) };
template<> const LInt32Vector LInt32Vector::UnitVectorY             { LInt32Vector( 0,  1,  0) };
template<> const LInt32Vector LInt32Vector::UnitVectorZ             { LInt32Vector( 0,  0,  1) };

template<> const LInt64Vector LInt64Vector::ZeroVector              { LInt64Vector( 0,  0,  0) };
template<> const LInt64Vector LInt64Vector::OneVector               { LInt64Vector( 1,  1,  1) };
template<> const LInt64Vector LInt64Vector::UpVector                { LInt64Vector( 0,  0,  1) };
template<> const LInt64Vector LInt64Vector::DownVector              { LInt64Vector( 0,  0, -1) };
template<> const LInt64Vector LInt64Vector::ForwardVector           { LInt64Vector( 1,  0,  0) };
template<> const LInt64Vector LInt64Vector::BackwardVector          { LInt64Vector(-1,  0,  0) };
template<> const LInt64Vector LInt64Vector::RightVector             { LInt64Vector( 0,  1,  0) };
template<> const LInt64Vector LInt64Vector::LeftVector              { LInt64Vector( 0, -1,  0) };
template<> const LInt64Vector LInt64Vector::UnitVectorX             { LInt64Vector( 1,  0,  0) };
template<> const LInt64Vector LInt64Vector::UnitVectorY             { LInt64Vector( 0,  1,  0) };
template<> const LInt64Vector LInt64Vector::UnitVectorZ             { LInt64Vector( 0,  0,  1) };

template<> const LInt8Vector2 LInt8Vector2::ZeroVector              { LInt8Vector2(0, 0) };
template<> const LInt8Vector2 LInt8Vector2::OneVector               { LInt8Vector2(1, 1) };

template<> const LInt16Vector2 LInt16Vector2::ZeroVector            { LInt16Vector2(0, 0) };
template<> const LInt16Vector2 LInt16Vector2::OneVector             { LInt16Vector2(1, 1) };

template<> const LInt32Vector2 LInt32Vector2::ZeroVector            { LInt32Vector2(0, 0) };
template<> const LInt32Vector2 LInt32Vector2::OneVector             { LInt32Vector2(1, 1) };

template<> const LInt64Vector2 LInt64Vector2::ZeroVector            { LInt64Vector2(0, 0) };
template<> const LInt64Vector2 LInt64Vector2::OneVector             { LInt64Vector2(1, 1) };

template<> const LRotatorF LRotatorF::ZeroRotator                   { LRotatorF( 0.0f, 0.0f, 0.0f) };
template<> const LRotatorF LRotatorF::OneRotator                    { LRotatorF( 1.0f, 1.0f, 1.0f) };
template<> const LRotatorD LRotatorD::ZeroRotator                   { LRotatorD( 0.0, 0.0, 0.0) };
template<> const LRotatorD LRotatorD::OneRotator                    { LRotatorD( 1.0, 1.0, 1.0) };

template<> const LPlaneF LPlaneF::ZeroPlane                         { LPlaneF( 0.0f, 0.0f, 0.0f, 0.0f) };
template<> const LPlaneF LPlaneF::OnePlane                          { LPlaneF( 1.0f, 1.0f, 1.0f, 1.0f) };
template<> const LPlaneF LPlaneF::UnitPlaneX                        { LPlaneF( 1.0f, 0.0f, 0.0f, 0.0f) };
template<> const LPlaneF LPlaneF::UnitPlaneY                        { LPlaneF( 0.0f, 1.0f, 0.0f, 0.0f) };
template<> const LPlaneF LPlaneF::UnitPlaneZ                        { LPlaneF( 0.0f, 0.0f, 1.0f, 0.0f) };
template<> const LPlaneF LPlaneF::UnitPlaneW                        { LPlaneF( 0.0f, 0.0f, 0.0f, 1.0f) };
template<> const LPlaneD LPlaneD::ZeroPlane                         { LPlaneD( 0.0 , 0.0 , 0.0 , 0.0 ) };
template<> const LPlaneD LPlaneD::OnePlane                          { LPlaneD( 1.0 , 1.0 , 1.0 , 1.0 ) };
template<> const LPlaneD LPlaneD::UnitPlaneX                        { LPlaneD( 1.0 , 0.0 , 0.0 , 0.0 ) };
template<> const LPlaneD LPlaneD::UnitPlaneY                        { LPlaneD( 0.0 , 1.0 , 0.0 , 0.0 ) };
template<> const LPlaneD LPlaneD::UnitPlaneZ                        { LPlaneD( 0.0 , 0.0 , 1.0 , 0.0 ) };
template<> const LPlaneD LPlaneD::UnitPlaneW                        { LPlaneD( 0.0 , 0.0 , 0.0 , 1.0 ) };

template<> const LMatrixF LMatrixF::Identity { LPlaneF::UnitPlaneX, LPlaneF::UnitPlaneY, LPlaneF::UnitPlaneZ, LPlaneF::UnitPlaneW };
template<> const LMatrixD LMatrixD::Identity { LPlaneD::UnitPlaneX, LPlaneD::UnitPlaneY, LPlaneD::UnitPlaneZ, LPlaneD::UnitPlaneW };

} /* ~Namespace Jafg */
