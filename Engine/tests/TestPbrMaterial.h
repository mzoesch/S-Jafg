// Copyright mzoesch. All rights reserved.

#pragma once

#include "Rhi/PhysicalRendering.h"

TRIVIAL_TEST_CASE("pbr", PbrParameterInfos)
{
    using namespace Jafg;

    /* Every default is valid and typical, every norm lies inside of the range, and every key is unique. */
    TArray<LStringView> Keys;
    LPbrMaterialParameters Parameters;
    LPbrMaterialParameters::ForEachParameter(Parameters, [&]<typename T>(TPbrParameter<T> const& Info, T& Field)
    {
        TEST_FALSE(algo::contains(Keys, Info.Key))
        Keys.push_back(Info.Key);
        TEST_TRUE(Field == Info.Default)
        TEST_TRUE(Info.Clamp(Info.Default) == Info.Default)
        TEST_FALSE(Info.Description.empty())
        if constexpr (!std::is_same_v<T, bool>)
        {
            TEST_TRUE(Info.Min <= Info.NormMin)
            TEST_TRUE(Info.NormMin < Info.NormMax)
            TEST_TRUE(Info.NormMax <= Info.Max)
        }
    });
    TEST_EQUAL(Keys.size(), 45uz)
}

TRIVIAL_TEST_CASE("pbr", PbrParameterClamp)
{
    using namespace Jafg;

    typedef LPbrMaterialParameters P;
    TEST_EQUAL(P::BaseWeightInfo.Clamp(1.5f), 1.0f)
    TEST_EQUAL(P::BaseWeightInfo.Clamp(-0.5f), 0.0f)
    /* Unbounded above. */
    TEST_EQUAL(P::SpecularWeightInfo.Clamp(4.0f), 4.0f)
    /* The lower bound of an index of refraction is open. */
    TEST_TRUE(P::SpecularIorInfo.Clamp(0.0f) > 0.0f)
    TEST_EQUAL(P::SpecularIorInfo.Clamp(2.42f), 2.42f)
    /* Per channel. */
    TEST_TRUE(P::BaseColorInfo.Clamp(LVec3F{-1.0f, 0.5f, 2.0f}) == (LVec3F{0.0f, 0.5f, 1.0f}))

    TEST_TRUE(P::SpecularIorInfo.DescribeValues() == LString{"Range: (0,inf). Norm: [1,3]. Default: 1.5."})
    TEST_TRUE(P::BaseColorInfo.DescribeValues() == LString{"Range: [0,1]. Norm: [0,1]. Default: (0.8, 0.8, 0.8)."})
    TEST_TRUE(P::TransmissionDepthInfo.DescribeValues() == LString{"Range: [0,inf). Norm: [0,1]. Default: 0. Unit: cm."})
    TEST_TRUE(P::GeometryThinWalledInfo.DescribeValues() == LString{"Default: false."})
}

TRIVIAL_TEST_CASE("pbr", PbrParameterDevice)
{
    using namespace Jafg;

    /* Everything that goes to the device comes back the same. */
    LPbrMaterialParameters Parameters;
    Parameters.BaseColor = {0.1f, 0.2f, 0.3f};
    Parameters.SpecularIor = 2.42f;
    Parameters.TransmissionWeight = 1.0f;
    Parameters.TransmissionScatter = {0.4f, 0.5f, 0.6f};
    Parameters.CoatWeight = 0.5f;
    Parameters.FuzzRoughness = 0.9f;
    Parameters.EmissionLuminance = 1000.0f;
    Parameters.GeometryThinWalled = true;
    Parameters.GeometryCoatTangentRotation = 45.0f;
    Parameters.UVOffset = {0.5f, -0.5f};
    Parameters.Flags = EPbrMaterialBits::AlphaMask | EPbrMaterialBits::DirectXNormals;

    SSBO::BindlessPbrMaterial Data{};
    Parameters.ToDevice(Data);
    TEST_EQUAL(Data.GeometryThinWalled, 1u)
    TEST_TRUE(LPbrMaterialParameters::FromDevice(Data) == Parameters)
}

TRIVIAL_TEST_CASE("pbr", PbrParameterTranslucent)
{
    using namespace Jafg;

    LPbrMaterialParameters Parameters;
    TEST_FALSE(Parameters.IsTranslucent())
    TEST_FALSE(Parameters.IsDoubleSided())

    Parameters.GeometryOpacity = 0.5f;
    TEST_TRUE(Parameters.IsTranslucent())
    /* Masked: Either fully there or not at all. */
    Parameters.Flags = EPbrMaterialBits::AlphaMask;
    TEST_FALSE(Parameters.IsTranslucent())

    Parameters = {};
    Parameters.Flags = EPbrMaterialBits::AlphaBlend;
    TEST_TRUE(Parameters.IsTranslucent())

    Parameters = {};
    Parameters.TransmissionWeight = 1.0f;
    TEST_TRUE(Parameters.IsTranslucent())
    /* Metals do not let light through. */
    Parameters.BaseMetalness = 1.0f;
    TEST_FALSE(Parameters.IsTranslucent())

    Parameters = {};
    Parameters.GeometryThinWalled = true;
    TEST_TRUE(Parameters.IsDoubleSided())
}
