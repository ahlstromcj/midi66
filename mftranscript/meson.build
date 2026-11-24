#*****************************************************************************
# meson.build (midi66/mftranscript)
#-----------------------------------------------------------------------------
##
# \file        mftranscript/meson.build
# \library     midi66
# \author      Chris Ahlstrom
# \date        2026-11-10
# \updates     2026-11-24
# \license     $XPC_SUITE_GPL_LICENSE$
#
#  This file is part of the "midi66" project. See the top-level meson.build
#  file for license information.
#
#  The liblib66_library_dep dependency provides some basic concepts for
#  using in all the "66" libraries.
#
#  The libcfg66_library_dep dependency provides some basic items for
#  all the "66" projects, including the midi66 library and mftranscript
#  program.
#
#  We use get_static_lib() to avoid jacking around with the installed
#  shared libraries. Only works for the main project library.
#
# After --install, one might have to run 'sudo ldconfig' to allow
# mftranscript to see the shared libraries.
#
# Debian:
#
#  $ readelf -d /usr/local/bin/mftranscript | grep -E 'PATH'
#  RUNPATH... Library runpath: [/usr/local/lib/x86-64-linux-gnu/midi66...
#
# None of the following works:
#
#  install_rpath: '$ORIGIN:$ORIGIN/../lib:$ORIGIN/../lib64'
#
# if host_machine.system() == 'darwin'
#    add_project_link_arguments('-Wl,-rpath,@loader_path', language: ['c', 'cpp'])
# else
#    add_project_link_arguments('-Wl,-rpath,$ORIGIN', language: ['c', 'cpp'])
# endif
#
#     libmidi66_dep, liblib66_library_dep, libcfg66_library_dep
#
#     cfg66_library_build.get_static_lib()
#     xpc66_library_build.get_static_lib()
#
#-----------------------------------------------------------------------------

mftranscript_exe = executable(
   'mftranscript',
   sources : ['src/mftranscript.cpp'],
   include_directories : [ 'include' ],
   dependencies : [
      libmidi66_dep, liblib66_library_dep, libcfg66_library_dep
      ],
   link_with : [
      midi66_library_build.get_static_lib()
      ],
   install : true,
   install_rpath : midi66_libdir
   )

#-----------------------------------------------------------------------------
# This is the main application program, not a test:
#
#     test('MIDI File Transcript', mftranscript_exe)
#
#-----------------------------------------------------------------------------
   
#****************************************************************************
# meson.build (midi66/mftranscript)
#----------------------------------------------------------------------------
# vim: ts=3 sw=3 ft=meson
#----------------------------------------------------------------------------
