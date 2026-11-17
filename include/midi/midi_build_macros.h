#if ! defined MIDI66_MIDI_BUILD_MACROS_H
#define MIDI66_MIDI_BUILD_MACROS_H

/*
 *  This file is part of midi66.
 *
 *  midi66 is free software; you can redistribute it and/or modify it under the
 *  terms of the GNU General Public License as published by the Free Software
 *  Foundation; either version 2 of the License, or (at your option) any later
 *  version.
 *
 *  midi66 is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 *  details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with midi66; if not, write to the Free Software Foundation, Inc., 59 Temple
 *  Place, Suite 330, Boston, MA  02111-1307  USA
 */

/**
 * \file          midi_build_macros.h
 *
 *  Macros that depend upon the build platform and developer desires.
 *
 * \library       midi66
 * \author        Chris Ahlstrom
 * \date          2026-10-17
 * \updates       2026-10-18
 * \license       See above.
 *
 * Introduction:
 *
 *      We have two ways of configuring the build for the midi66 library. This
 *      file presents features that depend upon the build platform, and build
 *      options that might be considered permanent, such as the option to pick
 *      from various JACK-processing callbacks.
 */

#include "platform_macros.h"            /* generic detecting of OS platform */

/**
 *  Official version number for this library. Already defined in the variable
 *  build_args in meson.build.
 *
 *      #define MIDI66_VERSION "0.2.0"
 */

/**
 *  This was the version of the RtMidi library from which this
 *  reimplementation was forked.  The RtMidi code is quite difficult to read,
 *  and so we adopted more readable coding and naming conventions.
 *
 *  Unlike the original Sequencer64/Seq66 implement of "rtmidi", this
 *  implementation more nearly aligns with RtMidi, and we force any new MIDI
 *  code to hew as much as possible to "midi66"/RtMidi operation.  This makes
 *  cross-platform support much easier.
 */

#define MIDI66_RTMIDI_VERSION    "5.0.0"     /* RtMidi revision at fork time */
#define MIDI66_RTMIDI_PATCHED    "5.0.0"     /* latest RtMidi version patch  */

/*
 * To do: Use this once the Android and the deprecated Windows UWP are
 * provisionally written.
 */

#define MIDI66_RTMIDI_VERSION_2  "6.0.0"     /* RtMidi revision at fork time */

/*
 * https://github.com/thestk/rtaudio.git
 */

#define MIDI66_RTAUDIO_VERSION   "6.0.0"     /* RtAudio revision at fork     */
#define MIDI66_RTAUDIO_PATCHED   "6.0.0"     /* latest RtAudio version patch */

/**
 *  Compiler/linker macros
 */

#if defined MIDI66_EXPORT

#if defined PLATFORM_WINDOWS || defined PLATFORM_CYGWIN

#define MIDI66_DLL_PUBLIC   __declspec(dllexport)
#define MIDI66_API          __declspec(dllexport)

#else

#if PLATFORM_GNUC >= 4
#define MIDI66_DLL_PUBLIC   __attribute__((visibility("default")))
#define MIDI66_API          __attribute__((visibility("default")))
#else
#define MIDI66_API
#define MIDI66_DLL_PUBLIC
#endif

#endif

#else                       /* not exporting    */

#define MIDI66_API
#define MIDI66_DLL_PUBLIC

#endif

/*
 *  Minimum, default, and maximum values for the most common parameters.
 */

#define MIDI66_DEFAULT_CLOCKS_PER_METRO   24     /* clocks per metronome     */
#define MIDI66_DEFAULT_32NDS_PER_QUARTER   8     /* 32nds per quarter        */
#define MIDI66_DEFAULT_PITCHBEND_SEMITONES 2     /* +/- pitchbend range      */
#define MIDI66_DEFAULT_INPUT_Q_SIZE      100     /* input queue size         */

#define MIDI66_MINIMUM_BEATS_PER_BAR       1     /* beats per bar            */
#define MIDI66_DEFAULT_BEATS_PER_BAR       4
#define MIDI66_MAXIMUM_BEATS_PER_BAR      32

#define MIDI66_MINIMUM_BEAT_WIDTH          1     /* quarter note             */
#define MIDI66_DEFAULT_BEAT_WIDTH          4
#define MIDI66_MAXIMUM_BEAT_WIDTH         32

#define MIDI66_MINIMUM_BPM                 2.0   /* beats per minute         */
#define MIDI66_DEFAULT_BPM               120.0
#define MIDI66_MAXIMUM_BPM               600.0

#define MIDI66_MINIMUM_PPQN               24     /* pulses per quarter note  */
#define MIDI66_LEGACY_PPQN               192
#define MIDI66_DEFAULT_PPQN              384
#define MIDI66_MAXIMUM_PPQN            19200     /* way more than needed     */

#define MIDI66_DEFAULT_US_PER_Q       500000     /* microseconds per quarter */

/**
 *  What Platform APIS to build? A few notes:
 *
 *      -   We still need to figure out how to detect the iPhone OS (see the
 *          platform_macros.h header file.)
 *      -   We need a way to get the user to specify that the
 *          MIDI66_BUILD_WEB_MIDI macro is defined universally.
 *      -   If no other build macros are found, then MIDI66_BUILD_DUMMY is
 *          defined.
 *      -   There are additional macros for the "extra" audio platforms:
 *          PulseAudio, OSS, Windows ASIO, WASAPI, and DS. But note that
 *          MIDI and Audio have their own separate lists of supported APIs.

#undef MIDI66_BUILD_ALSA
#undef MIDI66_BUILD_DUMMY
#undef MIDI66_BUILD_IPHONE_OS
#undef MIDI66_BUILD_JACK
#undef MIDI66_BUILD_MACOSX_CORE
#undef MIDI66_BUILD_OSS
#undef MIDI66_BUILD_PIPEWIRE
#undef MIDI66_BUILD_PULSEAUDIO
#undef MIDI66_BUILD_WEB_MIDI
#undef MIDI66_BUILD_WIN_ASIO
#undef MIDI66_BUILD_WIN_DS
#undef MIDI66_BUILD_WIN_MM
#undef MIDI66_BUILD_WIN_WASAPI
#undef MIDI66_BUILD_WIN_UWP
#undef MIDI66_BUILD_ANDROID
 */

#endif  // MIDI66_MIDI_BUILD_MACROS_H

/*
 * midi_build_macros.h
 *
 * vim: sw=4 ts=4 wm=4 et ft=c
 */
