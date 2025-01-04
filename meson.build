project(
    'drm_test', 'cpp',
    version: '0.1',
    default_options: ['cpp_std=c++20', 'warning_level=1'],
)

cxx = meson.get_compiler('cpp')

drm = dependency(
    'libdrm',
    version: '>=2.4.120'
)

sources = files(
    'drm_test.cpp',
    'main.cpp'
)

local_incs = include_directories('zzwlib')

executable(
    'drm_test',
    sources,
    dependencies: [drm],
    include_directories: [local_incs],
)
