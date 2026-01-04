// Copyright mzoesch. All rights reserved.

#pragma once

#include "Lal.afx"

namespace Jafg::PlatformMisc
{

ENGINE_API LPath GetEngineRootDir();
ENGINE_API LPath GetSelfProcDir();

//# E.g.: "Windows", "Linux", ...
ENGINE_API LStringView GetTargetPlatform() noexcept;
//# E.g.: "x86_64", "x86", ...
ENGINE_API LStringView GetTargetArchitecture() noexcept;
//# E.g.: "Client", "Daemon", ...
ENGINE_API LStringView GetTargetType() noexcept;
//# E.g.: "Debug", "Shipping", ...
ENGINE_API LStringView GetTargetConfiguration() noexcept;
//# E.g.: "Client-Shipping", ...
ENGINE_API LStringView GetTargetCompound() noexcept;
//# E.g.: "Windows-x86_64", "Linux-x86_64", ...
ENGINE_API LStringView GetTargetPlatformCompound() noexcept;
//# E.g.: "Linux-x86_64/Client-Shipping", ...
ENGINE_API LStringView GetTargetConfigPath() noexcept;

inline LPath GetRootBinaryDirectory() noexcept { return LPath{ "Binaries" } / GetTargetConfigPath(); }

} /* ~Namespace PlatformMisc::Jafg */
