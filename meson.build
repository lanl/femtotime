# building the femtotime repository in Diorama.  You can build with:
# meson setup _mbuild --prefix=$DIO_LOCAL_PREFIX
# ninja -C _mbuild

# to enable docs you can try:
# meson setup _mbuilddocs --prefix=$DIO_LOCAL_PREFIX -Dbuild_docs=true
# ninja -C _mbuilddocs

project('femtotime', 'cpp', version : '0.2.3',
       license : 'GPL-3.0-or-later', meson_version: '>= 0.58.2',
       default_options : ['cpp_std=c++20'])

conf_data = configuration_data()
conf_data.set('VERSION', meson.project_version())
conf_data.set('PACKAGE_NAME', meson.project_name())
conf_data.set('PREFIX', get_option('prefix'))
conf_data.set('COMPRESS_SUFFIX', 'xz')

configure_file(input: meson.project_name() + '.spec.in',
               output: meson.project_name() + '.spec',
               configuration: conf_data,
               install: false)

cpp = meson.get_compiler('cpp')

prefix = get_option('prefix')
pkgdatadir = prefix + '/share/' + meson.project_name()
add_project_arguments([
                       '-DFEMTOTIME_PKGDATADIR="' + pkgdatadir + '"',
		      ],
		      language : 'cpp')

src_dir = include_directories('src')
source_root_dir = include_directories('.')
all_inc_dirs = [src_dir, source_root_dir]

install_headers('src/femtotime/GPStime.hpp',
  'src/femtotime/time_constants.hpp',
  'src/femtotime/msgpack.hpp',
  install_dir : 'include/femtotime')

fmt_dep = dependency('fmt')
msgpack_dep = dependency('msgpack', required : false)
msgpackcxx_dep = dependency('msgpack-cxx', required : false)
if msgpack_dep.found() or msgpackcxx_dep.found()
  message('using msgpack')
  add_project_arguments([
                         '-DHAVE_MSGPACK=1',
                         ],
                         language : 'cpp')
endif
all_deps = [fmt_dep, msgpack_dep, msgpackcxx_dep]

libfemtotime = shared_library('femtotime',
        'src/GPStime.cpp',
	    include_directories : all_inc_dirs,
           dependencies : all_deps,
           install : true)

all_libs = [libfemtotime]

# install some (for now minimal) documentation
install_man('man/femtotime.3')

# now introduce the tests
cppunit_dep = dependency('cppunit', required : false)
if cppunit_dep.found()
  test_deps = all_deps
  test_deps += cppunit_dep
  subdir('testsrc/unit')
endif
# No integration or regression tests yet
# subdir('testsrc/integration')
# subdir('testsrc/regression')

# # stylecheck tests
# nsiqcppstyle = find_program('nsiqcppstyle', required : false)
# if nsiqcppstyle.found()
#   message('making stylecheck target')
#   cmd_str = ('python2 /usr/lib/nsiqcppstyle/nsiqcppstyle.py --no-update --output=xml -f ' + meson.project_source_root()
#              + '/filefilter.txt ' + meson.project_source_root())
#   stylecheck = custom_target('stylecheck',
#                              output : 'stylecheck',
#                              command : cmd_str.split())
#   cmd_str_e = ('python2 /usr/lib/nsiqcppstyle/nsiqcppstyle.py --no-update --output=emacs -f ' + meson.project_source_root()
#                + '/filefilter.txt ' + meson.project_source_root())
#   message('cmd_str_e: ' + cmd_str_e)
#   message(cmd_str_e)
#   stylecheck_emacs = custom_target('stylecheck-emacs',
#                                    output : 'stylecheck-emacs',
#                                    command : cmd_str_e.split())
# endif

# now prepare documentation
# if get_option('build_docs')
#   subdir('doc/sphinx_docs')
# endif

# finally prepare a pkg-config .pc file for the libraries in this package
pkg = import('pkgconfig')
pkg.generate(libraries : libfemtotime,
             subdirs : '.',
             version : meson.project_version(),
             name : meson.project_name(),
             filebase : meson.project_name(),
             requires : [fmt_dep, msgpack_dep],
             description : 'femtotime lib')
