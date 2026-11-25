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
    /*
     * The largest 1 - |dot| between a rotation and its round trip through Euler angles. About 0.23 deg in single and
     * 0.023 deg in double precision. Single precision cannot measure much finer: One rounding step of the dot below one
     * is already 0.04 deg. Double precision is bound by the gimbal lock branch, which treats a cosine of the pitch up to
     * 1e-4 as zero.
     */
    auto Tolerance{[]<std::floating_point T>() -> T { return std::is_same_v<T, maths::single_precision> ? T(2e-6) : T(2e-8); }};

    auto TestEuler{[&]<std::floating_point T, maths::qual_t Q>(TVec3<T,Q> Euler0)
    {
        auto Quaternion0{maths::rotator(Euler0)};
        auto Euler1{maths::editor_euler_angles(Quaternion0)};
        auto Quaternion1{maths::rotator(Euler1)};

        /* We use abs because: Quaternion == -Quaternion */
        const T Error{T{1} - maths::abs(maths::dot(Quaternion0, Quaternion1))};
        const T Max{Tolerance.template operator()<T>()};
        /* The angles only for failures. Formatting them for every check would be slow. */
        this->TestLess(Error < Max ? LString{} : algo::sprintf(
            "LT: 1 - |dot| < tolerance, {} precision round trip of ({:.9g}, {:.9g}, {:.9g}) rad, got ({:.9g}, {:.9g}, {:.9g})",
            std::is_same_v<T, maths::single_precision> ? "single" : "double",
            Euler0.x, Euler0.y, Euler0.z, Euler1.x, Euler1.y, Euler1.z
            ), __LINE__, Error, Max);
    }};

    auto TestPrecision{[&]<std::floating_point T, maths::qual_t Q>()
    {
        /* Fixed, so that a failure can be reproduced. */
        std::mt19937 Rng{0x4A616667u};
        std::uniform_real_distribution<T> PitchDistribution{-maths::pi_v<T> / T{2}, maths::pi_v<T> / T{2}};
        std::uniform_real_distribution<T> AngleDistribution(-maths::pi_v<T>, maths::pi_v<T>);

        for (auto Idx{0uz}; Idx < 100'000; ++Idx)
        {
            TestEuler.template operator()<T,Q>(TVec3<T,Q>{PitchDistribution(Rng), AngleDistribution(Rng), AngleDistribution(Rng)});
        }

        /*
         * Close to the gimbal lock, where uniform pitches hardly ever get: The first rounding steps away from +-90 deg
         * (in single precision, 90 deg itself is a bit more than pi / 2), and distances from 1e-7 to 1e-2 rad, around
         * the threshold of the gimbal lock branch.
         */
        for (const T Sign: {T{1}, T{-1}})
        {
            TArray<T> Pitches;
            for (T Pitch{maths::half_pi_v<T>}; Pitches.size() < 64uz; Pitch = std::nextafter(Pitch, T{0}))
            {
                Pitches.push_back(Pitch);
            }
            for (auto Idx{0}; Idx < 40; ++Idx)
            {
                Pitches.push_back(maths::half_pi_v<T> - static_cast<T>(std::pow(10.0, -7.0 + 5.0 * Idx / 39.0)));
            }
            for (const T Pitch: Pitches)
            {
                for (auto Idx{0}; Idx < 50; ++Idx)
                {
                    TestEuler.template operator()<T,Q>(TVec3<T,Q>{Sign * Pitch, AngleDistribution(Rng), AngleDistribution(Rng)});
                }
            }
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
        /* Typed into the editor as degrees, e.g. (90, 180, 20). Were 180 or 20 deg off in single precision. */
        LVec3D{ maths::half_pi_v<f64>, maths::pi_v<f64>, 0.0},
        LVec3D{ maths::half_pi_v<f64>, maths::pi_v<f64>, 0.34906585},
        LVec3D{ maths::half_pi_v<f64>, 0.0, 0.34906585},
        LVec3D{-maths::half_pi_v<f64>, maths::pi_v<f64>, 0.0},
        LVec3D{-maths::half_pi_v<f64>, maths::pi_v<f64>, 0.34906585},
        LVec3D{-maths::half_pi_v<f64>, 0.0, -0.34906585},
        /* The worst one found, at 1.57079637 (single precision 90 deg). */
        LVec3D{1.57079637, 1.69826877, -1.44324243},
        };
    for (auto const& Euler: Stress)
    {
        TestEuler.operator()<maths::single_precision, maths::defaultp>(LVec3F{Euler});
        TestEuler.operator()<maths::double_precision, maths::defaultp>(Euler);
    }
}
