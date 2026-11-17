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
 * \file          specstest.cpp
 *
 *      Simple program to test text-to-event-to-text conversions of
 *      MIDI messages (events).
 *
 * \library       midi66
 * \author        Chris Ahlstrom
 * \date          2026-10-22
 * \updates       2026-11-16
 * \license       See above.
 *
 *  It assumes it is run from the top-level directory of the midi66 project.
 */

#include <iostream>                     /* std::cout and std::cerr          */
#include <utility>                      /* std::pair template class         */
#include <vector>                       /* std::vector template class       */

#include "cli/parser.hpp"               /* cli::parser, etc.                */
#include "midi/calculations.hpp"        /* midi::split_14_bits()            */
#include "midi/eventspecs.hpp"          /* midi::conversion functions       */
#include "midi/song.hpp"                /* midi::song class                 */
#include "midi/transcript.hpp"          /* midi::transcript class           */
#include "util/msgfunctions.hpp"        /* util::investigate()              */
#include "util/strfunctions.hpp"        /* util::string_to_int()            */

namespace   // anonymous
{

int s_test_counter { 0 };
bool s_verbose { false };

/**
 *  Tests of the MIDI file header ("MThd") and the track marker ("MTrk").
 *
 *      midi::bytes mthd { 0x4D, 0x54, 0x68, 0x64 };
 *      midi::bytes mtrk { 0x4D, 0x54, 0x72, 0x6B };
 */

/**
 *  The format of the MIDI file header is described at about line 300
 *  in the midi/file module. Also see the functions in the eventspecs
 *  module. The format used there is:
 *
 *      "MThd smf=%d tracks=%d ppqn=%d"
 *
 *      -   0x00. "MThd", [ 4D 54 68 64 ]
 *      -   0x04. Header Length, 4 bytes, [ 00 00 00 06 ].
 *      -   0x08. MIDI Format (SMF). 2 bytes, 00, 01, or 02. [ 00 01 ]
 *      -   0x0A. Number of Tracks. 2 bytes, 16 bits. [ 00 13 ]
 *      -   0x0C. Divisions (PPQN). 2 bytes. [ 00 C0 ]
 *
 *  Unlike the other tests in this test module, decimal format is used.
 *  No need for deltas nor commenting.
 *
 *  The string_to_midi_header() function checks for valid values,
 *  but we provide only good values here.
 */

using header_test_values = struct
{
    int smf;
    int tracks;
    int ppq;
    std::string inputline;
};

header_test_values s_header_values []
{
    { 0,  1,   24, "MThd smf=0 tracks=1 ppqn=24"    },
    { 1, 10,  192, "MThd smf=1 tracks=10 ppqn=192"  },
    { 1,  2,  384, "MThd smf=1 tracks=2 ppqn=384"   },
    { 1, 20, 9600, "MThd smf=1 tracks=20 ppqn=9600" },
    { 2,  1,  240, "MThd smf=2 tracks=1 ppqn=240"   }
};

bool
header_tests ()
{
    bool result { false };
#if defined PLATFORM_DEBUG
    if (s_test_counter == 99)
        printf("Break\n");                          /* debug breakpoint     */
#endif
    for (const auto & hdr : s_header_values)
    {
        int smffmt, trackct, ppqv;
        bool ok
        {
            midi::string_to_midi_header
            (
                hdr.inputline,
                smffmt, trackct, ppqv               /* output parameters    */
            )
        };
        if (ok)
        {
            ok =
            (
                smffmt == hdr.smf &&
                trackct == hdr.tracks &&
                ppqv == hdr.ppq
            );
        }
        if (ok)
        {
            std::string headerstr
            {
                midi::midi_header_to_string(smffmt, trackct, ppqv)
            };
            if (s_verbose)
            {
                ++s_test_counter;
                std::cout
                    << "Test #" << s_test_counter << std::endl
                    << "Input line:   '"
                    << hdr.inputline << "'" << std::endl
                    << "Actual out:   '" << headerstr << "'\n"
                    << std::endl
                    ;
            }
            if (headerstr == hdr.inputline)         /* out same as input    */
                result = true;
            else
            {
                result = false;
                break;
            }
        }
        else
        {
            result = false;
            break;
        }
    }
    if (! result)
        std::cerr << "'MIDI header' test failed." << std::endl;

    return result;
}

/**
 *  Tests of an input line and an output line. Very strict matches
 *  are expected.
 */

using byte_test_values = struct
{
    long delta;
    int statusbyte;
    int d0;
    int d1;
    std::string inputline;
    std::string outputline;
};

std::vector<byte_test_values> s_event_values
{
    /*
     * 0x90. Basic smoke test. Note On, channel 0.
     */

    {
        0, 0x90, 0x3c, 0x40,
        "0 NoteOn chan=0 note=60 vel=64",
        "0 NoteOn chan=0 note=60 vel=64 # C4"
    },

    /*
     * 0x90. Verifies sscanf() handling of spaces.
     */

    {
        0, 0x90, 0x3c, 0x40,
        "     0 NoteOn chan= 0 note= 60 vel= 64     # C4",
        "0 NoteOn chan=0 note=60 vel=64 # C4"
    },
    /*
     * 0x95. Note On, channel 5 (re 0).
     */

    {
        0, 0x95, 0x3d, 0x40,
        "0 NoteOn chan=5 note=61 vel=64",
        "0 NoteOn chan=5 note=61 vel=64 # C#4"
    },

    /*
     * 0x85. Note Off, channel 5 (re 0).
     */

    {
        0, 0x85, 0x3d, 0x0,
        "     0 NoteOff chan= 5 note= 61 vel=  0     # C#4",
        "0 NoteOff chan=5 note=61 vel=0 # C#4"
    },

    /*
     * 0xA0. Aftertouch, channel 0.
     */

    {
        192, 0xa0, 104, 64,
        "192 Aftertouch chan=0 note=104 vel=64",
        "192 Aftertouch chan=0 note=104 vel=64 # G#7"
    },

    /*
     * Control change.
     */

    {
        192, 0xBF, 0x7F, 0x7F,
        "192 CtrlChange chan=15 ctrl=127 level=127",
        "192 CtrlChange chan=15 ctrl=127 level=127 # PolyOn"
    },
    {
        960000, 0xBA, 7, 127,
        "960000 CtrlChange chan=10 ctrl=7 level=127",
        "960000 CtrlChange chan=10 ctrl=7 level=127 # Volume"
    },
    {
        0, 0xB0, 14, 0x0,                           /* ctrl 14 is undefined */
        "0 CtrlChange chan=0 ctrl=14 level=0",
        "0 CtrlChange chan=0 ctrl=14 level=0 # 14"
    },

    /*
     * Program change (patch module).
     *
     * When we say "d1 is not used", this means it is not stored
     * in the message bytes.
     */

    {
        0, 0xC1, 0, 0,                              /* d1 is not used       */
        "0 ProgChange chan=1 prog=0",
        "0 ProgChange chan=1 prog=0 # Acoustic Grand Piano"
    },
    {
        0, 0xC1, 104, 0,
        "0 ProgChange chan=1 prog=104",
        "0 ProgChange chan=1 prog=104 # Sitar"
    },

    /*
     * 0xD0. ChPressure.
     */

    {
        0, 0xD8, 112, 0,                            /* d1 is not used       */
        "0 ChPressure chan=8 level=112",
        "0 ChPressure chan=8 level=112"
    },

    /*
     * 0xE0. PitchWheel. The center value is 8192 (0x2000).
     * Should we use percent=-100 to percent=100? No, not precise enough.
     * We will make the range from -8192 to 8192, so that 0 is no
     * bend. Below, 0x0 is the LSB value, and 0x40 is the MSB value.
     */

    {
        768, 0xE0, 0x0, 0x40,                       /* 0x2000 pitch center  */
        "768 PitchWheel chan=0 bend=0",
        "768 PitchWheel chan=0 bend=0"
    },
    {
        19200, 0xE7, 0x0, 0x0,                      /* bottom pitch bend    */
        "19200 PitchWheel chan=7 bend=-8192",
        "19200 PitchWheel chan=7 bend=-8192"
    },
    {
        39200, 0xE7, 0x7F, 0x7F,                    /* top pitch bend       */
        "39200 PitchWheel chan=7 bend=8191",
        "39200 PitchWheel chan=7 bend=8191"
    },
    {
        39200, 0xE7, 0x7F, 0x7F,                    /* above top pitch bend */
        "39200 PitchWheel chan=7 bend=8192",
        "39200 PitchWheel chan=7 bend=8191"
    },

    /*
     * 0xF1. QuarterFrame.
     */

    {
        0, 0xF1, 0xBB, 0,
        "0 QuarterFrame hmsf=BB",
        "0 QuarterFrame hmsf=BB"
    },

    /*
     * 0xF2. SongPosition.
     */

    {
        0, 0xF2, 0x0, 0,
        "0 SongPosition beats=0",
        "0 SongPosition beats=0"
    },
    {
        0, 0xF2, 0x7F, 0x7F,
        "0 SongPosition beats=16383",
        "0 SongPosition beats=16383"
    },
    {
        0, 0xF2, 0x7F, 0x7F,
        "0 SongPosition beats=16384",
        "0 SongPosition beats=16383"
    },

    /*
     * 0xF3. SongSelect.
     */

    {
        0, 0xF3, 0x0, 0,
        "0 SongSelect song=0",
        "0 SongSelect song=0"
    },
    {
        0, 0xF3, 0x7F, 0,
        "0 SongSelect song=127",
        "0 SongSelect song=127"
    },

    /*
     * 0xF6. TuneRequest.
     */

    {
        0, 0xF6, 0x0, 0,
        "0 TuneRequest",
        "0 TuneRequest"
    },

    /*
     * 0xF7. EOX.
     * 0xF8. MIDIClock.
     * 0xF9. TimingTick.
     * 0xFA. ClkStart.
     * 0xFB. ClkContinue.
     * 0xFC. ClkStop.
     * 0xFE. ActiveSense.
     */

    {
        177, 0xF7, 0x0, 0,
        "177 EOX",
        "177 EOX"
    },
    {
        0, 0xF8, 0x0, 0,
        "0 MIDIClock",
        "0 MIDIClock"
    },
    {
        0, 0xF9, 0x0, 0,
        "0 TimingTick",
        "0 TimingTick"
    },
    {
        0, 0xFA, 0x0, 0,
        "0 ClkStart",
        "0 ClkStart"
    },
    {
        0, 0xFB, 0x0, 0,
        "0 ClkContinue",
        "0 ClkContinue"
    },
    {
        0, 0xFC, 0x0, 0,
        "0 ClkStop",
        "0 ClkStop"
    },
    {
        0, 0xFE, 0x0, 0,
        "0 ActiveSense",
        "0 ActiveSense"
    },
};

/**
 * SysEx test data structure.
 *
 * The input line includes 0xF0, and the output line should be the
 * same. All values are in hex format here.
 */

using sysex_test_values = struct
{
    long delta;
    std::string inputline;
    std::string outputline;
};

/**
 *  SysEx samples.
 */

std::vector<sysex_test_values> s_sysex_values
{
    /*
     * Generic reset
     */

    {
        0,
        "0 SysEx 0xF0 0x7E 0x7F 0x09 0x01 0xF7",
        "0 SysEx 0xF0 0x7E 0x7F 0x09 0x01 0xF7 # Non-realtime",
    },

    /*
     * MIDI ping event
     */

    {
        192,
        "192 SysEx 0xF0 0x7E 0x7F 0x06 0x01 0xF7",
        "192 SysEx 0xF0 0x7E 0x7F 0x06 0x01 0xF7 # Non-realtime"
    },

    /*
     * A long SysEx event. Note: we support non-0x input,
     * but not non-0x output, at this time.
     */

    {
        0,
#if 0
        "0 SysEx "
        "F0 41 10 16 12 03 00 00 02 07 00 32 00 00 01 00 50 "
        "07 00 00 00 00 00 00 00 30 18 32 0C 00 01 00 50 "
        "08 00 00 00 00 00 00 01 1F 18 32 0C 00 01 00 50 "
        "07 00 00 00 00 00 00 01 0E 18 32 0C 00 01 00 50 "
        "08 00 00 00 00 00 00 00 29 18 32 0C 00 01 00 50 "
        "04 00 00 00 00 00 00 00 03 18 32 0C 00 01 00 50 "
        "0A 00 00 00 00 00 00 01 2E 18 32 0C 00 01 00 50 "
        "00 00 00 00 00 00 00 01 3A 18 32 0C 00 01 00 50 "
        "0E 00 00 00 00 00 00 31 F7",
#else
        "0 SysEx "
        "0xF0 0x41 0x10 0x16 0x12 0x03 0x00 0x00 "
        "0x02 0x07 0x00 0x32 0x00 0x00 0x01 0x00 0x50 "
        "0x07 0x00 0x00 0x00 0x00 0x00 0x00 0x00 "
        "0x30 0x18 0x32 0x0C 0x00 0x01 0x00 0x50 "
        "0x08 0x00 0x00 0x00 0x00 0x00 0x00 0x01 "
        "0x1F 0x18 0x32 0x0C 0x00 0x01 0x00 0x50 "
        "0x07 0x00 0x00 0x00 0x00 0x00 0x00 0x01 "
        "0x0E 0x18 0x32 0x0C 0x00 0x01 0x00 0x50 "
        "0x08 0x00 0x00 0x00 0x00 0x00 0x00 0x00 "
        "0x29 0x18 0x32 0x0C 0x00 0x01 0x00 0x50 "
        "0x04 0x00 0x00 0x00 0x00 0x00 0x00 0x00 "
        "0x03 0x18 0x32 0x0C 0x00 0x01 0x00 0x50 "
        "0x0A 0x00 0x00 0x00 0x00 0x00 0x00 0x01 "
        "0x2E 0x18 0x32 0x0C 0x00 0x01 0x00 0x50 "
        "0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x01 "
        "0x3A 0x18 0x32 0x0C 0x00 0x01 0x00 0x50 "
        "0x0E 0x00 0x00 0x00 0x00 0x00 0x00 0x31 "
        "0xF7",
#endif
        "0 SysEx "
        "0xF0 0x41 0x10 0x16 0x12 0x03 0x00 0x00 "
        "0x02 0x07 0x00 0x32 0x00 0x00 0x01 0x00 0x50 "
        "0x07 0x00 0x00 0x00 0x00 0x00 0x00 0x00 "
        "0x30 0x18 0x32 0x0C 0x00 0x01 0x00 0x50 "
        "0x08 0x00 0x00 0x00 0x00 0x00 0x00 0x01 "
        "0x1F 0x18 0x32 0x0C 0x00 0x01 0x00 0x50 "
        "0x07 0x00 0x00 0x00 0x00 0x00 0x00 0x01 "
        "0x0E 0x18 0x32 0x0C 0x00 0x01 0x00 0x50 "
        "0x08 0x00 0x00 0x00 0x00 0x00 0x00 0x00 "
        "0x29 0x18 0x32 0x0C 0x00 0x01 0x00 0x50 "
        "0x04 0x00 0x00 0x00 0x00 0x00 0x00 0x00 "
        "0x03 0x18 0x32 0x0C 0x00 0x01 0x00 0x50 "
        "0x0A 0x00 0x00 0x00 0x00 0x00 0x00 0x01 "
        "0x2E 0x18 0x32 0x0C 0x00 0x01 0x00 0x50 "
        "0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x01 "
        "0x3A 0x18 0x32 0x0C 0x00 0x01 0x00 0x50 "
        "0x0E 0x00 0x00 0x00 0x00 0x00 0x00 0x31 "
        "0xF7 # Manufacturer"
    },

};

/*
 * 0xF0. SysEx.
 */

bool
sysex_tests ()
{
    bool result { false };
#if defined PLATFORM_DEBUG
    if (s_test_counter == 99)
        printf("Break\n");                              /* debug breakpoint */
#endif
    for (const auto & syx : s_sysex_values)
    {
        midi::event ev = midi::string_to_midi_event(syx.inputline);
        long delta { long(ev.timestamp()) };
        if (delta >= 0)
        {
            if (delta != syx.delta)
            {
                result = false;
                break;
            }
            else
            {
                std::string evstr { midi_event_to_string(ev) };
                if (s_verbose)
                {
                    ++s_test_counter;
                    std::cout
                        << "Test #" << s_test_counter << std::endl
                        << "Input line:   '"
                        << syx.inputline << "'" << std::endl
                        << "Actual out:   '" << evstr << "'" << std::endl
                        ;
                }
                if (evstr == syx.outputline)
                    result = true;
                else
                {
                    result = false;
                    break;
                }
            }
        }
        else
        {
            result = false;
            break;
        }
    }
    if (! result)
        std::cerr << "'SysEx' test failed." << std::endl;

    return result;
}

/*
 * 0xFF. Meta.
 *
 * Format of a MIDI meta message:
 *
 *      0xFF 0xmm len data
 *
 *      -   0xFF is the exact status byte value.
 *      -   0xmm is the meta type byte.
 *      -   len is the varinum length value of the data portion.
 *      -   data is the variable-length data for the meta event.
 *
 *  The name of the meta event 0xmm is obtained from meta_label(0xmm).
 */

/**
 * Meta test data structure. Very similar to the SysEx data structure.
 *
 * The input line includes 0xFF, and the output line should be the
 * same. All values are in hex format here.
 *
 * See the editable_event module, line 156, for the format of the various MIDI
 * meta events.
 */

using meta_test_values = struct
{
    long delta;
    std::string inputline;
    std::string outputline;
};

/**
 *  Meta samples.
 *
 *  Do we want to use numbers or the Meta label? Numbers for now.
 */

std::vector<meta_test_values> s_meta_values
{
    /*
     * Sequence number:
     *
     *      FF 00 02 ss ss
     *      ^  ^  ^  ^  ^
     *      |  |  |  |  |
     *      |  |  |  |   --- LSB of 16-bit number
     *      |  |  |   ------ MSB of 16-bit number
     *      |  |   --------- 2 bytes of data follow
     *      |   ------------ Meta value for track number
     *       --------------- Meta code
     */

    {
        0,
        "0 Meta 0xFF 0x00 0x02 0x00 0x00",          /* track 0   */
        "0 Meta 0xFF 0x00 0x02 0x00 0x00 # Seq Number"
    },

    {
        192,
        "192 Meta 0xFF 0x00 0x02 0x01 0x00",        /* track 256 */
        "192 Meta 0xFF 0x00 0x02 0x01 0x00 # Seq Number"
    },

    /*
     * Meta text events:
     *
     * Text event:   "FF 01 len text"
     * Copyright:    "FF 02 len text"
     * Track name:   "FF 03 len text"
     * Instrument:   "FF 04 len text"
     * Lyric:        "FF 05 len text"
     * Marker:       "FF 06 len text"
     * Cue_point:    "FF 07 len text"
     * Program_name: "FF 08 len text"
     * Port_name:    "FF 09 len text"
     */

    {
        0,
        "0 Meta 0xFF 0x01 0x04 0x41 0x42 0x43 0x2E",            /* "ABC."   */
        "0 Meta 0xFF 0x01 0x04 0x41 0x42 0x43 0x2E # Text Event"
    },
    {
        0,
        "0 Meta 0xFF 0x02 0x04 0x41 0x42 0x43 0x2E",            /* "ABC."   */
        "0 Meta 0xFF 0x02 0x04 0x41 0x42 0x43 0x2E # Copyright"
    },
    {
        0,
        "0 Meta 0xFF 0x03 0x04 0x41 0x42 0x43 0x2E",            /* "ABC."   */
        "0 Meta 0xFF 0x03 0x04 0x41 0x42 0x43 0x2E # Seq Name"
    },
    {
        0,
        "0 Meta 0xFF 0x04 0x04 0x41 0x42 0x43 0x2E",            /* "ABC."   */
        "0 Meta 0xFF 0x04 0x04 0x41 0x42 0x43 0x2E # Instrument"
    },
    {
        0,
        "0 Meta 0xFF 0x05 0x04 0x41 0x42 0x43 0x2E",            /* "ABC."   */
        "0 Meta 0xFF 0x05 0x04 0x41 0x42 0x43 0x2E # Lyric"
    },
    {
        0,
        "0 Meta 0xFF 0x06 0x04 0x41 0x42 0x43 0x2E",            /* "ABC."   */
        "0 Meta 0xFF 0x06 0x04 0x41 0x42 0x43 0x2E # Marker"
    },
    {
        0,
        "0 Meta 0xFF 0x07 0x04 0x41 0x42 0x43 0x2E",            /* "ABC."   */
        "0 Meta 0xFF 0x07 0x04 0x41 0x42 0x43 0x2E # Cue Point"
    },
    {
        0,
        "0 Meta 0xFF 0x08 0x04 0x41 0x42 0x43 0x2E",            /* "ABC."   */
        "0 Meta 0xFF 0x08 0x04 0x41 0x42 0x43 0x2E # Program Name"
    },
    {
        0,
        "0 Meta 0xFF 0x09 0x04 0x41 0x42 0x43 0x2E",            /* "ABC."   */
        "0 Meta 0xFF 0x09 0x04 0x41 0x42 0x43 0x2E # Port Name"
    },

    /*
     * Deprecated events:
     *
     * MIDI channel: "FF 20 01 cc"
     * MIDI port:    "FF 21 01 pp"
     */

    {
        0,
        "0 Meta 0xFF 0x20 0x01 0x01",
        "0 Meta 0xFF 0x20 0x01 0x01 # MIDI Channel"
    },
    {
        0,
        "0 Meta 0xFF 0x21 0x01 0x05",
        "0 Meta 0xFF 0x21 0x01 0x05 # MIDI Port"
    },

    /*
     * End of track: "FF 2F 00"
     */

    {
        0,
        "0 Meta 0xFF 0x2F 0x00",
        "0 Meta 0xFF 0x2F 0x00 # End of Track"
    },

    /*
     * Tempo: "FF 51 03 tt tt tt"
     */

    {
        0,
        "0 Meta 0xFF 0x51 0x03 0x07 0xA1 0x20",                 /* 120 BPM  */
        "0 Meta 0xFF 0x51 0x03 0x07 0xA1 0x20 # Set Tempo"
    },

    /*
     * SMPTE offset: "FF 54 03 tt tt tt"
     */

    {
        0,
        "0 Meta 0xFF 0x54 0x03 0x00 0x00 0x00",                 /* 120 BPM  */
        "0 Meta 0xFF 0x54 0x03 0x00 0x00 0x00 # SMPTE Offset"
    },

    /*
     * Time signature: "FF 58 04 nn dd cc bb"
     */

    {
        0,
        "0 Meta 0xFF 0x58 0x04 0x04 0x02 0x18 0x08",            /* 4/4      */
        "0 Meta 0xFF 0x58 0x04 0x04 0x02 0x18 0x08 # Time Signature"
    },

    /*
     * Key signature: "FF 59 02 ss kk"
     */

    {
        0,
        "0 Meta 0xFF 0x59 0x02 0x00 0x00",                      /* C major  */
        "0 Meta 0xFF 0x59 0x02 0x00 0x00 # Key Signature"
    },

    /*
     * Sequencer-specific: "FF 7F len SeqSpec data bytes"
     */

    {
        0,
        "0 Meta 0xFF 0x7F 0x08 0x24 0x24 0x00 0x10 0x00 0x00 0x00 0x00",
        "0 Meta 0xFF 0x7F 0x08 0x24 0x24 0x00 0x10 0x00 0x00 0x00 0x00 "
            "# Seq Spec"
    }
};

/*
 * 0xFF. Meta events.
 */

bool
meta_tests ()
{
    bool result { false };
#if defined PLATFORM_DEBUG
    if (s_test_counter == 31)
        printf("Break\n");                              /* debug breakpoint */
#endif
    for (const auto & met : s_meta_values)
    {
        midi::event ev = midi::string_to_midi_event(met.inputline);
        long delta { long(ev.timestamp()) };
        if (delta >= 0)
        {
            if (delta != met.delta)
            {
                result = false;
                break;
            }
            else
            {
                std::string evstr { midi_event_to_string(ev) };
                if (s_verbose)
                {
                    ++s_test_counter;
                    std::cout
                        << "Test #" << s_test_counter << std::endl
                        << "Input line:   '"
                        << met.inputline << "'" << std::endl
                        << "Actual out:   '" << evstr << "'" << std::endl
                        ;
                }
                if (evstr == met.outputline)
                    result = true;
                else
                {
                    result = false;
                    break;
                }
            }
        }
        else
        {
            result = false;
            break;
        }
    }
    if (! result)
        std::cerr << "'Meta' test failed." << std::endl;

    return result;
}

#if defined MIDI66_SUPPORT_NOTE_SPECIFIC_EVENTSPECS

/*
 * We don't need this with the string_to_midi_event() function,
 * but let's just keep the "notes" version in our back pockets.
 *
 * The #define is in eventspecs.hpp.
 */

bool
note_test (const byte_test_values & t3)
{
    midi::event ev = midi::string_to_note(t3.inputline);
    long delta { long(ev.timestamp()) };
    int statusbyte { int(ev.status_byte()) };
    int d0 { int(ev.d0()) };
    int d1 { int(ev.d1()) };
    bool result { delta == t3.delta };
    if (result)
        result = statusbyte == t3.statusbyte;

    if (result)
        result = d0 == t3.d0;

    if (result)
        result = d1 == t3.d1;

    std::string outputline;
    if (result)
    {
        midi::byte b { midi::mask_status(ev.status_byte()) };
        midi::status s { midi::to_status(b) };
        outputline = midi::note_to_string(ev, s);
        result = outputline == t3.outputline;
    }
    if (s_verbose)
    {
        ++s_test_counter;
        std::cout
            << "Test #" << s_test_counter << " Values: " << t3.delta
            << ", " << t3.statusbyte << ", " << t3.d0 << ", " << t3.d1
            << " (hex: " << std::hex << t3.statusbyte
            << ", " << t3.d0 << ", " << t3.d1 << ")" << std::dec
            << std::endl
            ;
        std::cout
            << "Input line:   '" << t3.inputline << "'" << std::endl
            << "Expected out: '" << t3.outputline << "'" << std::endl
            << "Actual out:   '" << outputline << "'" << std::endl
            << "Success:       " << util::bool_to_string(result, true)
            << std::endl
            << std::endl
            ;
    }
    return result;
}

bool
note_tests ()
{
    bool result { false };
    for (const auto & test : s_event_values)
    {
        result = note_test(test);
        if (! result)
        {
            std::cerr << "'Notes' test failed." << std::endl;
            break;
        }
        if (s_test_counter == 5)
        {
            std::cout << "End of 'Notes' tests.\n" << std::endl;
            break;                      /* end of the "Note" events */
        }
    }
    return result;
}

#endif  // defined MIDI66_SUPPORT_NOTE_SPECIFIC_EVENTSPECS

bool
generic_test (const byte_test_values & t3)
{
    midi::event ev = midi::string_to_midi_event(t3.inputline);
    long delta { long(ev.timestamp()) };
    int statusbyte { int(ev.status_byte()) };
    int d0 { int(ev.d0()) };
    int d1 { int(ev.d1()) };
    bool result { delta == t3.delta };
    if (result)
        result = statusbyte == t3.statusbyte;

    if (result)
        result = d0 == t3.d0;

    if (result)
        result = d1 == t3.d1;

    std::string outputline;
    if (result)
    {
        outputline = midi::midi_event_to_string(ev);
        result = outputline == t3.outputline;
    }
    if (s_verbose)
    {
        ++s_test_counter;
        std::cout
            << "Test #" << s_test_counter << " Values: " << t3.delta
            << ", " << t3.statusbyte << ", " << t3.d0 << ", " << t3.d1
            << " (hex: " << std::hex << t3.statusbyte
            << ", " << t3.d0 << ", " << t3.d1 << ")" << std::dec
            << std::endl
            ;
        std::cout
            << "Input line:   '" << t3.inputline << "'" << std::endl
            << "Expected out: '" << t3.outputline << "'" << std::endl
            << "Actual out:   '" << outputline << "'" << std::endl
            << "Success:       " << util::bool_to_string(result, true)
            << std::endl
            << std::endl
            ;
    }
    return result;
}

bool
generic_tests ()
{
    bool result { false };
    for (const auto & test : s_event_values)
    {
        result = generic_test(test);
        if (! result)
        {
            std::cerr << "'Generic' test failed." << std::endl;
            break;
        }
    }
    return result;
}

#if defined SHOW_14_BITS

bool
show_14_bits (unsigned short bits14)
{
    midi::byte msb, lsb;
    bool result { midi::split_14_bits(bits14, msb, lsb) };
    if (result)
    {
        int MSB { int(msb) };
        int LSB { int(lsb) };
        std::cout
            << "14-bit value " << bits14 << " (0x"
            << std::hex << 8192 << "): "
            << "msb " << std::dec << MSB << " (0x"
            << std::hex << MSB << "); "
            << "lsb " << std::dec << LSB << " (0x"
            << std::hex << LSB << ") "
            << std::dec << std::endl
            ;
    }
    return result;
}

#endif

/**
 *  Simple test of converting a whole, but small, MIDI file from text
 *  and back.
 *
 *  [1] Sets ppqn = 384, bpm = 120, beats/bar = 4, and beat-width = 4.
 *      PPQN in the song_header structure is set.
 *
 *  [2] Calls midi::read_midi_file(). Makes a midi::file object, and
 *      calls parse(). This fills a bytevector and calls parse_smf_1().
 *
 *      Reads the header: magic ID, header length, format, track count,
 *      and PPQN (all saved in the song object).
 *
 *      Reads the track marker and the track length. Creates a track
 *      with the song as a "parent". Parses the track, and finalizes it.
 *      It is installed into the song.
 */

bool
to_text_test ()
{
    const std::string testfile { "tests/data/midi/smoke.mid" };
    midi::song s(testfile);                 /* [1] many default parameters  */
    std::string errmsg;
    bool result { s.read_midi_file(testfile, errmsg, false) };  /* see [2]  */
    if (result)
    {
        int smffmt, trackct, ppq;
        s.get_song_header(smffmt, trackct, ppq);

        std::string miditext
        {
            midi::midi_header_to_string(smffmt, trackct, ppq)
        };
        if (miditext.empty())
        {
            result = false;
        }
        else
        {
            miditext += "\n";

            const midi::tracklist & trklist { s.track_list() };
            const midi::tracklist::container trkcontainer { trklist.tracks() };
            if (util::investigate())
            {
                /*
                 * String versions of conversions.
                 */

                for (const auto & trk : trkcontainer)
                {
                    std::string trkstring { midi_track_to_string(*trk) };
                    miditext += trkstring;
                }
                if (s_verbose)
                {
                    std::cout
                        << "----- MIDI TEXT ------\n\n"
                        << miditext
                        << "----------------------"
                        << std::endl
                        ;
                }
            }
            else
            {
                /*
                 * Tokenizations versions of conversions.
                 */

                for (const auto & trk : trkcontainer)
                {
                    lib66::tokenization trklist { midi_track_to_lines(*trk) };
                    if (s_verbose)
                    {
                        std::cout << "----- MIDI TEXT ------\n\n";
                        for (const auto & line : trklist)
                        {
                            std::cout << line;
                        }
                        std::cout << "----------------------\n\n";
                    }
                }
            }
        }
    }
    return result;
}

/*
 * Steps:
 *
 *      -   Create a song object with a filename.
 *      -   Create a transcript object. It sets (again) the filename.
 *      -   Parse the transcript file to fill in the song from the file.
 *      -   Create another transcript to write the data from the song
 *          to another file.
 */

bool
transcript_test ()
{
    const std::string testfile { "tests/data/mftranscript/smoke.transcript" };
    const std::string textfile { "tests/data/mftranscript/smoke-2.transcript" };
    midi::song s(testfile);                 /* [1] many default parameters  */
    midi::transcript t(testfile, s);        /* default song gets file-spec  */
    bool result { t.parse("spectest") };
    if (result)
    {
        util::status_message("Success", testfile);
        if (s_verbose)
            s.print_tracks("Transcript");

        midi::transcript t2(textfile, s);
        result = t2.write();
    }
    else
    {
        util::error_message(t.error_message());
    }
    return result;
}

}           // namespace anonymous

/**
 *  The main routine. It first checks if a port was specified and queries
 *  the user if not. If valid, then the test files are opened and played.
 *
 *  rt_choose_port_number() in test_helpers opens temporary I/O ports to get
 *  data, and lets the user choose a port number.
 *
 *  We would like to support setting up a player, getting the port list from it,
 *  letting the user choose one, then launching the player.  TODO.
 */

int
main (int argc, char * argv [])
{
    int rcode { EXIT_FAILURE };
    cli::parser clip;
    std::string single_filename { /* rt_test_name() */ };
    bool success { clip.parse(argc, argv) };
    bool show_info { clip.show_information_only() };
    bool can_run { ! show_info };
    s_verbose = clip.verbose_request();
    if (success && can_run)
    {
#if defined MIDI66_SUPPORT_NOTE_SPECIFIC_EVENTSPECS
        success { note_tests() };
        if (success)
            success = header_tests();
#else
        success = header_tests();
#endif
        if (success)
            success = generic_tests();

        if (success)
            success = sysex_tests();

        if (success)
            success = meta_tests();

        if (success)
            success = to_text_test();

        if (success)
            success = transcript_test();

#if defined SHOW_14_BITS
        if (s_verbose)
        {
            unsigned short pitchcenter { 8192 };
            /* success = */ (void) show_14_bits(pitchcenter);

            midi::byte msb { 12 };          /* 12 semitones (2 octave)      */
            midi::byte lsb { 2 };           /*  2 cents for pitch-bend      */
            unsigned short bits14 { midi::join_14_bits(msb, lsb) };
            std::cout << "12:2 --> " << bits14 << std::endl;
            /* success = */ (void) show_14_bits(bits14);
        }
#endif
        if (success)
        {
            rcode = EXIT_SUCCESS;
            std::cout << "All spec-tests succeeded." << std::endl;
        }
        else
            std::cerr << "A spec-test failed." << std::endl;
    }
    else
    {
        if (clip.help_request())            /* also shows the --help        */
        {
            std::cout
    << "Note that not all basic options are supported by this application."
                << std::endl
                ;

        }
        if (clip.description_request())
        {
            std::cout
    << "This application tests the conversion, in both directions, between\n"
       "MIDI events and a brief text representation similar to midicvt and\n"
       "other midi-to-text converters. Add --verbose to see each test in\n"
       "more detail."
                << std::endl
                ;
        }
    }
    return rcode;
}

/*
 * specstest.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */
