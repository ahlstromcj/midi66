#if ! defined MIDI66_MIDI_FILEBASE_HPP
#define MIDI66_MIDI_FILEBASE_HPP

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
 * \file          filebase.hpp
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

#include "midi/track.hpp"               /* midi::track raw midi container   */
#include "midi/tracklist.hpp"           /* midi::tracklist vector of tracks */

namespace midi
{

class song;

/**
 *  This class handles the parsing and writing of MIDI files.  In addition to
 *  the standard MIDI tracks, it also handles some "private" or "proprietary"
 *  tracks specific to Seq24.  It does not, however, handle SYSEX events.
 */

class filebase
{

private:

    /**
     *  The optional destination for global parameters obtained from the
     *  MIDI file.
     */

    midi::song & m_tune;

    /**
     *  The tentative list of tracks.
     */

    tracklist m_track_list { };

    /**
     *  Holds the size of the MIDI file.
     */

    size_t m_file_size { 0 };

    /**
     *  The unchanging name of the MIDI file. Includes the full path
     *  name to reach the file.
     */

    const std::string m_file_spec { };

    /**
     *  The value of the PPQN from the file itself.
     */

    int m_file_ppqn { 0 };

    /**
     *  Error handling.
     */

    std::string m_error_message { };
    bool m_error_is_active { false };
    bool m_error_is_fatal { false };

public:

    filebase () = delete;               /* because of a reference member    */
    filebase
    (
        const std::string & filespec,
        midi::song & p
    );
    filebase (const filebase &) = default;
    filebase (filebase &&) = delete;
    filebase & operator = (const filebase &) = delete;
    filebase & operator = (filebase &&) = delete;
    virtual ~filebase ();

    virtual bool write (bool eventsonly = true) = 0;
    virtual bool parse (const std::string & tag = "") = 0;

    const std::string & file_spec () const
    {
        return m_file_spec;
    }

    virtual const std::string & error_message () const
    {
        return m_error_message;
    }

    virtual bool error_is_active () const
    {
        return m_error_is_active;
    }

    virtual bool error_is_fatal () const
    {
        return m_error_is_fatal;
    }

    int file_ppqn () const
    {
        return m_file_ppqn;
    }

    size_t file_size () const
    {
        return m_file_size;
    }

protected:

    void file_size (size_t sz)
    {
        m_file_size = sz;
    }

    virtual midi::track * create_track ();
    virtual bool finalize_track (midi::track * trk, int trkno);

#if 0
    virtual void clear () = 0;
    virtual int read_header () = 0;
    virtual bool put_track (/*const*/ midi::track & lst) = 0;
    virtual bool put_track_events (/*const*/ midi::track & lst) = 0;
    virtual bool put_header (int numtracks, int smfformat = 1) = 0;
#endif

    virtual bool set_error (const std::string & msg);

    virtual void clear_errors ()
    {
        m_error_message.clear();
        m_error_is_active = false;
        m_error_is_fatal = false;
    }

    midi::song & tune ()
    {
        return m_tune;
    }

    const midi::song & tune () const
    {
        return m_tune;
    }

    tracklist & track_list ()
    {
        return m_track_list;
    }

    const tracklist & track_list () const
    {
        return m_track_list;
    }

    void file_ppqn (int p)
    {
        m_file_ppqn = p;
    }


};          // class filebase

}           // namespace midi

#endif      // MIDI66_MIDI_FILEBASE_HPP

/*
 * filebase.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */
