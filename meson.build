project('vulkan-sandbox', 'c',
        version: '1.0.0',
        license: 'GPL-3.0-only',
        default_options: [
            'c_std=c11',
            'warning_level=2',
            'werror=true',
        ],
        meson_version: '>= 0.56.0'
)

cc = meson.get_compiler('c')
cflags = [
    '-Wno-unused-parameter',
    '-Wno-c++-compat',
    '-Wno-missing-field-initializers',
    '-Wmissing-prototypes',
    '-Wstrict-prototypes',
    '-Wundef',
    '-Wlogical-op',
    '-Wpointer-arith',
    '-Wuninitialized',
    '-Winit-self',
    '-Wstrict-prototypes',
    '-Wimplicit-fallthrough',
    '-Wredundant-decls',
    '-Wincompatible-pointer-types',
    '-Wformat=2',
    '-Wmissing-declarations',
    '-fvisibility=hidden',
]
add_project_arguments(cc.get_supported_arguments(cflags), language: 'c')

dep_vulkan = dependency('vulkan')

executable('video-decode',
           'video-decode.c',
           dependencies: [dep_vulkan],
)
