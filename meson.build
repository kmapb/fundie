project('fundie', 'cpp',
    default_options: ['warning_level=3', 'cpp_std=c++20'],
)
libfundlogic=library('libfundlogic', 'stage.cpp', 'round.cpp', 'position.cpp', 'fund.cpp', 'debug.cpp')

gtest_dep = dependency('gtest', main: true, fallback: ['gtest', 'gtest_main_dep'])
yaml_cpp_dep = dependency('yaml-cpp')

main_exe = executable('sim', 'main.cpp', link_with: libfundlogic, dependencies: [yaml_cpp_dep])

test_position_exe = executable('test_position', 'test/test_position.cpp',
    link_with: libfundlogic,
    dependencies: gtest_dep)
test('position unit test', test_position_exe)

test_stage_exe = executable('test_stage', 'test/test_stage.cpp',
    link_with: libfundlogic,
    dependencies: gtest_dep)
test('stage unit test', test_stage_exe)

test_fund_exe = executable('test_fund', 'test/test_fund.cpp',
    link_with: libfundlogic,
    dependencies: gtest_dep)
test('fund unit test', test_fund_exe)
