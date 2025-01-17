# Copyright mzoesch. All rights reserved.

from Programs.Meta import *


def apply_policies(in_const_target: Target, me: Module) -> None:
    me.pch = PchUsage.ALLOW
    me.pch_content = '#include "CoreAfx.h"'
    me.kind = ModuleKind.SHARED

    me.public_dependencies.append('Lal')
    me.private_dependencies.append('EngineForward')

    me.private_native_include_dirs.extend([
        'Engine/Vendor/Include',
        'Engine/Vendor/Include/Freetype',
    ])

    me.private_native_dependencies.extend([
        'Engine/Vendor/Lib/glfw3.lib',
        'Engine/Vendor/Lib/freetype.lib',
    ])

    if (in_const_target.volatile_build_configuration == BuildConfiguration.DEBUG or
        in_const_target.volatile_build_configuration == BuildConfiguration.DEVELOPMENT):
        me.private_native_dependencies.extend([
            'Engine/Vendor/Lib/FastNoiseD.lib',
            'Engine/Vendor/Lib/FastNoiseD.dll',
        ])
        me.private_additional_copied_files.extend([
            'Engine/Vendor/Lib/FastNoiseD.pdb',
            'Engine/Vendor/Lib/freetype.pdb',
        ])
    if in_const_target.volatile_build_configuration == BuildConfiguration.SHIPPING:
        me.private_native_dependencies.extend([
            'Engine/Vendor/Lib/FastNoise.lib',
            'Engine/Vendor/Lib/FastNoise.dll',
        ])

    if 'WITH_TESTS' in in_const_target.defines:
        me.public_dependencies.append('TesterForward')

    return None
