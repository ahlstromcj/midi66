#*****************************************************************************
# meson.build (midi66/mftranscript)
#-----------------------------------------------------------------------------
##
# \file        mftranscript/meson.build
# \library     midi66
# \author      Chris Ahlstrom
# \date        2026-11-10
# \updates     2026-11-22
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
#-----------------------------------------------------------------------------

mftranscript_exe = executable(
   'mftranscript',
   sources : ['src/mftranscript.cpp'],
   include_directories : [ 'include' ],
   dependencies : [
      libmidi66_dep, liblib66_library_dep, libcfg66_library_dep
      ],
   install : true
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
