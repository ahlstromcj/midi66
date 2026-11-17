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
 * \file          eventspecs.cpp
 *
 *  This module declares/defines functions that convert between MIDI
 *  events and abbreviation specifications for them, akin to those
 *  in the midicvt project.
 *
 * \library       midi66 application
 * \author        Chris Ahlstrom
 * \date          2026-10-20
 * \updates       2026-11-16
 * \license       GNU GPLv2 or above
 *
 * std::string status_abbreviations (int m) uses these abbreviations:
 *
 *  Coded Tested
 *               MThd  MIDI file header
 *    *     *    0x80  NoteOff
 *    *     *    0x90  NoteOn
 *    *     *    0xA0  Aftertouch
 *    *     *    0xB0  CtrlChange
 *    *     *    0xC0  ProgChange
 *    *     *    0xD0  ChPressure
 *    *     *    0xE0  PitchWheel
 *               0xF0  SysEx
 *    *     *    0xF1  QuarterFrame
 *    *     *    0xF2  SongPosition
 *    *     *    0xF3  SongSelect
 *    -     -    0xF4  SongF4
 *    -     -    0xF5  SongF5
 *    *     *    0xF6  TuneRequest
 *    *     *    0xF7  EOX
 *    *     *    0xF8  MIDIClock
 *    *     *    0xF9  TimingTick
 *    *     *    0xFA  ClkStart
 *    *     *    0xFB  ClkContinue
 *    *     *    0xFC  ClkStop
 *    -     -    0xFD  SongFD
 *    *     *    0xFE  ActiveSense
 *    *     *    0xFF  Meta (more testing to come)
 */

#include <cstdio>                       /* std::sscanf()                    */
#include <cctype>                       /* std::isxdigit()                  */

#include "cpp_types.hpp"                /* CSTR()                           */
#include "midi/calculations.hpp"        /* midi::join/split_14_bits()       */
#include "midi/editable_event.hpp"      /* midi::sysex_string, _bytes()     */
#include "midi/eventspecs.hpp"          /* extern declarations              */
#include "midi/patches.hpp"             /* program-change functions         */
#include "midi/song.hpp"                /* midi::song class                 */
#include "midi/track.hpp"               /* midi::track class                */
#include "util/strfunctions.hpp"        /* util::tokenize()                 */

namespace midi
{

/**
 *  MIDI File Header.
 *
 *  Note that the header is more explicit than the format used in
 *  midicvt.
 */

static const std::string s_midi_header_format
{
    "MThd smf=%d tracks=%d ppqn=%d"
};

/**
 *  Fill in the header values from a string.
 *
 *  We could use std::tuple<bool, int, int, int> as the return value,
 *  as in "return { result, smffmt, trackct, ppq}" and unpack using
 *  C++17 structured bindings: auto [ success, smf_format, etc.] =
 *  string_to_midi_header(line).
 */

bool
string_to_midi_header
(
    const std::string & line,
    int & smf_format,
    int & track_count,
    int & ppqn_value
)
{
    const char * source { CSTR(line) };
    int smffmt, trackct, ppq;
    int rc
    {
        std::sscanf
        (
            source, CSTR(s_midi_header_format), &smffmt, &trackct, &ppq
        )
    };
    bool result { rc == 3 };
    if (result)
    {
        result = smffmt == 0 || smffmt == 1 || smffmt == 2;
        if (result)
            result = trackct > 0 && ppqn_is_valid(ppq);

        if (result)
        {
            smf_format = smffmt;
            track_count = trackct;
            ppqn_value = ppq;
        }
    }
    return result;
}

std::string
midi_header_to_string
(
    int smffmt,
    int trackct,
    int ppq
)
{
    char tmp [64];
    (void) snprintf
    (
        tmp, sizeof tmp, CSTR(s_midi_header_format), smffmt, trackct, ppq
    );
    return std::string(tmp);
}

/**
 *  We want to be able to look up sprintf() format string.
 *
 *  Compare to "static const midi_name_map s_status_abbreviations"
 *  in the eventcodes module. Somewhat duplicative.
 */

/**
 *  An internal map to support looking up the event-specification
 *  formats.
 */

using midi_format_map = std::map<int, std::string>;

/**
 *  These formats are both sscanf() formats and snprintf() formats.
 *  If desired and "#" comment can be written after the item
 *  to show, for example, note values.
 */

static const midi_format_map s_status_formats
{
    /*
     * Channel Mode Messages
     */

    { 0x80, "%ld NoteOff chan=%d note=%d vel=%d"        },
    { 0x90, "%ld NoteOn chan=%d note=%d vel=%d"         },
    { 0xA0, "%ld Aftertouch chan=%d note=%d vel=%d"     },
    { 0xB0, "%ld CtrlChange chan=%d ctrl=%d level=%d"   },
    { 0xC0, "%ld ProgChange chan=%d prog=%d"            },
    { 0xD0, "%ld ChPressure chan=%d level=%d"           },
    { 0xE0, "%ld PitchWheel chan=%d bend=%d"            }, /* -8192 to 8192 */

    /*
     * System Exclusive Message
     */

    { 0xF0, "%ld SysEx bytes=%s"                        },

    /*
     * System Common Messages
     */

    { 0xF1, "%ld QuarterFrame hmsf=%X"                  },
    { 0xF2, "%ld SongPosition beats=%d"                 }, /* 14-bit value  */
    { 0xF3, "%ld SongSelect song=%d"                    }, /* 7-bit value   */
    { 0xF4, "SongF4"                                    }, /* undefined     */
    { 0xF5, "SongF5"                                    }, /* undefined     */
    { 0xF6, "%ld TuneRequest"                           }, /* TuneRequest   */
    { 0xF7, "%ld EOX"                                   }, /* End of SysEx  */

    /*
     * System Realtime Messages
     */

    { 0xF8, "%ld MIDIClock"                             },
    { 0xF9, "%ld TimingTick"                            },
    { 0xFA, "%ld ClkStart"                              },
    { 0xFB, "%ld ClkContinue"                           },
    { 0xFC, "%ld ClkStop"                               },
    { 0xFD, "SongFD"                                    }, /* undefined     */
    { 0xFE, "%ld ActiveSense"                           },
    { 0xFF, "%ld Meta %s"                               }
};

std::string
status_format (int s)
{
    std::string result;
    const auto iter { s_status_formats.find(s) };
    if (iter != s_status_formats.end())
        result = iter->second;

    return result;
}

std::string
status_format (const midi::event & ev)
{
    midi::byte b { ev.status_byte() };
    if (ev.has_channel())
        b = mask_status(b);

    return status_format(int(b));
}

std::string
status_format (midi::status s)
{
    return status_format(int(midi::to_byte(s)));
}

/**
 *  Given a legal MIDI note, returns the octave of the note,
 *  ranging from -2 to 8 for the "C3 == middle C" convention.
 *
 *  See extras/notes/midi-note-chart.text for more discussion
 *  and a table of notes.
 *
 * \param note
 *      Provides the input MIDI note, which must range from 0 to
 *      127.
 *
 * \param use_c3
 *      If true (the default is false), then use the C3 convention.
 *      Otherwise use the C4 convention.
 *
 * \return
 *      Returns the C3-based octave.
 */

int
octave_of_note (int note, bool use_c3)
{
    int result { -99 };
    if (note >= 0 && note <= c_note_max)
        result = (note / c_octave_size) - (use_c3 ? 2 : 1);

    return result;
}

/**
 *    Converts a note number to a generic note name.
 *
 * \param note
 *    Provides the MIDI note value to be written.
 *
 * \param use_c3
 *      If true (the default is false), then use the C3 convention.
 *      Otherwise use the C4 convention.
 *
 * \return
 *    Returns the note name, such as "C4", "C#5"....
 */

std::string
pitch_note (int note, bool use_c3)
{
    static const char * s_note_names []
    {
        "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
    };
    static char buf [8];
    int octave { octave_of_note(note, use_c3) };
    if (octave != (-99))
    {
        (void) snprintf
        (
            buf, sizeof buf, "%s%d",
            s_note_names[note % c_octave_size], octave
        );
        return std::string(buf);
    }
    else
        return std::string("X");
}

#if defined MIDI66_SUPPORT_NOTE_SPECIFIC_EVENTSPECS

/**
 *  Conversion between notes and abbreviated string descriptions.
 *  Applies to note on, note off, and aftertouch, as they all have
 *  the same parameters (note number and velocity).
 */

std::string
note_to_string (const midi::event & ev, midi::status mstat)
{
    int index { int(to_byte(mstat)) };
    std::string fmt { status_format(index) };
    long delta { long(ev.timestamp()) };
    std::string abbr { status_abbreviation(index) };
    int channel { int(ev.channel()) };
    int note { int(ev.get_note()) };
    int velocity { int(ev.note_velocity()) };
    std::string notename { pitch_note(note) };
    char tmp [64];
    (void) snprintf
    (
        tmp, sizeof tmp, CSTR(fmt),
        delta, /* CSTR(abbr), */
        channel, note, velocity /* , CSTR(notename) */
    );

    /*
     * Do we want to support a comment naming the note value?
     * But see midi_event_to_string().
     */

    std::string result { std::string(tmp) };
    result += " # ";
    result += notename;
    return result;
}

std::string
note_off_to_string (const midi::event & ev)
{
    return note_to_string(ev, midi::status::note_off);
}

std::string
note_on_to_string (const midi::event & ev)
{
    return note_to_string(ev, midi::status::note_on);
}

std::string
aftertouch_to_string (const midi::event & ev)
{
    return note_to_string(ev, midi::status::aftertouch);
}

/*
 * Function type spec_translator.
 */

midi::event
string_to_note (const std::string & line)
{
    static const char * fmt { "%ld %s chan=%d note=%d vel=%d" };

    const char * source { CSTR(line) };
    long delta;
    char abbr [16];
    int channel;
    int note;
    int velocity;
    int rc
    {
        std::sscanf(source, fmt, &delta, abbr, &channel, &note, &velocity)
    };
    if (rc == 5)
    {
        std::string a { abbr };
        midi::status s { status_midi(a) };
        if (s != midi::status::erroneous)
        {
            midi::event result = midi::event
            (
                delta, s, channel, note, velocity
            );
            return result;
        }
        else
        {
            midi::event ev;
            return ev;
        }

    }
    else
    {
        midi::event ev;
        return ev;
    }
}

#endif  // defined MIDI66_SUPPORT_NOTE_SPECIFIC_EVENTSPECS

/**
 *  Given a line like "192 SysEx 0xF0 0x7E 0x7F 0x06 0x01 0xF7", where the
 *  first value is the event delta, and the rest is the whole SysEx message,
 *  this function extracts the values.
 *
 *  This function is used in lieu of sscanf().
 *
 *  It's a little wasteful. Does that matter?
 */

midi::pulse
extract_sysex
(
    const std::string & line,
    midi::bytes & bytedest
)
{
    midi::pulse result { -1 };
    lib66::tokenization tokens { util::tokenize(line) };
    std::size_t counter { 0 };
    bytedest.clear();
    for (const auto & t : tokens)
    {
        if (counter == 0)
        {
            long ts { util::string_to_long(t) };
            result = midi::pulse(ts);
        }
        else if (counter == 1)
        {
            if (t != "SysEx")
            {
                result = (-1);
                break;
            }
        }
        else
        {
            unsigned ubyte { util::string_to_unsigned(t, 0, 16) };
            midi::byte b { midi::byte(ubyte) };
            bytedest.push_back(b);
        }
        ++counter;
    }
    return result;
}

/**
 *  Handles the alternate format "0 Meta TimeSig..."
 */

/**
 *  This function first gets the name of the event, which is always
 *  the second token in the input line. Then it figures out the
 *  midi::status value and the format to be used.
 */

midi::event
string_to_midi_event (const std::string & line)
{
    lib66::tokenization tokens { util::tokenize(line) };
    if (tokens.size() >= 2)
    {
        std::string eventnam { tokens[1] };
        midi::status sevent { status_midi(eventnam) };
        midi::byte bevent { midi::to_byte(sevent) };
        std::string fmt { status_format(sevent) };
        int rc;                         /* provides the result of sscanf()  */
        long delta;                     /* holds the timestamp delta        */
        int p;                          /* channel or other first parameter */
        int p0;                         /* note number, control number, etc */
        int p1;                         /* note velocity, level, etc        */
        midi::bytes byts;               /* for SysEx and Meta events        */
        switch (sevent)
        {

        /*
         * 3-byte events. Channel, key or control, and velocity or level.
         */

        case midi::status::note_off:
        case midi::status::note_on:
        case midi::status::aftertouch:
        case midi::status::control_change:

            rc = std::sscanf(CSTR(line), CSTR(fmt), &delta, &p, &p0, &p1);
            if (rc == 4)
            {
                return midi::event
                (
                    midi::pulse(delta), sevent,
                    midi::byte(p), midi::byte(p0), midi::byte(p1)
                );
            }
            else
                return midi::event();
            break;

        /*
         * 2-byte events. Channel and patch or pressure level.
         * For now, we treat Quarter Frame simply, as a single
         * parameter in hex format.
         */

        case midi::status::program_change:
        case midi::status::channel_pressure:

            rc = std::sscanf(CSTR(line), CSTR(fmt), &delta, &p, &p0);
            if (rc == 3)
            {
                /*
                 * TO DO: tweak the event class so 2-and-3-byte
                 * functions match up.
                 */

                return midi::event
                (
                    midi::pulse(delta), sevent,
                    midi::byte(p), midi::byte(p0)
                );
            }
            else
                return midi::event();
            break;

        case midi::status::pitch_wheel:

            /*
             * Here, p0 is the bend of the pitchbend using a range from
             * -8192 to 0 to 8192, and p1 is not used.
             * Note the reversal of parameters.
             */

            rc = std::sscanf(CSTR(line), CSTR(fmt), &delta, &p, &p0);
            if (rc == 3)
            {
                p0 += 8192;             /* change range to 0 to 16383       */
                if (p0 < 0)
                    p0 = 0;
                else if (p0 > 16383)
                    p0 = 16383;

                midi::byte msb, lsb;
                unsigned short usp0 { (unsigned short)(p0) };
                (void) split_14_bits(usp0, msb, lsb);
                return midi::event
                (
                    midi::pulse(delta), sevent,
                    midi::byte(p), lsb, msb
                );
            }
            else
                return midi::event();
            break;


            break;

        /*
         * System Exclusive.
         *
         * midi::status::real_time was the same value (0xF0), but now
         * has been removed.
         */

        case midi::status::sysex:

            delta = extract_sysex(line, byts);
            if (delta >= 0)
                return midi::event(midi::pulse(delta), byts);
            else
                return midi::event();
            break;

        /*
         * System Common Messages.
         *
         * For now, we treat Quarter Frame simply, as a single value:
         * "0xF1 p".
         */

        case midi::status::quarter_frame:

            rc = std::sscanf(CSTR(line), CSTR(fmt), &delta, &p0);
            if (rc == 2)
                return midi::event(midi::pulse(delta), bevent, midi::byte(p0));
            else
                return midi::event();
            break;

            break;

        case midi::status::song_pos:

            rc = std::sscanf(CSTR(line), CSTR(fmt), &delta, &p0);
            if (rc == 2)
            {
                midi::byte msb, lsb;
                unsigned short usp0 { (unsigned short)(p0) };
                if (usp0 > 16383)
                    usp0 = 16383;

                (void) split_14_bits(usp0, msb, lsb);
                return midi::event(midi::pulse(delta), bevent, lsb, msb);
            }
            else
                return midi::event();
            break;

        case midi::status::song_select:

            rc = std::sscanf(CSTR(line), CSTR(fmt), &delta, &p0);
            if (rc == 2)
            {
                return midi::event(midi::pulse(delta), bevent, p0);
            }
            else
                return midi::event();
            break;

        case midi::status::song_F4:
        case midi::status::song_F5:

            break;

        /*
         * System Common/Realtime 1-byte Messages
         */

        case midi::status::tune_request:
        case midi::status::sysex_continue:  // midi::status::sysex_end, EOX
        case midi::status::clk_clock:
        case midi::status::timing_tick:
        case midi::status::clk_start:
        case midi::status::clk_continue:
        case midi::status::clk_stop:
        case midi::status::active_sense:

            rc = std::sscanf(CSTR(line), CSTR(fmt), &delta);
            if (rc == 1)
            {
                midi::message msg { midi::message(double(delta)) };
                msg.push(bevent);
                return midi::event(msg);
            }
            else
                return midi::event();
            break;

        case midi::status::song_FD:

            break;

        case midi::status::meta_msg:

            delta = extract_meta(line, byts);
            if (delta >= 0 && byts.size() > 2)  /* need at least 3 bytes    */
            {
                midi::meta metatype { to_meta(byts[1]) };
                return midi::event(midi::pulse(delta), metatype, byts);
            }
            else
                return midi::event();
            break;

        case midi::status::erroneous:

            break;
        }
    }
    return midi::event();
}

std::string
midi_event_to_string (const midi::event & ev)
{
    std::string result;
    std::string fmt { status_format(ev) };
    long delta { long(ev.timestamp()) };
    if (ev.is_two_bytes())              /* channel and two data values      */
    {
        int channel { ev.channel() };
        midi::byte d0 { ev.d0() };
        midi::byte d1 { ev.d1() };
        char tmp [80];
        if (ev.is_pitchbend())
        {
            unsigned short ubend { midi::join_14_bits(d1, d0) };
            int bend { ubend - 8192 };
            if (bend < -8192)
                bend = -8192;

            (void) snprintf
            (
                tmp, sizeof tmp, CSTR(fmt), delta, channel, bend
            );
            result = std::string(tmp);
        }
        else
        {
            int i0 { int(d0) };
            int i1 { int(d1) };
            (void) snprintf
            (
                tmp, sizeof tmp, CSTR(fmt), delta, channel, i0, i1
            );
            result = std::string(tmp);
        }
        result = std::string(tmp);
    }
    else if (ev.is_one_byte())          /* channel and one data value       */
    {
        int channel { ev.channel() };
        int d0 { ev.d0() };
        char tmp [64];
        (void) snprintf(tmp, sizeof tmp, CSTR(fmt), delta, channel, d0);
        result = std::string(tmp);
    }
    else
    {
        std::size_t count { ev.message_size() };
        if (ev.is_sysex())
        {
            const midi::bytes & byts { ev.get_message().event_bytes() };
            std::string ts { std::to_string(ev.timestamp()) };
            std::string text { sysex_string(byts) };
            result = ts + " SysEx " + text;

            /*
             * Now add a "#" comment further clarifying the event.
             *
             * F0 <ID number> <device ID> <sub-ID#1> <sub-ID#2> . . . F7
             */

            if (byts.size() >= 6)
            {
                std::string id_number_string;
                midi::byte id { byts[1] };
                if (id == 0x7D)
                    id_number_string = "Non-commercial";
                else if (id == 0x7E)
                    id_number_string = "Non-realtime";
                else if (id == 0x7F)
                    id_number_string = "Realtime";
                else
                    id_number_string = "Manufacturer";

                /*
                 * midi::byte devid { byts[2] };
                 * midi::byte subid { byts[3] };
                 */

                result += " # " + id_number_string;
            }
        }
        else if (ev.is_meta())
        {
            result = meta_to_string(ev);
        }
        else if (count == 3)            /* e.g. Song Position               */
        {
            midi::byte d0 { ev.d0() };
            midi::byte d1 { ev.d1() };
            int position { int(midi::join_14_bits(d1, d0)) };
            char tmp [48];
            (void) snprintf(tmp, sizeof tmp, CSTR(fmt), delta, position);
            result = std::string(tmp);
        }
        else if (count == 2)            /* e.g. Quarter Frame               */
        {
            int hmsf { int(ev.get_byte(1)) };
            char tmp [32];
            (void) snprintf(tmp, sizeof tmp, CSTR(fmt), delta, hmsf);
            result = std::string(tmp);
        }
        else if (count == 1)            /* e.g. Tune Request                */
        {
            char tmp [32];
            (void) snprintf(tmp, sizeof tmp, CSTR(fmt), delta);
            result = std::string(tmp);
        }
    }

    /*
     * Now add a "#" comment further clarifying the event.
     */

    if (ev.is_note())
    {
        int note { int(ev.get_note()) };
        std::string notename { midi::pitch_note(note) };
        result += " # ";
        result += notename;
    }
    else if (ev.is_controller())
    {
        std::string ctrlname { midi::controller_abbreviation(int(ev.d0())) };
        result += " # ";
        result += ctrlname;
    }
    else if (ev.is_program_change())
    {
        std::string progname { midi::program_name(int(ev.d0()), false) };
        result += " # ";
        result += progname;
    }
    return result;
}

/**
 *  MTrk format.
 */

static std::string s_mtrk_format { "MTrk bytes=%d" };

/**
 *  This functions takes vector of strings, each of which are a line
 *  from a text file as read in by util::file_read_lines() from the cfg66
 *  library's filefunctions module.
 *
 *  It treats each line as part of the definition of a MIDI track, and
 *  returns a unique pointer to the new track.
 *
 *  TODO:
 *
 *      -   Provide an offset so that parsing starts at the next "MTrk"
 *          marker in the data.
 *
 *          - OR -
 *
 *      -   Provide a midi::textfile class to work somewhat like the
 *          midi::file class.
 */

std::unique_ptr<midi::track>
lines_to_midi_track (song * s, const lib66::tokenization & lines)
{
    bool ok { lines.size() >= 3};       /* MTrk, 1 event, and EndOfTrack    */
    if (ok)
    {
        lib66::tokenization tokens { util::tokenize(lines[0]) };
        ok = tokens[0] == "MTrk";
    }

    std::unique_ptr<midi::track> result { new (std::nothrow) midi::track(s) };
    if (ok && bool(result))
    {
        for (const auto & evstr : lines)
        {
            midi::event ev = string_to_midi_event(evstr);
            if (ev.is_valid())
                result->add_event(ev);
            else
                break;
        }
    }
    return result;
}

/**
 *
 *  This comes up 0 after parsing: int trksize { int(trkdata.size()) }.
 *  It is the size of the trackdata bytevector.
 *
 */

std::string
midi_track_to_string (const midi::track & trk)
{
    const trackdata & trkdata { trk.data() };
    int trksize { int(trkdata.messages_size()) };       /* number of bytes  */
    char tmp [32];
    (void) snprintf(tmp, sizeof tmp, CSTR(s_mtrk_format), trksize);
    std::string result { std::string(tmp) + "\n" };
    midi::eventlist evlist { trkdata.events() };
    for (const auto & ev : evlist)
    {
        std::string line { midi_event_to_string(ev) + "\n"};
        result += line;
    }
    return result;
}

lib66::tokenization
midi_track_to_lines (const midi::track & trk)
{
    lib66::tokenization result;
    const trackdata & trkdata { trk.data() };
    int trksize { int(trkdata.messages_size()) };       /* number of bytes  */
    char tmp [32];
    (void) snprintf(tmp, sizeof tmp, CSTR(s_mtrk_format), trksize);
    std::string firstline { std::string(tmp) + "\n" };
    result.push_back(firstline);

    midi::eventlist evlist { trkdata.events() };
    for (const auto & ev : evlist)
    {
        std::string line { midi_event_to_string(ev) + "\n"};
        result.push_back(line);
    }
    return result;
}

#if defined USE_EXTENDED_META_SPECS

/**
 * For the future, we want more readable Meta lines, where the Meta
 * name is part of the data, rather than a comment.
 *
 * The integer key here is the Meta type number.
 *
 * SeqSpec is 0xFF 0x7F len data. The data can be:
 *
 *      -   Manufacturer ID.
 *          -   1 byte      -or-
 *          -   3 bytes (0x00 plus two bytes)
 *      -   Data.
 *
 * Seq66's SeqSpec is based on that of the old Seq24 program:
 *
 *      -   0xFF 7F len 24 24 00 cc ...
 *
 *              where cc determines what data follows
 */

static const midi_format_map s_alt_meta_formats
{
    /* No. Delta 0xFF  No.     Data */

    { 0x00, "%ld Meta SeqNumber %d"     },  /* ss ss (len == 2)         */
    { 0x01, "%ld Meta TextEvent %s"     },  /* len text                 */
    { 0x02, "%ld Meta Copyright %s"     },  /* len text                 */
    { 0x03, "%ld Meta TrackName %s"     },  /* len text                 */
    { 0x04, "%ld Meta Instrument %s"    },  /* len text                 */
    { 0x05, "%ld Meta Lyric %s"         },  /* len text                 */
    { 0x06, "%ld Meta Marker %s"        },  /* len text                 */
    { 0x07, "%ld Meta CuePoint %s"      },  /* len text                 */
    { 0x08, "%ld Meta ProgramName %s"   },  /* len text                 */
    { 0x09, "%ld Meta PortName %s"      },  /* len text                 */
    { 0x20, "%ld Meta MidiChannel %d"   },  /* cc obsolete              */
    { 0x21, "%ld Meta MidiPort %d"      },  /* pp obsolete,             */
    { 0x2F, "%ld Meta EndOfTrack"       },  /* len = 0                  */
    { 0x51, "%ld Meta SetTempo usec=%d" },  /* tt tt tt (len = 3)       */
    {
        0x54,
        "%ld Meta SmpteOffset hr=%d mn=%d se=%d fr=%d ff=%d"    /* len = 5  */
    },
    {
        /*
         * nn = numerator; dd = denominator; cc = MIDI clocks per metronome
         * click (24 clock/qn); bb = 32nd notes/qn
         */

        0x58,
        "%d Meta TimeSig nn=%d dd=%d cc=%d bb=%d"               /* len = 4  */
    },
    {
        0x59,                                   /* -7-to-7 maj/min len == 2 */
        "%d Meta KeySig accidental=%d mode=%d"
    },
    { 0x7F, "%d Meta SeqSpec %s"            }   /* data (len is variable)   */
};

std::string
meta_format_ex (int metatype)
{
    std::string result;
    const auto iter { s_alt_meta_formats.find(metatype) };
    if (iter != s_alt_meta_formats.end())
        result = iter->second;

    return result;
}

std::string
meta_name (int metatype)
{
    std::string fmt { meta_format_ex(metatype) };
    if (! fmt.empty())
    {
        lib66::tokenization fmttokens { util::tokenize(fmt) };
        result =  fmttokens[2];
    }
    return result;
}

int
meta_type (const std::string & metaname)
{
    int result { -1 };
    for (const auto & mf : s_alt_meta_formats)
    {
        lib66::tokenization fmttokens { util::tokenize(mf.second) };
        const std::string & third { fmttokens[2] };
        if (metaname == third)
        {
            result = mf.first;
            break;
        }
    }
    return result;
}

/**
 *  See the code used in midi::event string_to_midi_event(), where
 *  we choose to use extract_meta_ex() versus extract_meta().
 *  The point is we need 0xFF <metatype> data.
 *
 *  In these meta-extraction functions, it is assumed the meta
 *  type number has been obtained, and that bytedest has been
 *  cleared.
 */

midi::pulse
extract_seq_number
(
    int mtype,
    const std::string & line,
    midi::bytes & bytedest
)
{
    midi::pulse result { -1 };
    std::string fmt { meta_format_ex(mtype) };  /* copying a short string   */
    if (! fmt.empty())
    {
        long delta;
        int seqno;
        int rc { sscantf(fmt, &delta, &seqno) };
        if (rc == 2)
        {
            midi::byte mb { midi::to_byte(mtype) };
            bytedest.push_back(0xFF);
            bytedest.push_back(mb);
            bytedest.push_back(2);                      /* len    */
            bytedest.push_back((seqno & 0xff00) >> 8);  /* ss lsb */
            bytedest.push_back((seqno & 0x00ff);        /* ss msb */
        }
    }
    return result;
}

midi::pulse
extract_meta_ex (const std::string & line, midi::bytes & bytedest)
{
    midi::pulse result { -1 };
    lib66::tokenization tokens { util::tokenize(line) };
    std::size_t counter { 0 };
    bytedest.clear();
    if (tokens.size() < 3)
        return result;

    for (const auto & t : tokens)
    {
        if (counter == 0)
        {
            long ts { util::string_to_long(t) };
            result = midi::pulse(ts);
        }
        else if (counter == 1)
        {
            if (t != "Meta")
            {
                result = (-1);
                break;
            }
        }
        else if (counter == 2)
        {
            int mtype { meta_type(t) };
//          int rc { -1 };
            if (mtype >= 0)
            {
                midi::meta m { midi::to_meta(midi::byte(mtype)) };
                switch (m)
                {
                case seq_number:

                    result = extract_seq_number(mtype, line, bytedest);
                    break;

                case text_event:

                    break;

                case copyright:

                    break;

                case track_name:

                    break;

                case instrument:

                    break;

                case lyric:

                    break;

                case marker:

                    break;

                case cue_point:

                    break;

                case program_name:

                    break;

                case port_name:

                    break;

                case midi_channel:

                    break;

                case midi_port:

                    break;

                case end_of_track:

                    break;

                case set_tempo:

                    break;

                case smpte_offset:

                    break;

                case time_signature:

                    break;

                case key_signature:

                    break;

                case seq_spec:

                    break;
        }
        else
        {
#if 0
            unsigned ubyte { util::string_to_unsigned(t, 0, 16) };
            midi::byte b { midi::byte(ubyte) };
            bytedest.push_back(b);
#else
#endif
        }
        ++counter;
    }
    return result;
}

std::string
meta_to_string_ex (const midi::event & ev)
{
    const midi::bytes & byts { ev.get_message().event_bytes() };
    std::string ts { std::to_string(ev.timestamp()) };
    std::string text { sysex_string(byts) };    /* also does meta   */
    std::string result { ts + " Meta " + text };

    /*
     * Now add a "#" comment further clarifying the event.
     *
     * FF <meta type> len data...
     */

    if (byts.size() >= 2)
    {
        midi::byte id { byts[1] };
        std::string id_number_string { meta_label(id) };
        result += " # " + id_number_string;
    }
    return result;
}


#else       // defined USE_EXTENDED_META_SPECS

/**
 *  Extracts data from lines of the form "0 Meta 0xFF ... # comments".
 */

midi::pulse
extract_meta
(
    const std::string & line,
    midi::bytes & bytedest
)
{
    midi::pulse result { -1 };
    lib66::tokenization tokens { util::tokenize(line) };
    std::size_t counter { 0 };
    bytedest.clear();
    if (tokens.size() < 3)
        return result;

    for (const auto & t : tokens)
    {
        /*
         * Avoid handling non-numbers after the first two tokens.
         */

        if (counter > 1 && ! std::isxdigit(t[0]))
            break;

        if (counter == 0)
        {
            long ts { util::string_to_long(t) };
            result = midi::pulse(ts);
        }
        else if (counter == 1)
        {
            if (t != "Meta")
            {
                result = (-1);
                break;
            }
        }
        else
        {
            unsigned ubyte { util::string_to_unsigned(t, 0, 16) };
            midi::byte b { midi::byte(ubyte) };
            bytedest.push_back(b);
        }
        ++counter;
    }
    return result;
}

std::string
meta_to_string (const midi::event & ev)
{
    const midi::bytes & byts { ev.get_message().event_bytes() };
    std::string ts { std::to_string(ev.timestamp()) };
    std::string text { sysex_string(byts) };    /* also does meta   */
    std::string result { ts + " Meta " + text };

    /*
     * Now add a "#" comment further clarifying the event.
     *
     * FF <meta type> len data...
     */

    if (byts.size() >= 2)
    {
        midi::byte id { byts[1] };
        std::string id_number_string { meta_label(id) };
        result += " # " + id_number_string;
    }
    return result;
}

#endif      // defined USE_EXTENDED_META_SPECS

}           // namespace midi

/*
 * eventspecs.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */
