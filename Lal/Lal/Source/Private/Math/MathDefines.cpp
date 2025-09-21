// Copyright mzoesch. All rights reserved.

#ifndef PRIVATE_JAFG_MATH_DEFINES_CPP
#define PRIVATE_JAFG_MATH_DEFINES_CPP

#if (PLATFORM_USES_INLINE_MATH_DEFINES && defined(PRIVATE_JAFG_MAKE_INLINE)) || !PLATFORM_USES_INLINE_MATH_DEFINES

#ifndef PRIVATE_JAFG_MAKE_INLINE
    #include "Lal.afx"
#endif /* !PRIVATE_JAFG_MAKE_INLINE */

#ifdef PRIVATE_JAFG_MAKE_INLINE
    #define TRANSLATION_API         inline
    #define SPECIALIZATION_API      inline
#else /* PRIVATE_JAFG_MAKE_INLINE */
    #define TRANSLATION_API         LAL_API
    #define SPECIALIZATION_API
#endif /* !PRIVATE_JAFG_MAKE_INLINE */

/*----------------------------------------------------------------------------
    Globals.
----------------------------------------------------------------------------*/

TRANSLATION_API LString LexToString(const EVectorAxis::Type InAxis)
{
    switch (InAxis)
    {
    case EVectorAxis::X: { return "X"; }
    case EVectorAxis::Y: { return "Y"; }
    case EVectorAxis::Z: { return "Z"; }
    case EVectorAxis::W: { return "W"; }
    case EVectorAxis::V: { return "V"; }
    case EVectorAxis::U: { return "U"; }
    case EVectorAxis::T: { return "T"; }
    case EVectorAxis::S: { return "S"; }
    }
    panic( "Combination of axes not yet supported." );
    return { };
}

template<> SPECIALIZATION_API const LVector2F LVector2F::ZeroVector                    { LVector2F( 0.0f,  0.0f) };
template<> SPECIALIZATION_API const LVector2F LVector2F::OneVector                     { LVector2F( 1.0f,  1.0f) };
template<> SPECIALIZATION_API const LVector2F LVector2F::UnitVector
{ LVector2F(
0.70710678118654752440084436210484903928483593768847403658833986899536623923105351942519376716382078636750692311545614851246241802f,
0.70710678118654752440084436210484903928483593768847403658833986899536623923105351942519376716382078636750692311545614851246241802f
) };
template<> SPECIALIZATION_API const LVector2F LVector2F::UnitVectorX                   { LVector2F( 1.0f,  0.0f) };
template<> SPECIALIZATION_API const LVector2F LVector2F::UnitVectorY                   { LVector2F( 0.0f,  1.0f) };

template<> SPECIALIZATION_API const LVector2D LVector2D::ZeroVector                    { LVector2D( 0.0,  0.0) };
template<> SPECIALIZATION_API const LVector2D LVector2D::OneVector                     { LVector2D( 1.0,  1.0) };
template<> SPECIALIZATION_API const LVector2D LVector2D::UnitVector
{ LVector2D(
0.70710678118654752440084436210484903928483593768847403658833986899536623923105351942519376716382078636750692311545614851246241802,
0.70710678118654752440084436210484903928483593768847403658833986899536623923105351942519376716382078636750692311545614851246241802
) };
template<> SPECIALIZATION_API const LVector2D LVector2D::UnitVectorX                   { LVector2D( 1.0,  0.0) };
template<> SPECIALIZATION_API const LVector2D LVector2D::UnitVectorY                   { LVector2D( 0.0,  1.0) };

template<> SPECIALIZATION_API const LVector3F LVector3F::ZeroVector                    { LVector3F( 0.0f,  0.0f,  0.0f) };
template<> SPECIALIZATION_API const LVector3F LVector3F::OneVector                     { LVector3F( 1.0f,  1.0f,  1.0f) };
template<> SPECIALIZATION_API const LVector3F LVector3F::UpVector                      { LVector3F( 0.0f,  0.0f,  1.0f) };
template<> SPECIALIZATION_API const LVector3F LVector3F::DownVector                    { LVector3F( 0.0f,  0.0f, -1.0f) };
template<> SPECIALIZATION_API const LVector3F LVector3F::ForwardVector                 { LVector3F( 1.0f,  0.0f,  0.0f) };
template<> SPECIALIZATION_API const LVector3F LVector3F::BackwardVector                { LVector3F(-1.0f,  0.0f,  0.0f) };
template<> SPECIALIZATION_API const LVector3F LVector3F::RightVector                   { LVector3F( 0.0f,  1.0f,  0.0f) };
template<> SPECIALIZATION_API const LVector3F LVector3F::LeftVector                    { LVector3F( 0.0f, -1.0f,  0.0f) };
template<> SPECIALIZATION_API const LVector3F LVector3F::UnitVectorX                   { LVector3F( 1.0f,  0.0f,  0.0f) };
template<> SPECIALIZATION_API const LVector3F LVector3F::UnitVectorY                   { LVector3F( 0.0f,  1.0f,  0.0f) };
template<> SPECIALIZATION_API const LVector3F LVector3F::UnitVectorZ                   { LVector3F( 0.0f,  0.0f,  1.0f) };

template<> SPECIALIZATION_API const LVector3D LVector3D::ZeroVector                    { LVector3D( 0.0,  0.0,  0.0) };
template<> SPECIALIZATION_API const LVector3D LVector3D::OneVector                     { LVector3D( 1.0,  1.0,  1.0) };
template<> SPECIALIZATION_API const LVector3D LVector3D::UpVector                      { LVector3D( 0.0,  0.0,  1.0) };
template<> SPECIALIZATION_API const LVector3D LVector3D::DownVector                    { LVector3D( 0.0,  0.0, -1.0) };
template<> SPECIALIZATION_API const LVector3D LVector3D::ForwardVector                 { LVector3D( 1.0,  0.0,  0.0) };
template<> SPECIALIZATION_API const LVector3D LVector3D::BackwardVector                { LVector3D(-1.0,  0.0,  0.0) };
template<> SPECIALIZATION_API const LVector3D LVector3D::RightVector                   { LVector3D( 0.0,  1.0,  0.0) };
template<> SPECIALIZATION_API const LVector3D LVector3D::LeftVector                    { LVector3D( 0.0, -1.0,  0.0) };
template<> SPECIALIZATION_API const LVector3D LVector3D::UnitVectorX                   { LVector3D( 1.0,  0.0,  0.0) };
template<> SPECIALIZATION_API const LVector3D LVector3D::UnitVectorY                   { LVector3D( 0.0,  1.0,  0.0) };
template<> SPECIALIZATION_API const LVector3D LVector3D::UnitVectorZ                   { LVector3D( 0.0,  0.0,  1.0) };

template<> SPECIALIZATION_API const LVector4F LVector4F::ZeroVector                    { LVector4F( 0.0f, 0.0f, 0.0f, 0.0f) };
template<> SPECIALIZATION_API const LVector4F LVector4F::IdentityVector                { LVector4F( 0.0f, 0.0f, 0.0f, 1.0f) };
template<> SPECIALIZATION_API const LVector4F LVector4F::OneVector                     { LVector4F( 1.0f, 1.0f, 1.0f, 1.0f) };
template<> SPECIALIZATION_API const LVector4F LVector4F::UnitVectorX                   { LVector4F( 1.0f, 0.0f, 0.0f, 0.0f) };
template<> SPECIALIZATION_API const LVector4F LVector4F::UnitVectorY                   { LVector4F( 0.0f, 1.0f, 0.0f, 0.0f) };
template<> SPECIALIZATION_API const LVector4F LVector4F::UnitVectorZ                   { LVector4F( 0.0f, 0.0f, 1.0f, 0.0f) };
template<> SPECIALIZATION_API const LVector4F LVector4F::UnitVectorW                   { LVector4F( 0.0f, 0.0f, 0.0f, 1.0f) };

template<> SPECIALIZATION_API const LVector4D LVector4D::ZeroVector                    { LVector4D( 0.0, 0.0, 0.0, 0.0) };
template<> SPECIALIZATION_API const LVector4D LVector4D::IdentityVector                { LVector4D( 0.0, 0.0, 0.0, 1.0) };
template<> SPECIALIZATION_API const LVector4D LVector4D::OneVector                     { LVector4D( 1.0, 1.0, 1.0, 1.0) };
template<> SPECIALIZATION_API const LVector4D LVector4D::UnitVectorX                   { LVector4D( 1.0, 0.0, 0.0, 0.0) };
template<> SPECIALIZATION_API const LVector4D LVector4D::UnitVectorY                   { LVector4D( 0.0, 1.0, 0.0, 0.0) };
template<> SPECIALIZATION_API const LVector4D LVector4D::UnitVectorZ                   { LVector4D( 0.0, 0.0, 1.0, 0.0) };
template<> SPECIALIZATION_API const LVector4D LVector4D::UnitVectorW                   { LVector4D( 0.0, 0.0, 0.0, 1.0) };

template<> SPECIALIZATION_API const Li8Vector Li8Vector::ZeroVector                    { Li8Vector( 0,  0,  0) };
template<> SPECIALIZATION_API const Li8Vector Li8Vector::OneVector                     { Li8Vector( 1,  1,  1) };
template<> SPECIALIZATION_API const Li8Vector Li8Vector::UpVector                      { Li8Vector( 0,  0,  1) };
template<> SPECIALIZATION_API const Li8Vector Li8Vector::DownVector                    { Li8Vector( 0,  0, -1) };
template<> SPECIALIZATION_API const Li8Vector Li8Vector::ForwardVector                 { Li8Vector( 1,  0,  0) };
template<> SPECIALIZATION_API const Li8Vector Li8Vector::BackwardVector                { Li8Vector(-1,  0,  0) };
template<> SPECIALIZATION_API const Li8Vector Li8Vector::RightVector                   { Li8Vector( 0,  1,  0) };
template<> SPECIALIZATION_API const Li8Vector Li8Vector::LeftVector                    { Li8Vector( 0, -1,  0) };
template<> SPECIALIZATION_API const Li8Vector Li8Vector::UnitVectorX                   { Li8Vector( 1,  0,  0) };
template<> SPECIALIZATION_API const Li8Vector Li8Vector::UnitVectorY                   { Li8Vector( 0,  1,  0) };
template<> SPECIALIZATION_API const Li8Vector Li8Vector::UnitVectorZ                   { Li8Vector( 0,  0,  1) };

template<> SPECIALIZATION_API const Li16Vector Li16Vector::ZeroVector                  { Li16Vector( 0,  0,  0) };
template<> SPECIALIZATION_API const Li16Vector Li16Vector::OneVector                   { Li16Vector( 1,  1,  1) };
template<> SPECIALIZATION_API const Li16Vector Li16Vector::UpVector                    { Li16Vector( 0,  0,  1) };
template<> SPECIALIZATION_API const Li16Vector Li16Vector::DownVector                  { Li16Vector( 0,  0, -1) };
template<> SPECIALIZATION_API const Li16Vector Li16Vector::ForwardVector               { Li16Vector( 1,  0,  0) };
template<> SPECIALIZATION_API const Li16Vector Li16Vector::BackwardVector              { Li16Vector(-1,  0,  0) };
template<> SPECIALIZATION_API const Li16Vector Li16Vector::RightVector                 { Li16Vector( 0,  1,  0) };
template<> SPECIALIZATION_API const Li16Vector Li16Vector::LeftVector                  { Li16Vector( 0, -1,  0) };
template<> SPECIALIZATION_API const Li16Vector Li16Vector::UnitVectorX                 { Li16Vector( 1,  0,  0) };
template<> SPECIALIZATION_API const Li16Vector Li16Vector::UnitVectorY                 { Li16Vector( 0,  1,  0) };
template<> SPECIALIZATION_API const Li16Vector Li16Vector::UnitVectorZ                 { Li16Vector( 0,  0,  1) };

template<> SPECIALIZATION_API const Li32Vector Li32Vector::ZeroVector                  { Li32Vector( 0,  0,  0) };
template<> SPECIALIZATION_API const Li32Vector Li32Vector::OneVector                   { Li32Vector( 1,  1,  1) };
template<> SPECIALIZATION_API const Li32Vector Li32Vector::UpVector                    { Li32Vector( 0,  0,  1) };
template<> SPECIALIZATION_API const Li32Vector Li32Vector::DownVector                  { Li32Vector( 0,  0, -1) };
template<> SPECIALIZATION_API const Li32Vector Li32Vector::ForwardVector               { Li32Vector( 1,  0,  0) };
template<> SPECIALIZATION_API const Li32Vector Li32Vector::BackwardVector              { Li32Vector(-1,  0,  0) };
template<> SPECIALIZATION_API const Li32Vector Li32Vector::RightVector                 { Li32Vector( 0,  1,  0) };
template<> SPECIALIZATION_API const Li32Vector Li32Vector::LeftVector                  { Li32Vector( 0, -1,  0) };
template<> SPECIALIZATION_API const Li32Vector Li32Vector::UnitVectorX                 { Li32Vector( 1,  0,  0) };
template<> SPECIALIZATION_API const Li32Vector Li32Vector::UnitVectorY                 { Li32Vector( 0,  1,  0) };
template<> SPECIALIZATION_API const Li32Vector Li32Vector::UnitVectorZ                 { Li32Vector( 0,  0,  1) };

template<> SPECIALIZATION_API const Li64Vector Li64Vector::ZeroVector                  { Li64Vector( 0,  0,  0) };
template<> SPECIALIZATION_API const Li64Vector Li64Vector::OneVector                   { Li64Vector( 1,  1,  1) };
template<> SPECIALIZATION_API const Li64Vector Li64Vector::UpVector                    { Li64Vector( 0,  0,  1) };
template<> SPECIALIZATION_API const Li64Vector Li64Vector::DownVector                  { Li64Vector( 0,  0, -1) };
template<> SPECIALIZATION_API const Li64Vector Li64Vector::ForwardVector               { Li64Vector( 1,  0,  0) };
template<> SPECIALIZATION_API const Li64Vector Li64Vector::BackwardVector              { Li64Vector(-1,  0,  0) };
template<> SPECIALIZATION_API const Li64Vector Li64Vector::RightVector                 { Li64Vector( 0,  1,  0) };
template<> SPECIALIZATION_API const Li64Vector Li64Vector::LeftVector                  { Li64Vector( 0, -1,  0) };
template<> SPECIALIZATION_API const Li64Vector Li64Vector::UnitVectorX                 { Li64Vector( 1,  0,  0) };
template<> SPECIALIZATION_API const Li64Vector Li64Vector::UnitVectorY                 { Li64Vector( 0,  1,  0) };
template<> SPECIALIZATION_API const Li64Vector Li64Vector::UnitVectorZ                 { Li64Vector( 0,  0,  1) };

template<> SPECIALIZATION_API const Li8Vector2 Li8Vector2::ZeroVector                  { Li8Vector2(0, 0) };
template<> SPECIALIZATION_API const Li8Vector2 Li8Vector2::OneVector                   { Li8Vector2(1, 1) };
template<> SPECIALIZATION_API const Lu8Vector2 Lu8Vector2::ZeroVector                  { Lu8Vector2(0u, 0u) };
template<> SPECIALIZATION_API const Lu8Vector2 Lu8Vector2::OneVector                   { Lu8Vector2(1u, 1u) };

template<> SPECIALIZATION_API const Li16Vector2 Li16Vector2::ZeroVector                { Li16Vector2(0, 0) };
template<> SPECIALIZATION_API const Li16Vector2 Li16Vector2::OneVector                 { Li16Vector2(1, 1) };
template<> SPECIALIZATION_API const Lu16Vector2 Lu16Vector2::ZeroVector                { Lu16Vector2(0u, 0u) };
template<> SPECIALIZATION_API const Lu16Vector2 Lu16Vector2::OneVector                 { Lu16Vector2(1u, 1u) };

template<> SPECIALIZATION_API const Li32Vector2 Li32Vector2::ZeroVector                { Li32Vector2(0, 0) };
template<> SPECIALIZATION_API const Li32Vector2 Li32Vector2::OneVector                 { Li32Vector2(1, 1) };
template<> SPECIALIZATION_API const Lu32Vector2 Lu32Vector2::ZeroVector                { Lu32Vector2(0u, 0u) };
template<> SPECIALIZATION_API const Lu32Vector2 Lu32Vector2::OneVector                 { Lu32Vector2(1u, 1u) };

template<> SPECIALIZATION_API const Li64Vector2 Li64Vector2::ZeroVector                { Li64Vector2(0, 0) };
template<> SPECIALIZATION_API const Li64Vector2 Li64Vector2::OneVector                 { Li64Vector2(1, 1) };
template<> SPECIALIZATION_API const Lu64Vector2 Lu64Vector2::ZeroVector                { Lu64Vector2(0u, 0u) };
template<> SPECIALIZATION_API const Lu64Vector2 Lu64Vector2::OneVector                 { Lu64Vector2(1u, 1u) };

template<> SPECIALIZATION_API const LRotatorF LRotatorF::ZeroRotator                   { LRotatorF( 0.0f, 0.0f, 0.0f) };
template<> SPECIALIZATION_API const LRotatorF LRotatorF::OneRotator                    { LRotatorF( 1.0f, 1.0f, 1.0f) };
template<> SPECIALIZATION_API const LRotatorD LRotatorD::ZeroRotator                   { LRotatorD( 0.0, 0.0, 0.0) };
template<> SPECIALIZATION_API const LRotatorD LRotatorD::OneRotator                    { LRotatorD( 1.0, 1.0, 1.0) };

template<> SPECIALIZATION_API const LPlaneF LPlaneF::ZeroPlane                         { LPlaneF( 0.0f, 0.0f, 0.0f, 0.0f) };
template<> SPECIALIZATION_API const LPlaneF LPlaneF::OnePlane                          { LPlaneF( 1.0f, 1.0f, 1.0f, 1.0f) };
template<> SPECIALIZATION_API const LPlaneF LPlaneF::UnitPlaneX                        { LPlaneF( 1.0f, 0.0f, 0.0f, 0.0f) };
template<> SPECIALIZATION_API const LPlaneF LPlaneF::UnitPlaneY                        { LPlaneF( 0.0f, 1.0f, 0.0f, 0.0f) };
template<> SPECIALIZATION_API const LPlaneF LPlaneF::UnitPlaneZ                        { LPlaneF( 0.0f, 0.0f, 1.0f, 0.0f) };
template<> SPECIALIZATION_API const LPlaneF LPlaneF::UnitPlaneW                        { LPlaneF( 0.0f, 0.0f, 0.0f, 1.0f) };
template<> SPECIALIZATION_API const LPlaneD LPlaneD::ZeroPlane                         { LPlaneD( 0.0 , 0.0 , 0.0 , 0.0 ) };
template<> SPECIALIZATION_API const LPlaneD LPlaneD::OnePlane                          { LPlaneD( 1.0 , 1.0 , 1.0 , 1.0 ) };
template<> SPECIALIZATION_API const LPlaneD LPlaneD::UnitPlaneX                        { LPlaneD( 1.0 , 0.0 , 0.0 , 0.0 ) };
template<> SPECIALIZATION_API const LPlaneD LPlaneD::UnitPlaneY                        { LPlaneD( 0.0 , 1.0 , 0.0 , 0.0 ) };
template<> SPECIALIZATION_API const LPlaneD LPlaneD::UnitPlaneZ                        { LPlaneD( 0.0 , 0.0 , 1.0 , 0.0 ) };
template<> SPECIALIZATION_API const LPlaneD LPlaneD::UnitPlaneW                        { LPlaneD( 0.0 , 0.0 , 0.0 , 1.0 ) };

template<> SPECIALIZATION_API const LMatrix3F LMatrix3F::Identity                      { LVector3F::UnitVectorX, LVector3F::UnitVectorY, LVector3F::UnitVectorZ };
template<> SPECIALIZATION_API const LMatrix3D LMatrix3D::Identity                      { LVector3D::UnitVectorX, LVector3D::UnitVectorY, LVector3D::UnitVectorZ };
template<> SPECIALIZATION_API const LMatrix3F LMatrix3F::Zero                          { LVector3F::ZeroVector, LVector3F::ZeroVector, LVector3F::ZeroVector };
template<> SPECIALIZATION_API const LMatrix3D LMatrix3D::Zero                          { LVector3D::ZeroVector, LVector3D::ZeroVector, LVector3D::ZeroVector };
template<> SPECIALIZATION_API const LMatrix4F LMatrix4F::Identity                      { LPlaneF::UnitPlaneX, LPlaneF::UnitPlaneY, LPlaneF::UnitPlaneZ, LPlaneF::UnitPlaneW };
template<> SPECIALIZATION_API const LMatrix4D LMatrix4D::Identity                      { LPlaneD::UnitPlaneX, LPlaneD::UnitPlaneY, LPlaneD::UnitPlaneZ, LPlaneD::UnitPlaneW };
template<> SPECIALIZATION_API const LMatrix4F LMatrix4F::Zero                          { LPlaneF::ZeroPlane, LPlaneF::ZeroPlane, LPlaneF::ZeroPlane, LPlaneF::ZeroPlane };
template<> SPECIALIZATION_API const LMatrix4D LMatrix4D::Zero                          { LPlaneD::ZeroPlane, LPlaneD::ZeroPlane, LPlaneD::ZeroPlane, LPlaneD::ZeroPlane };

#undef TRANSLATION_API
#undef SPECIALIZATION_API
#undef PRIVATE_JAFG_MAKE_INLINE
#endif /* PLATFORM_USES_INLINE_MATH_DEFINES && defined(PRIVATE_JAFG_MAKE_INLINE) */
#endif /* !PRIVATE_JAFG_MATH_DEFINES_CPP */
