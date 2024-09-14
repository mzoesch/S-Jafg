// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"

/*----------------------------------------------------------------------------
    Globals.
----------------------------------------------------------------------------*/

template<> const LVectorF LVectorF::ZeroVector { LVectorF(0.0f, 0.0f, 0.0f) };
template<> const LVectorF LVectorF::OneVector { LVectorF(1.0f, 1.0f, 1.0f) };
template<> const LVectorF LVectorF::UpVector { LVectorF(0.0f, 0.0f, 1.0f) };
template<> const LVectorF LVectorF::DownVector { LVectorF(0.0f, 0.0f, -1.0f) };
template<> const LVectorF LVectorF::ForwardVector { LVectorF(1.0f, 0.0f, 0.0f) };
template<> const LVectorF LVectorF::BackwardVector { LVectorF(-1.0f, 0.0f, 0.0f) };
template<> const LVectorF LVectorF::RightVector { LVectorF(0.0f, 1.0f, 0.0f) };
template<> const LVectorF LVectorF::LeftVector { LVectorF(0.0f, -1.0f, 0.0f) };
template<> const LVectorF LVectorF::UnitVectorX { LVectorF(1.0f, 0.0f, 0.0f) };
template<> const LVectorF LVectorF::UnitVectorY { LVectorF(0.0f, 1.0f, 0.0f) };
template<> const LVectorF LVectorF::UnitVectorZ { LVectorF(0.0f, 0.0f, 1.0f) };

template<> const LVectorD LVectorD::ZeroVector { LVectorD(0.0, 0.0, 0.0) };
template<> const LVectorD LVectorD::OneVector { LVectorD(1.0, 1.0, 1.0) };
template<> const LVectorD LVectorD::UpVector { LVectorD(0.0, 0.0, 1.0) };
template<> const LVectorD LVectorD::DownVector { LVectorD(0.0, 0.0, -1.0) };
template<> const LVectorD LVectorD::ForwardVector { LVectorD(1.0, 0.0, 0.0) };
template<> const LVectorD LVectorD::BackwardVector { LVectorD(-1.0, 0.0, 0.0) };
template<> const LVectorD LVectorD::RightVector { LVectorD(0.0, 1.0, 0.0) };
template<> const LVectorD LVectorD::LeftVector { LVectorD(0.0, -1.0, 0.0) };
template<> const LVectorD LVectorD::UnitVectorX { LVectorD(1.0, 0.0, 0.0) };
template<> const LVectorD LVectorD::UnitVectorY { LVectorD(0.0, 1.0, 0.0) };
template<> const LVectorD LVectorD::UnitVectorZ { LVectorD(0.0, 0.0, 1.0) };
