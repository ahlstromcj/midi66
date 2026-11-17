#*****************************************************************************
# meson.build (midi66)
#-----------------------------------------------------------------------------
##
# \file        include/meson.build
# \library     midi66
# \author      Chris Ahlstrom
# \date        2022-06-08
# \updates     2026-11-13
# \license     $XPC_SUITE_GPL_LICENSE$
#
#  This file is part of the "midi66" library. See the top-level meson.build
#  file for license information.
#
#  Here we locate the headers.  They reside in:
#
#     -  include. The main include directory, for general definitions.
#     -  include/midi. Contains headers adapted from the Seq66 project.
#
#-----------------------------------------------------------------------------

libmidi66_headers += files(
   'midi/calculations.hpp',
   'midi/clocking.hpp',
   'midi/controllers.hpp',
   'midi/drums.hpp',
   'midi/editable_event.hpp',
   'midi/editable_events.hpp',
   'midi/event.hpp',
   'midi/eventcodes.hpp',
   'midi/eventlist.hpp',
   'midi/eventspecs.hpp',
   'midi/file.hpp',
   'midi/filebase.hpp',
   'midi/macro.hpp',
   'midi/macros.hpp',
   'midi/measures.hpp',
   'midi/message.hpp',
   'midi/midibytes.hpp',
   'midi/patches.hpp',
   'midi/rpn.hpp',
   'midi/scales.hpp',
   'midi/song.hpp',
   'midi/splitter.hpp',
   'midi/timing.hpp',
   'midi/track.hpp',
   'midi/trackdata.hpp',
   'midi/trackinfo.hpp',
   'midi/tracklist.hpp',
   'midi/transcript.hpp'
   )

configure_file(
   input : 'midi66-config.h.in',
   output : 'midi66-config.h',
   configuration : cdata
   )

#****************************************************************************
# meson.build (include/midi66)
#----------------------------------------------------------------------------
# vim: ts=3 sw=3 ft=meson
#----------------------------------------------------------------------------
