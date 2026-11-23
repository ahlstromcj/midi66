#*****************************************************************************
# meson.build (midi66)
#-----------------------------------------------------------------------------
##
# \file        meson.build
# \library     midi66
# \author      Chris Ahlstrom
# \date        2022-06-07
# \updates     2026-11-22
# \license     $XPC_SUITE_GPL_LICENSE$
#
#  This file is part of the midi66 library.
#
#  The midi66 library is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#
#  The midi66 library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with the midi66 library. If not, see <https://www.gnu.org/licenses/>.
#
#  See midi66/meson.build for more information.
#
#  The midi66 library is a heavy refactoring of RtMidi 5.0.0, meant to replace
#  the seq_rtmidi library in version 2 of Seq66.
#
#  For information on meson subprojects, see
#
#     https://mesonbuild.com/Subprojects.html
#
#  Also see the "Pitchfork" project layout conventions.
#
#     https://github.com/vector-of-bool/pitchfork
#
#   cxx-pflR1
#   The Pitchfork Layout (PFL)
#   A Collection of Interesting Ideas, 2022-06-04
#
#   https://api.csswg.org/bikeshed/?force=1&url=
#       https://raw.githubusercontent.com/vector-of-bool/
#           pitchfork/develop/data/spec.bs
#
#  Note that we prefer to employ "3.1.2 Separate Header Placement".
#
# meson_version:
#
#     On mesonbuild.com: 1.4
#     Installed via pip: 1.4
#
#     If errors occur in running this build, then consider replacing your
#     distro's old meson by removing it and running:
#
#        # pip install meson
#
#     Requires version >= 1.1.0 which enables "preserve_paths" and usage of
#     the "meson.options" file.
#
# Windows:
#
#     Installed Git Bash, QtCreator with MingW usage as per Seq66, Python 3
#     from an installer (added the feature that allows > 260 character path
#     names), and Meson 1.4 from an MSI installer.  Did not install the
#     Visual Studio Community edition on our Windows virtual machine.
#
# Setup:
#
#     See the "work" bash script.
#
# Release:
#
#     See the description of the 'buildtype' option in contrib/meson.text.
#     Specify "--buildtype=release" on the meson command line.
#
# Reconfigure:
#
#        $ meson configure -Dprefix=/home/me/tmp -Dbuildtype=release build/
#
# Installation:
#
#     The library is build/libXXX66-0.1.a. We will install the XXX66 libraries
#     all in in /usr/lib/midi66-0.1 or /usr/local/lib/midi66-0.1, rather
#     than a subdirectory for each of these somewhat related libraries..
#
# Build one module only (example):
#
#     $ cd build
#     $ ninja libmidi66-0.1.a.p/src_midi_busarray.cpp.o
#
# Also see:
#
#     "Porting poppler to meson"
#     https://fasterthanli.me/series/dont-shell-out/part-5
#
#     "Getting started with Meson in C++ (part 2)"
#     https://germandiagogomez.medium.com/
#        getting-started-with-meson-in-c-part-2-58150354ff17
#
# Note that C++14 is required, in order to support the usage of the
# std::make_unique<>() function, but we've boosted it to C++17.
#
#-----------------------------------------------------------------------------

project('midi66',
   [ 'c', 'cpp' ],
   version : '0.2',
   license : 'GPLv3',
   meson_version : '>=1.4.0',
   default_options : [
      'c_std=c11', 'cpp_std=c++17', 'warning_level=3',
      'default_library=both'
   ]
   )

#-----------------------------------------------------------------------------
# Conditional enabling of the Potext translation library.
#-----------------------------------------------------------------------------

midi66_use_potext = false
if get_option('potext')

   midi66_use_potext = true
   add_project_arguments('-DMIDI66_USE_POTEXT', language : [ 'c', 'cpp' ])

endif

#-----------------------------------------------------------------------------
# Information for this sub-project.
#-----------------------------------------------------------------------------
#
#  Tricky: the *.pc file must have a base name matching the name of the
#  subproject *.wrap file.
#
#-----------------------------------------------------------------------------

midi66_pkg_description = 'Refactoring/extension of the Seq66 MIDI file library.'
midi66_pkg_version = meson.project_version()
midi66_api_version = '0.2'
midi66_info_date = '2026-11-17'
midi66_info_build_type = get_option('buildtype')
midi66_info_build_root = meson.project_build_root()
midi66_info_lib_type = get_option('default_library')
midi66_info_project_root = meson.project_source_root()
midi66_debug = get_option('buildtype').contains('debug')
midi66_project_base = 'midi66-@0@'.format(midi66_api_version)
midi66_pkgconfig_name = 'libmidi66'
midi66_subproj = meson.is_subproject()

if midi66_debug
   if not midi66_subproj
      add_project_arguments('-DDEBUG', language : 'cpp')
      add_project_arguments('-fno-inline-functions', language : 'cpp')
   endif
endif

#-----------------------------------------------------------------------------
# Easy access to directory options.
#-----------------------------------------------------------------------------

midi66_dir = 'midi66-@0@'.format(midi66_api_version)
midi66_prefix = get_option('prefix')
midi66_bindir = join_paths(midi66_prefix, get_option('bindir'))
midi66_includedir = join_paths(midi66_prefix, get_option('includedir'), midi66_dir)
midi66_libdir = join_paths(midi66_prefix, get_option('libdir'), midi66_dir)
midi66_datadir = join_paths(midi66_prefix, get_option('datadir'), midi66_dir)
midi66_docdir = join_paths(midi66_prefix, midi66_datadir, 'doc', midi66_dir)
libraries_top = meson.project_source_root()
libmidi66_include_top = join_paths(libraries_top, 'include')

#-----------------------------------------------------------------------------
# The *.pc file goes into $libdir/midi66-0.1/pkgconfig instead of
# $libdir/pkgconfig as the meson documentation asserts.  Not sure why, but
# wiring the expected directory here works.
#-----------------------------------------------------------------------------

alt_pkgconfig_libdir = join_paths(
   midi66_prefix, get_option('libdir'), 'pkgconfig'
   )

#-----------------------------------------------------------------------------
# libmidi66_headers provides the list of all headers in the project. These are
# currently all installable, public headers. They are set in
# include/meson.build.
#
# libmidi66_sources provides the source-code files. They are set in
# src/meson.build.
#-----------------------------------------------------------------------------

libmidi66_headers = []
libmidi66_sources = []
build_args = []

#-----------------------------------------------------------------------------
# This library has no platform-dependent characteristics. But just in case...
#-----------------------------------------------------------------------------

os_unix     = false
os_freebsd  = false
os_linux    = false
os_win32    = false
os_darwin   = false

if host_machine.system() == 'darwin'
   os_darwin = true
elif host_machine.system() == 'windows'
   os_win32 = true
elif host_machine.system() == 'linux'
   os_linux = true
elif host_machine.system() == 'freebsd'
   os_freebsd = true
endif

os_unix = not os_win32

#-----------------------------------------------------------------------------
# Each include file directory is added to the list passed to the compiler.
# We prefer to use the style '#include "cfg/header.hpp".
#-----------------------------------------------------------------------------

libmidi66_includes = include_directories('.',
   'include',
   'include/midi'
   )

#-----------------------------------------------------------------------------
# Used by the midi66_xxx_version() function.
#-----------------------------------------------------------------------------

build_args += [
   '-DMIDI66_NAME=' + '"' + meson.project_name() + '"',
   '-DMIDI66_VERSION=' + '"' + meson.project_version() + '"'
   ]

#-----------------------------------------------------------------------------
# Set up some "HAVE" macros.  By default the header file is generated in the
# "build" directory. If configure_file() is moved to include/meson.build,
# the the header goes into "build/include", which is what we want.
#-----------------------------------------------------------------------------

cc = meson.get_compiler('cpp')
cdata = configuration_data()
cdata.set10('fcntl_h', cc.has_header('fcntl.h'))
cdata.set10('limits_h', cc.has_header('limits.h'))

#-----------------------------------------------------------------------------
# Potential sub-projects
#-----------------------------------------------------------------------------

if midi66_use_potext

   libpotext_dep = subproject('libpotext').get_variable('libpotext_dep')
   libpotext_library_dep = dependency(
      'libpotext', fallback : [ 'libpotext', 'libpotext_dep' ]
      )

endif

liblib66_dep = subproject('liblib66').get_variable('liblib66_dep')
liblib66_library_dep = dependency(
   'liblib66', fallback : [ 'liblib66', 'liblib66_dep' ]
   )

libcfg66_dep = subproject('libcfg66').get_variable('libcfg66_dep')
libcfg66_library_dep = dependency(
   'libcfg66', fallback : [ 'libcfg66', 'libcfg66_dep' ]
   )

#  default_options : [ 'default_library=static' ]

libxpc66_dep = subproject('libxpc66').get_variable('libxpc66_dep')
libxpc66_library_dep = dependency(
   'libxpc66', fallback : [ 'libxpc66', 'libxpc66_dep' ]
   )

#-----------------------------------------------------------------------------
# Sub-directories
#-----------------------------------------------------------------------------

subdir('include')
subdir('src')

#-----------------------------------------------------------------------------
# Dependencies on Linux or FreeBSD
#-----------------------------------------------------------------------------
#
# pthreads:
#
#     The 'threads' dependency is required, but currently we don't care
#     about the version.
#
#-----------------------------------------------------------------------------

if os_unix

threads_dep = dependency('threads', required : true)
system_depends = [ threads_dep ]

endif

if os_win32

   winmm_dep = cc.find_library('winmm')
   system_depends = [ winmm_dep ]

endif

#-----------------------------------------------------------------------------
# We recommemd using a recent version of meson by installing it outside the
# Linux distro's repository.  For example, meson on Ubuntu is pretty damn
# old.
#
# The choice of library type is of some import for debugging:
#
#     library:          Defaults to a shared library. The User setting is
#                       'default_library'.  See project() above.
#     shared_library:   Cannot see source files until stepped into.
#     static_library:   Can see the source files when debugging.
#     both_libraries:   Product both static and shared libraries.
#
# This one causes the linker to not see the library functions, wotta pain:
#
#     gnu_symbol_visibility: 'hidden'
#
# Also see the https://gcc.gnu.org/wiki/Visibility site.
#
#-----------------------------------------------------------------------------

if midi66_use_potext

   midi66_library_build = library(
      midi66_project_base,
      libmidi66_sources,
      install : not midi66_subproj,
      install_dir : midi66_libdir,
      c_args : build_args,
      cpp_args : build_args,
      dependencies : [
         system_depends, liblib66_library_dep, libcfg66_library_dep,
         libxpc66_library_dep, libpotext_library_dep
         ],
      include_directories : [ libmidi66_includes ]
      )

else

   midi66_library_build = library(
      midi66_project_base,
      libmidi66_sources,
      install : not midi66_subproj,
      install_dir : midi66_libdir,
      c_args : build_args,
      cpp_args : build_args,
      dependencies : [
         system_depends, liblib66_library_dep, libcfg66_library_dep,
         libxpc66_library_dep
         ],
      include_directories : [ libmidi66_includes ]
      )

endif

#-----------------------------------------------------------------------------
# Making the internal library usable as a Meson subproject requires using
# declare_dependency().
#-----------------------------------------------------------------------------

libmidi66_dep = declare_dependency(
   include_directories : [ libmidi66_includes ],
   link_with : [ midi66_library_build ]
   )

#-----------------------------------------------------------------------------
# Make this library usable from the system's package manager.
#-----------------------------------------------------------------------------

install_headers(libmidi66_headers, subdir : midi66_dir, preserve_path : true)

#-----------------------------------------------------------------------------
# Package config
#-----------------------------------------------------------------------------
#  On Arch Linux, the .pc file is installed to /usr/local/lib/pkgconfig.
#  On Ubuntu, the .pc file is installed to
#     /usr/local/lib/x86_64-linux-gnu/pkgconfig.
#-----------------------------------------------------------------------------

if not midi66_subproj

   pkg_mod = import('pkgconfig')
   pkg_mod.generate(
      midi66_library_build,
      filebase : midi66_pkgconfig_name,
      version : midi66_pkg_version,
      name : midi66_project_base,
      description : midi66_pkg_description,
      install_dir : alt_pkgconfig_libdir,
      subdirs : midi66_dir,
      libraries : midi66_library_build,
      url : 'https://github.com/ahlstromcj/midi66',
      )

# TODO: figure out where to put and install the logo

   install_data(
      files(
         'ChangeLog',
         'LICENSE.md',
         'README.md'
         ),
      install_dir : get_option('docdir')
      )

   if get_option('enable-tests')
      subdir('tests')
   endif

   subdir('mftranscript')

'''
   if get_option('enable-docs')
      subdir('doc')
   endif
'''

endif

#-----------------------------------------------------------------------------
# Info
#-----------------------------------------------------------------------------

summary(
   {
      'Date' : midi66_info_date,
      'Filebase' : midi66_project_base,
      'Version' : midi66_pkg_version,
      'Base' : midi66_project_base,
      'Description' : midi66_pkg_description,
      'Source Root' : midi66_info_project_root,
      'Headers Root' : libmidi66_include_top,
      'Sub Dir' : midi66_project_base,
      'Build Type' : midi66_info_build_type,
      'Build Root' : midi66_info_build_root,
      'Library Type' : midi66_info_lib_type,
      'Subproject' : midi66_subproj
   },
   section : 'Project'
   )

summary(
   {
      'prefix' : midi66_prefix,
      'includedir' : midi66_includedir,
      'bindir' : midi66_bindir,
      'libdir' : midi66_libdir,
      'datadir' : midi66_datadir,
      'appdatadir' : midi66_datadir
   },
   section : 'Midi66 Install Directories'
   )

#****************************************************************************
# meson.build (midi66)
#----------------------------------------------------------------------------
# vim: ts=3 sw=3 ft=meson
#----------------------------------------------------------------------------
