// Copyright mzoesch. All rights reserved.

#pragma once

TRIVIAL_TEST_CASE("maths", Constants)
{
    TEST_EQUAL(maths::zero_vector<LVec1F>, LVec1F{0.0})
    TEST_EQUAL(maths::zero_vector<LVec1D>, LVec1D{0.0})
    TEST_EQUAL(maths::zero_vector<LVec2F>, LVec2F{0.0})
    TEST_EQUAL(maths::zero_vector<LVec2D>, LVec2D{0.0})
    TEST_EQUAL(maths::zero_vector<LVec3F>, LVec3F{0.0})
    TEST_EQUAL(maths::zero_vector<LVec3D>, LVec3D{0.0})
    TEST_EQUAL(maths::zero_vector<LVec4F>, LVec4F{0.0})
    TEST_EQUAL(maths::zero_vector<LVec4D>, LVec4D{0.0})

    TEST_EQUAL(maths::one_vector<LVec1F>, LVec1F{1.0})
    TEST_EQUAL(maths::one_vector<LVec1D>, LVec1D{1.0})
    TEST_EQUAL(maths::one_vector<LVec2F>, LVec2F{1.0})
    TEST_EQUAL(maths::one_vector<LVec2D>, LVec2D{1.0})
    TEST_EQUAL(maths::one_vector<LVec3F>, LVec3F{1.0})
    TEST_EQUAL(maths::one_vector<LVec3D>, LVec3D{1.0})
    TEST_EQUAL(maths::one_vector<LVec4F>, LVec4F{1.0})
    TEST_EQUAL(maths::one_vector<LVec4D>, LVec4D{1.0})

    TEST_EQUAL(maths::forward_vector<LVec3F>, LVec3F{0, 0, 1})
    TEST_EQUAL(maths::forward_vector<LVec3D>, LVec3D{0, 0, 1})
    TEST_EQUAL(maths::backward_vector<LVec3F>, LVec3F{0, 0, -1})
    TEST_EQUAL(maths::backward_vector<LVec3D>, LVec3D{0, 0, -1})
    TEST_EQUAL(maths::right_vector<LVec3F>, LVec3F{1, 0, 0})
    TEST_EQUAL(maths::right_vector<LVec3D>, LVec3D{1, 0, 0})
    TEST_EQUAL(maths::left_vector<LVec3F>, LVec3F{-1, 0, 0})
    TEST_EQUAL(maths::left_vector<LVec3D>, LVec3D{-1, 0, 0})
    TEST_EQUAL(maths::up_vector<LVec3F>, LVec3F{0, 1, 0})
    TEST_EQUAL(maths::up_vector<LVec3D>, LVec3D{0, 1, 0})
    TEST_EQUAL(maths::down_vector<LVec3F>, LVec3F{0, -1, 0})
    TEST_EQUAL(maths::down_vector<LVec3D>, LVec3D{0, -1, 0})

    TEST_EQUAL(maths::unit_vector_x<LVec1F>, LVec1F{1})
    TEST_EQUAL(maths::unit_vector_x<LVec1D>, LVec1D{1})
    TEST_EQUAL(maths::unit_vector_x<LVec2F>, LVec2F{1, 0})
    TEST_EQUAL(maths::unit_vector_x<LVec2D>, LVec2D{1, 0})
    TEST_EQUAL(maths::unit_vector_x<LVec3F>, LVec3F{1, 0, 0})
    TEST_EQUAL(maths::unit_vector_x<LVec3D>, LVec3D{1, 0, 0})
    TEST_EQUAL(maths::unit_vector_x<LVec4F>, LVec4F{1, 0, 0, 0})
    TEST_EQUAL(maths::unit_vector_x<LVec4D>, LVec4D{1, 0, 0, 0})

    TEST_EQUAL(maths::unit_vector_y<LVec2F>, LVec2F{0, 1})
    TEST_EQUAL(maths::unit_vector_y<LVec2D>, LVec2D{0, 1})
    TEST_EQUAL(maths::unit_vector_y<LVec3F>, LVec3F{0, 1, 0})
    TEST_EQUAL(maths::unit_vector_y<LVec3D>, LVec3D{0, 1, 0})
    TEST_EQUAL(maths::unit_vector_y<LVec4F>, LVec4F{0, 1, 0, 0})
    TEST_EQUAL(maths::unit_vector_y<LVec4D>, LVec4D{0, 1, 0, 0})

    TEST_EQUAL(maths::unit_vector_z<LVec3F>, LVec3F{0, 0, 1})
    TEST_EQUAL(maths::unit_vector_z<LVec3D>, LVec3D{0, 0, 1})
    TEST_EQUAL(maths::unit_vector_z<LVec4F>, LVec4F{0, 0, 1, 0})
    TEST_EQUAL(maths::unit_vector_z<LVec4D>, LVec4D{0, 0, 1, 0})

    TEST_EQUAL(maths::unit_vector_w<LVec4F>, LVec4F{0, 0, 0, 1})
    TEST_EQUAL(maths::unit_vector_w<LVec4D>, LVec4D{0, 0, 0, 1})
}

TRIVIAL_TEST_CASE("maths", Quaternion)
{
    auto RandomEuler{[]<std::floating_point T, maths::qual_t Q>(std::mt19937& Rng) -> TVec3<T,Q>
    {
        std::uniform_real_distribution<T> PitchDistribution{-maths::pi_v<T> / T{2}, maths::pi_v<T> / T{2}};
        std::uniform_real_distribution<T> AngleDistribution(-maths::pi_v<T>, maths::pi_v<T>);
        return { PitchDistribution(Rng), AngleDistribution(Rng), AngleDistribution(Rng) };
    }};

    auto TestEuler{[&]<std::floating_point T, maths::qual_t Q>(TVec3<T,Q> Euler0)
    {
        auto Quaternion0{maths::rotator(Euler0)};
        auto Euler1{maths::editor_euler_angles(Quaternion0)};
        auto Quaternion1{maths::rotator(Euler1)};

        /* We use abs because: Quaternion == -Quaternion */
        auto Error{maths::abs(maths::dot(Quaternion0, Quaternion1))};

        TEST_GREATER(Error, static_cast<T>(1.0 - 1e-2))
    }};

    auto TestPrecision{[&]<std::floating_point T, maths::qual_t Q>()
    {
        std::random_device Device;
        std::mt19937 Rng(Device());
        for (auto Idx{0uz}; Idx < 100'000; ++Idx)
        {
            TestEuler.operator()<T,Q>(RandomEuler.operator()<T,Q>(Rng));
        }
    }};

    TestPrecision.operator()<maths::single_precision, maths::defaultp>();
    TestPrecision.operator()<maths::double_precision, maths::defaultp>();

    constexpr std::array Stress{
        LVec3D{ maths::half_pi_v<f64>, 0.0, 0.0},
        LVec3D{-maths::half_pi_v<f64>, 0.0, 0.0},
        LVec3D{0.0, maths::pi_v<f64>, 0.0},
        LVec3D{0.0, -maths::pi_v<f64>, 0.0},
        LVec3D{0.0, 0.0, maths::pi_v<f64>},
        LVec3D{0.0, 0.0, -maths::pi_v<f64>},
        LVec3D{0.001, 1.5, -2.0},
        LVec3D{-1.57052112, 1.87566066, 2.51877141},
        LVec3D{-1.57059002, 1.64620233, -1.32447338},
        };
    for (auto const& Euler: Stress)
    {
        TestEuler.operator()<maths::single_precision, maths::defaultp>(LVec3F{Euler});
        TestEuler.operator()<maths::double_precision, maths::defaultp>(Euler);
    }
}
