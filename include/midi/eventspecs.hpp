#if ! defined MIDI66_EVENTSPECS_HPP
#define MIDI66_EVENTSPECS_HPP

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
 * \file          eventspecs.hpp
 *
 *  This module declares the array of MIDI program names.
 *
 * \library       midi66 library
 * \author        Chris Ahlstrom
 * \date          2026-10-21
 * \updates       2026-11-16
 * \license       GNU GPLv2 or above
 *
 */

#include <memory>                       /* std::unique_ptr<> template class */
#include <string>                       /* std::string<> template class     */

#include "cpp_types.hpp"                /* lib66::tokenization              */
#include "midi/event.hpp"               /* midi::event class                */

/*
 *  We started with note-specific functions, but the general functions
 *  work well for now.
 */

#undef MIDI66_SUPPORT_NOTE_SPECIFIC_EVENTSPECS

namespace midi
{

class song;
class track;

/*
 * MIDI header support. Also see the midi::song::header structure,
 * which is not used here.
 */

extern bool string_to_midi_header
(
    const std::string & line,
    int & smf_format,
    int & track_count,
    int & ppqn_value
);
extern std::string midi_header_to_string
(
    int smffmt,
    int trackct,
    int ppq
);

/*
 * These don't need to be exposed, but they might be useful later.
 */

extern std::string status_format (int s);
extern std::string status_format (const midi::event & ev);
extern std::string status_format (midi::status s);
extern int octave_of_note (int note, bool use_c3 = false);
extern std::string pitch_note (int note, bool use_c3 = false);

/*
 *  Functions of the internal types spec_stringer and spec_translator.
 */

#if defined MIDI66_SUPPORT_NOTE_SPECIFIC_EVENTSPECS

extern std::string note_to_string (const midi::event & ev, midi::status mstat);
extern std::string note_off_to_string (const midi::event & ev);
extern std::string note_on_to_string (const midi::event & ev);
extern std::string aftertouch_to_string (const midi::event & ev);
extern midi::event string_to_note (const std::string & line);

#endif

/*
 *  These functions are more general. We'll keep the "note" functions
 *  above around for awhile.
 */

extern midi::pulse extract_sysex
(
    const std::string & line,
    midi::bytes & bytedestination
);
extern midi::event string_to_midi_event (const std::string & line);
extern std::string midi_event_to_string (const midi::event & ev);
extern std::unique_ptr<midi::track> lines_to_midi_track
(
    song * s,
    const lib66::tokenization & lines
);
extern std::string midi_track_to_string (const midi::track & trk);
extern lib66::tokenization midi_track_to_lines (const midi::track & trk);
extern midi::pulse extract_meta
(
    const std::string & line,
    midi::bytes & bytedestination
);
extern std::string meta_to_string (const midi::event & ev);

#if defined USE_EXTENDED_META_SPECS
extern std::string meta_name (int metatype);
extern int meta_type (const std::string & metaname);
#endif

}           // namespace midi

#endif      // MIDI66_EVENTSPECS_HPP

/*
 * eventspecs.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */
