## unit tests
unit_test_list = [
  'test_unit_gps_time',
  'test_unit_utc_time',
]

foreach test_base : unit_test_list
  message('adding unit test ' + test_base)
  temp_exe = executable(test_base + '.cpp',
                        [test_base + '.cpp',
                         'test_main.cpp'],
                        include_directories : all_inc_dirs,
                        link_with : all_libs,
                        install : false,
                        dependencies : test_deps
                       )
  test(test_base, temp_exe,
       timeout : 0,
       env : ['srcdir=' + meson.project_source_root()],
       suite : 'unit')
endforeach
