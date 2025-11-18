/home/mzoesch/Downloads/slang-2025.22-linux-x86_64/bin/slangc \
        Slang/Test.slang \
        -target spirv \
        -profile spirv_1_4 \
        -emit-spirv-directly \
        -fvk-use-entrypoint-name -entry vertMain -entry fragMain \
        -o Spir-V/Test.spv
