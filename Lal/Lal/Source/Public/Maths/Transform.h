// Copyright mzoesch. All rights reserved.

#pragma once

namespace Jafg
{

/** Jafg implementation of a transform. */
template <typename T>
struct TTransform final
{
    static_assert(std::is_floating_point_v<T>, "Generic type T of TTransform must be a floating point type.");

    TVector<T>  Translation;
    TRotator<T> Rotator;
    TVector<T>  Scale;
};

} /* ~Namespace Jafg */
