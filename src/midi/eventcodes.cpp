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
 * \file          eventcodes.cpp
 *
 *  This module declares/defines the values for the MIDI status codes (also
 *  known as the event codes).
 *
 * \library       midi66 application
 * \author        Chris Ahlstrom
 * \date          2015-07-24
 * \updates       2026-11-02
 * \license       GNU GPLv2 or above
 *
 *  This module also declares/defines the various constants, status-byte
 *  values, or data values for MIDI events.  These values were extracted from
 *  Seq66's event.hpp header file in order to be useful beyond that
 *  application.  In addition, the values were changed to enum class values
 *  for better partitioning.
 *
 *  The MIDI protocol consists of MIDI events that carry four types of messages:
 *
 *      -   Voice messages.  0x80 to 0xEF; includes channel information.
 *      -   System common messages.  0xF0 (SysEx) to 0xF7 (End of SysEx)
 *      -   System realtime messages. 0xF8 to 0xFF.
 *      -   Meta messages. 0xFF is the flag, followed by type, length, and data.
 *
 *  Control Change Messages:
 *
 *      This enumeration summarizes the MIDI Continuous Controllers (CC) that
 *      are available.
 *
 * Notes:
 *
 *  For balance and pan: 0 = hard left, 64 = center, 127 = hard right
 *
 *  For all on/off switches, 0 to 63 = Off and 64 to 127 = On.
 *
 *  Damper Pedal 64 versus Sostenuto CC 66:
 *
 *      Sustain is a pedal on/off switch that controls sustain.  Sostenuto is
 *      an on/off switch like the Sustain controller (CC 64), but it only holds
 *      notes that were on when the pedal was pressed.  People use it to “hold”
 *      chords” and play melodies over the held chord.
 *
 *  NRPN 98 and 99: Non-Registered Parameter Number LSB and MSB. For
 *  controllers 6, 38, 96, and 97, it selects the NRPN parameter.
 *
 *  RPN 100 and 101: Registered Parameter Number LSB and MSB. For controllers
 *  6, 38, 96, and 97, it selects the RPN parameter.
 *
 *  Local On/Off Switch 122: Turns the internal connection of a MIDI
 *  keyboard/workstation, etc., On or Off.  For a computer, one will most
 *  likely want Local Control off to avoid notes being played twice, once
 *  locally and twice when the note is sent back from the computer to your
 *  keyboard.
 *
 *  All Notes Off 123: Mutes all sounding notes. Release time will be
 *  maintained, and notes held by sustain will not turn off until sustain
 *  pedal is depressed.
 *
 *  Undefined values summary: 3, 9, 14-15, 20-31, 85-90, and 102-119.
 *
 *  Values 32 to 63 are for Controllers 0 to 31, the Least Significant Bit
 *  (LSB).
 *
 *  Copped from seq66/libseq66/include/midi/controllers.hpp.
 *
 *  But consider this, events other than System Exclusive, which do not have
 *  an arbitrary number of bytes, but a definite number. These events are:
 *
\verbatim
        -   Sequence No.:   FF 00 02 s1 s1
        -   MIDI Channel:   FF 20 01 cc
        -   MIDI Port:      FF 21 01 pp
        -   Set Tempo:      FF 51 03 tt tt tt
        -   SMPTE Offset:   FF 54 05 hh mm ss fr ff
        -   Time Signature: FF 58 04 nn dd cc bb
        -   Key Signature:  FF 59 02 sf mi
\endverbatim
 *
 *  The arbitrarily-sized Meta events are:
 *
\verbatim
        -   Text:           FF 01 len text
        -   Copyright:      FF 02 len text
        -   Track Name:     FF 03 len name
        -   Instrument:     FF 04 len name
        -   Marker:         FF 05 len text
        -   Cue Point:      FF 06 len text
        -   Seq. Specific:  FF 7F len data
\endverbatim
 *
 *  The maximum amount of constant-size data is 5 bytes.  We should aim to
 *  increase this and use it, using event::m_status as the "meta" byte and
 *  perhaps m_channel as the "meta-event" byte.  But curently, the tempo and
 *  time signature events are stored as data in the sequence object, so
 *  that's probably the best tact for the future.
 *
 *  Channel Voice Messages:
 *
 *      The MIDI events with status from 0x80 to 0xEF are channel messages.
 *      The comments in eventcodes.hpp represent the one or two data-bytes
 *      of the message.
 *
 *      Note that Channel Mode Messages use the same code as the Control Change,
 *      but uses reserved controller numbers ranging from 120 to 127.
 *
 *      -   All sound off: c = 120, v = 0
 *      -   Reset all controllers: c = 121, v = 0 (usually)
 *      -   Local control: c = 122, v = 0 (Off), v = 127 (On)
 *      -   All Notes off:
 *          -   All off: c = 123, v = 0
 *          -   Omni mode off: c = 124, v = 0
 *          -   Omni mode on: c = 125, v = 0
 *          -   Mono mode on (poly off): c = 126, v = 0 (Omni on) or M (channels)
 *          -   Poly mode on: c = 127, v = 0
 *
 *      The "any" (0x00) value may prove to be useful in allowing any event to be
 *      dealt with.  Not sure yet, but the cost is minimal.
 *
 *  System Messages:
 *
 *      The following MIDI events have no channel.  We have included redundant
 *      constant variables for the SysEx Start and End bytes just to make it
 *      clear that they are part of this sequence of values, though usually
 *      treated separately.
 *
 *      Only the following constants are followed by some data bytes:
 *
 *      -   sysex           = 0xF0  // ends with 0xF7
 *      -   quarter_frame   = 0xF1  // and 0x0n to 0x7n
 *      -   song_pos        = 0xF2  // and 0x0 to 0x3FFF 16th note
 *      -   song_select     = 0xF3  // and 0x0 to 0x7F song number
 *      -   tune_select     = 0xF6  // no data, tune yourself
 *
 *      A MIDI System Exclusive (SYSEX) message starts with F0, followed by the
 *      manufacturer ID (how many? bytes), a number of data bytes, and ended by
 *      an F7.
 *
 * System Exclusive:
 *
 *      F0H <ID number> <device ID> <sub-ID#1> <sub-ID#2> . . . F7H
 *
 *      Here we only note that there are non-manufacture IDs:
 *
 *      -   7DH. Non Commercial System Exclusive ID
 *      -   7EH. Non-Real Time System Exclusive. The next byte selects
 *          a "universal" message, such as ACK, NAK, CANCEL, WAIT, EOF,
 *          MIDI Time Code, and File Dump; there are about 15 such messages.
 *          An important message is the "device inquiry" or "MIDI ping":
 *          "F0 7E <devID> 06 01 F7". A device ID of 0x7F indicates all
 *          devices should respond. Another message is the "reset":
 *          "F0 7E <devID> 09 01 F7".
 *      -   7FH. Real Time System Exclusive. Has 9 "universal" messages.
 *
 * MIDI System Real-Time messages:
 *
 *      -   https://en.wikipedia.org/wiki/MIDI_beat_clock
 *      -   http://www.midi.org/techspecs/midimessages.php
 *
 *      Real-time message are used for synchronization of all clock-based units
 *      in a system.  They contain only a status byte, no data bytes.
 *
 * Meta messages:
 *
 *      0xFF is a MIDI "escape code" used in MIDI files to introduce a MIDI meta
 *      event.  Note that it has the same code (0xFF) as the Reset message, but
 *      the Meta message is read from a MIDI file, while the Reset message is
 *      sent to the sequencer by other MIDI participants.
 *
 * Quarter Frame message (0xF1 nnnddd):
 *
 *      This message is timing information in the hours:minutes:seconds:frames
 *      format (similar to SMPTE) that is used to synchronize MIDI devices.
 *      It is a MIDI System Common message for MIDI Time Code.
 *
 *      It consists of two bytes of data, the status, 0xF1, and a piece of the
 *      MIDI time code as described below.
 *
 *      This byte cannot carry the full time, so several quarter frame messages
 *      must be sent to piece together the current MIDI time.
 *
 *      The second byte of the quarter frame message carries two pieces of
 *      information: The high nybble of the byte (bits 4 through 7) carry a
 *      flag that denotes which piece of the MIDI time code this byte is all
 *      about (the frames, seconds, minutes, or hours).
 *
 *      The low nibble of the byte (bits 0 through 3) carries a piece of the
 *      actual time code. Thus, this byte can be one of the following eight
 *      types.
 *
 *  Hex   High    Low     Description
 *  Value Nybble  Nybble
 *  0x0X    0       X    "0": low nibble "X" is low nibble of frames
 *  0x1X    1       X    "1": low nibble "X" is high nibble of frames
 *  0x2X    2       X    "2": low nibble "X" is low nibble of seconds
 *  0x3X    3       X    "3": low nibble "X" is high nibble of seconds
 *  0x4X    4       X    "4": low nibble "X" is low nibble of minutes
 *  0x5X    5       X    "5": low nibble "X" is high nibble of minutes
 *  0x6X    6       X    "6": low nibble "X" is low nibble of hours
 *  0x7X    7       X    "7": low nibble "X" contains info about hours
 *                        and frames/second.
 *
 *  For more information, see:
 *
 *      https://www.recordingblogs.com/wiki/midi-quarter-frame-message
 *
 *  At this time, we do not go into these details in the eventspecs
 *  module.
 *
 * Song Position message (0xF2 lsb msb):
 *
 *      The two data bytes form a 14-bit value representing the number of
 *      MIDI beats (where 1 MIDI beat = a 16th note, or 6 MIDI clocks)
 *      since the start of the song. It supports up to 16,384 16th notes,
 *      which equals 4,096 quarter notes or 1,024 measures in 4/4 time.
 *
 * MIDI Timing Tick:
 *
 *  To convert a tick value or delta time into actual seconds or milliseconds,
 *  use the PPQ and the current tempo in microseconds per quarter note:
 *
 *                  us per quarter note
 *  us per tick =  ---------------------
 *                      PPQN
 *
 *                      us per tick
 *  Seconds} = Ticks x -------------
 *                      1,000,000
 */


#include <map>                          /* std::map<>                       */

#include "midi/eventcodes.hpp"          /* midi::status, to_byte(), etc.    */

/*
 *  Note that it is named "midi", not "rtl"; this scoping helps reduce the
 *  length of enumeration value names.
 */

namespace midi
{

/**
 *  An internal structure for accessing controller and other names via
 *  a status byte or meta byte. Bare midi::bytes are used.
 */

using namepair = struct
{
    int number;
    std::string name;
};

/**
 *  An internal map to support looking up the names of status bytes.
 */

using midi_name_map = std::map<int, std::string>;

/*
 *  Functions used in analysizing MIDI events by external callers.
 *
 * is_one_byte (byte m):
 *
 *      Test for channel messages that have only one data byte:
 *
 *          -   Program Change      = 0xC0
 *          -   Channel Pressure    = 0xD0
 *
 *      The rest of the channel messages have two data bytes.
 *
 *      We use the actual bytes rather than the status enumeration to save a
 *      couple of static_casts.
 *
 *          -   to_byte(status::program_change)
 *          -   to_byte(status::channel_pressure)
 *
 * is_two_byte (byte m):
 *
 *      Test for channel messages that have two data bytes: Note On, Note Off,
 *      Control Change, Aftertouch, and Pitch Wheel.  We use the actual bytes,
 *      to save some static_casts.
 *
 * status_msg_size (byte m):
 *
 *      In addition to the functions above, this one get the actual expected
 *      size of the message.  Returns -1 for SysEx.
 *
 * is_note (byte m):
 *
 *      Tests for messages that involve notes and velocity: Note On,
 *      Note Off, and Aftertouch.
 *
 * is_note_off_velocity (byte status, byte vel):
 *
 *      Tests for a Note On with a velocity of 0.  This function is used in the
 *      midifile module and in the is_note_off_recorded() function in Seq66.
 *
 *  There are many more such "one-liner" functions.  See the header file.
 */

/**
 *  In addition to the functions above, this one get the actual expected
 *  size of a status message.
 *
 *  Does not handle SysEx.
 *
 * \param s
 *      Must provide a status byte, otherwise this call makes no sense.
 *
 * \return
 *      Returns the expected size of the message, or (-1) if other data
 *      is needed to get the message size. Includes the size of the status
 *      byte.
 */

int
status_msg_size (int s)
{
    int result { -1 };
    if (is_two_byte_msg(s) || s == 0xF2)        /* status = song_pos        */
    {
        result = 3;                             /* status + d0 + d1         */
    }
    else if
    (
        is_one_byte_msg(s) || s == 0xF1 ||      /* status = quarter frame   */
        s == 0xF3                               /* status = song_select     */
    )
    {
        result = 2;                             /* status + d0              */
    }
    else if
    (
        s == 0xF6 ||                            /* status = tune_select     */
        s == 0xF8 ||                            /* status = clock           */
        s == 0xFA ||                            /* status = clk_start       */
        s == 0xFB ||                            /* status = clk_continue    */
        s == 0xFC ||                            /* status = clk_stop        */
        s == 0xFE ||                            /* status = active_sense    */
        s == 0xFF                               /* status = reset/meta !!!  */
    )
    {
        result = 1;
    }
    return result;
}

/**
 *  Get the actual expected size of a Meta message.
 *
 * \param m
 *      Must provide a meta byte, otherwise this call makes no sense.
 *
 * \return
 *      Returns the expected size of the message, or (-1) if other data
 *      is needed to get the message size.  Includes the size of the meta
 *      status byte (0xFF).
 */

int
meta_msg_size (int m)
{
    int result { -1 };
    if (m == 0x51)                      /* set_tempo                        */
    {
        result = 6;
    }
    else if (m == 0x58)                 /* time_signature                   */
    {
        result = 7;
    }
    else if (m == 0x00 || m == 0x59)    /* seq_number, key_signature        */
    {
        result = 5;
    }
    else if (m == 0x20 || m == 0x21)    /* midi_channel, midi_port (deprec) */
    {
        result = 4;
    }
    else if (m == 0x54)                 /* smpte_offset                     */
    {
        result = 8;
    }
    return result;
}

/**
 *  Name of the initial text meta events (00 through 07).
 *
 *  TODO: Use the namepair structure as the key.
 */

static const std::string sm_meta_text_labels [] =
{
    "Seq number",
    "Text",
    "Copyright",
    "Track Name",
    "Instrument Name",
    "Lyric",
    "Marker",
    "Cue Point",
    "Program Name",     /* TODO: backport to Seq66  */
    "Port Name"         /* TODO: backport to Seq66  */
};

std::string
meta_text_label (int m)
{
    std::string result { "Unknown" };
    if (m <= 9)
    {
        result = sm_meta_text_labels[int(m)];
    }
    else
    {
        switch (m)
        {
        case 0x20:  result = "MIDI Channel";        break;
        case 0x21:  result = "MIDI Port";           break;
        case 0x2f:  result = "End of Track";        break;
        case 0x51:  result = "Set Tempo";           break;
        case 0x54:  result = "SMPTE Offset";        break;
        case 0x58:  result = "Time Signature";      break;
        case 0x59:  result = "Key Signature";       break;
        case 0x7f:  result = "Seq Spec";            break;
        }
    }
    return result;
}

/*
 *  Basic statuses names.
 */

static const midi_name_map s_status_names
{
    { 0x80, "Note Off"              },
    { 0x90, "Note On"               },
    { 0xA0, "Aftertouch"            },
    { 0xB0, "Control"               },
    { 0xC0, "Program"               },
    { 0xD0, "Ch Pressure"           },
    { 0xE0, "Pitch Wheel"           },
    { 0xF0, "System Exclusive"      },
    { 0xF1, "Quarter Frame"         },
    { 0xF2, "Song Position"         },
    { 0xF3, "Song Select"           },
    { 0xF4, "Song F4"               },
    { 0xF5, "Song F5"               },
    { 0xF6, "Tune Request"          },
    { 0xF7, "Sysex Continue/End"    },
    { 0xF8, "MIDI Clock"            },
    { 0xF9, "Timing Tick"           },
    { 0xFA, "Clock Start"           },
    { 0xFB, "Clock Continue"        },
    { 0xFC, "Clock Stop"            },
    { 0xFD, "Song FD"               },
    { 0xFE, "Active Sense"          },
    { 0xFF, "Meta Message"          }
};

std::string
status_label (int m)
{
    std::string result { "Unknown" };
    const auto iter { s_status_names.find(m) };
    if (iter != s_status_names.end())
        result = iter->second;

    return result;
}

static const midi_name_map s_status_abbreviations
{
    { 0x80, "NoteOff"       },
    { 0x90, "NoteOn"        },
    { 0xA0, "Aftertouch"    },
    { 0xB0, "CtrlChange"    },
    { 0xC0, "ProgChange"    },
    { 0xD0, "ChPressure"    },
    { 0xE0, "PitchWheel"    },
    { 0xF0, "SysEx"         },
    { 0xF1, "QuarterFrame"  },
    { 0xF2, "SongPosition"  },
    { 0xF3, "SongSelect"    },
    { 0xF4, "SongF4"        },
    { 0xF5, "SongF5"        },
    { 0xF6, "TuneRequest"   },
    { 0xF7, "EOX"           },
    { 0xF8, "MIDIClock"     },
    { 0xF9, "TimingTick"    },
    { 0xFA, "ClkStart"      },
    { 0xFB, "ClkContinue"   },
    { 0xFC, "ClkStop"       },
    { 0xFD, "SongFD"        },
    { 0xFE, "ActiveSense"   },
    { 0xFF, "Meta"          }
};

std::string
status_abbreviation (int m)
{
    std::string result { "Unknown" };
    const auto iter { s_status_abbreviations.find(m) };
    if (iter != s_status_names.end())
        result = iter->second;

    return result;
}

int
status_number (const std::string & abbreviation)
{
    int result { -1 };
    for (const auto & a : s_status_abbreviations)
    {
        if (a.second == abbreviation)
        {
            result = a.first;
            break;
        }
    }
    return result;
}

midi::status
status_midi (const std::string & abbreviation)
{
    midi::status result { midi::status::erroneous };
    int nmbr { status_number(abbreviation) };
    if (nmbr >= 0)
    {
        midi::byte b { midi::byte(nmbr) };
        result = to_status(b);
    }
    return result;
}

/*
 *  Meta messages names.
 */

static const midi_name_map s_meta_names
{
    { 0x00, "Seq Number"        },
    { 0x01, "Text Event"        },
    { 0x02, "Copyright"         },
    { 0x03, "Seq Name"          },
    { 0x04, "Instrument"        },
    { 0x05, "Lyric"             },
    { 0x06, "Marker"            },
    { 0x07, "Cue Point"         },
    { 0x08, "Program Name"      },
    { 0x09, "Port Name"         },
    { 0x20, "MIDI Channel"      },
    { 0x21, "MIDI Port"         },
    { 0x2F, "End of Track"      },
    { 0x51, "Set Tempo"         },
    { 0x54, "SMPTE Offset"      },
    { 0x58, "Time Signature"    },
    { 0x59, "Key Signature"     },
    { 0x7F, "Seq Spec"          }
};

std::string
meta_label (int m)
{
    std::string result { "Unknown" };
    const auto iter { s_meta_names.find(m) };
    if (iter != s_meta_names.end())
        result = iter->second;

    return result;
}

static const midi_name_map s_meta_abbreviations
{
    { 0x00, "SeqNo"         },
    { 0x01, "Text"          },
    { 0x02, "Copyright"     },
    { 0x03, "SeqName"       },
    { 0x04, "Instrument"    },
    { 0x05, "Lyric"         },
    { 0x06, "Marker"        },
    { 0x07, "Cue"           },
    { 0x08, "Program"       },
    { 0x09, "Port"          },
    { 0x20, "MIDIChannel"   },
    { 0x21, "MIDIPort"      },
    { 0x2F, "TrkEnd"        },
    { 0x51, "SetTempo"      },
    { 0x54, "SMPTEOffset"   },
    { 0x58, "TimeSig"       },
    { 0x59, "KeySig"        },
    { 0x7F, "SeqSpec"       }
};

std::string
meta_abbreviation (int m)
{
    std::string result { "Unknown" };
    const auto iter { s_meta_abbreviations.find(m) };
    if (iter != s_meta_abbreviations.end())
        result = iter->second;

    return result;
}

int
meta_number (const std::string & abbreviation)
{
    int result { -1 };
    for (const auto & a : s_meta_abbreviations)
    {
        if (a.second == abbreviation)
        {
            result = a.first;
            break;
        }
    }
    return result;
}

}           // namespace midi

/*
 * eventcodes.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */
