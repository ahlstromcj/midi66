#*****************************************************************************
# meson.build (midi66)
#-----------------------------------------------------------------------------
##
# \file        tests/midi/meson.build
# \library     midi66
# \author      Chris Ahlstrom
# \date        2026-10-19
# \updates     2026-11-13
# \license     $XPC_SUITE_GPL_LICENSE$
#
#  This file is part of the "midi66" library. See the top-level meson.build
#  file for license information.
#
#  The liblib66_library_dep dependency provides some basic concepts for
#  using in all the "66" libraries.
#
#  The libcfg66_library_dep dependency provides some basic items for
#  all the "66" projects, such as the messaging and app-info
#  functions used by the midi66 library.
#
#-----------------------------------------------------------------------------

specstest_exe = executable(
   'specstest',
   sources : ['specstest.cpp'],
   dependencies : [
      libmidi66_dep, liblib66_library_dep, libcfg66_library_dep
      ]
   )

testfiles_exe = executable(
   'testfiles',
   sources : ['testfiles.cpp'],
   dependencies : [
      libmidi66_dep, liblib66_library_dep, libcfg66_library_dep
      ]
   )

test('MIDI Specs Test', specstest_exe)
test('MIDI Files Test', testfiles_exe)
   
#****************************************************************************
# meson.build (tests/midi)
#----------------------------------------------------------------------------
# vim: ts=3 sw=3 ft=meson
#----------------------------------------------------------------------------
