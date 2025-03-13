# Copyright mzoesch. All rights reserved.

from Programs.Meta import *


def add_module(me: Module, args: ModuleArgs) -> None:
    me.pch = PchUsage.ALLOW
    me.pch_content = '#include "CoreAfx.h"'
    me.kind = ModuleKind.SHARED

    me.public_dependencies.append('Lal')
    me.private_dependencies.append('EngineForward')

    me.native_includes.append('Engine/Vendor/Include')

    if args.platform.get_name() != 'Wasm':
        me.native_includes.append('Engine/Vendor/Include/Freetype')
        
        me.native_dependencies.extend([
            'Engine/Vendor/Lib/glfw3.lib',
            'Engine/Vendor/Lib/freetype.lib',
        ])

    if args.platform.get_name() == 'Windows':
        if args.build_configuration.get_name() == 'Debug' or args.build_configuration.get_name() == 'Development':
            me.native_runtime_dependencies.extend([
                'Engine/Vendor/Lib/FastNoiseD.lib',
                'Engine/Vendor/Lib/FastNoiseD.dll',
            ])
            me.native_runtime_dependencies.extend([
                'Engine/Vendor/Lib/FastNoiseD.pdb',
                'Engine/Vendor/Lib/freetype.pdb',
            ])

        if args.build_configuration.get_name() == 'Shipping':
            me.native_runtime_dependencies.extend([
                'Engine/Vendor/Lib/FastNoise.lib',
                'Engine/Vendor/Lib/FastNoise.dll',
            ])

    return None
