#if ! defined MIDI66_MIDI_TRANSCRIPT_HPP
#define MIDI66_MIDI_TRANSCRIPT_HPP

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
 * \file          transcript.hpp
 *
 *  This module declares/defines the base class for MIDI files.
 *
 * \library       midi66
 * \author        Chris Ahlstrom
 * \date          2026-11-07
 * \updates       2026-11-16
 * \license       GNU GPLv2 or above
 *
 *  This version is very basic, and does not include any Seq66 features.
 */

#include "cpp_types.hpp"                /* lib66::tokenization alias        */
#include "midi/filebase.hpp"            /* midi::filebase read/write class  */

namespace midi
{
    class song;

/**
 *  This class handles the parsing and writing of MIDI files.  In addition to
 *  the standard MIDI tracks, it also handles some "private" or "proprietary"
 *  tracks specific to Seq24.  It does not, however, handle SYSEX events.
 */

class transcript final : public midi::filebase
{

private:

    /**
     *  Holds every line of text in the MIDI file transcript.
     */

    lib66::tokenization m_lines;

public:

    transcript () = delete;             /* base class has a refrence member */
    transcript
    (
        const std::string & filespec,
        midi::song & p
    );
    transcript (const transcript &) = default;
    transcript (transcript &&) = delete;
    transcript & operator = (const transcript &) = delete;
    transcript & operator = (transcript &&) = delete;
    virtual ~transcript ();

    virtual bool write (bool eventsonly = true) override;
    virtual bool parse (const std::string & tag = "") override;
    virtual bool set_error (const std::string & msg) override;

    std::string magic_string () const
    {
        return "%Midi66 Transcript";
    }

protected:

#if 0
    virtual midi::track * create_track ();
    virtual bool finalize_track (midi::track * trk, int trkno);
    virtual void clear () = 0;
    virtual int read_header () = 0;
    virtual bool put_track (/*const*/ midi::track & lst) = 0;
    virtual bool put_track_events (/*const*/ midi::track & lst) = 0;
    virtual bool put_header (int numtracks, int smfformat = 1) = 0;
#endif

    lib66::tokenization & lines ()
    {
        return m_lines;
    }

    const lib66::tokenization & lines () const
    {
        return m_lines;
    }

};          // class transcript

/*-------------------------------------------------------------------------
 * Free functions
 *-------------------------------------------------------------------------*/

extern bool read_midi_transcript
(
    const std::string & transcriptfile,
    song & s
);
extern bool write_midi_transcript
(
    const std::string & transcriptfile,
    const song & s
);

}           // namespace midi

#endif      // MIDI66_MIDI_TRANSCRIPT_HPP

/*
 * transcript.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */
