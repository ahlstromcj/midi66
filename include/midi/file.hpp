#if ! defined MIDI66_MIDI_FILE_HPP
#define MIDI66_MIDI_FILE_HPP

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
 * \file          file.hpp
 *
 *  This module declares/defines the base class for MIDI files.
 *
 * \library       midi66
 * \author        Chris Ahlstrom
 * \date          2015-07-24
 * \updates       2026-11-17
 * \license       GNU GPLv2 or above
 *
 *  This version is very basic, and does not include any Seq66 features.
 */

#include "midi/filebase.hpp"            /* midi::filebase class             */
#include "midi/splitter.hpp"            /* midi::splitter SMF 0 converter   */
#include "midi/track.hpp"               /* midi::track raw midi container   */
#include "midi/tracklist.hpp"           /* midi::tracklist vector of tracks */
#include "util/bytevector.hpp"          /* util::bytevector big-endian data */

namespace midi
{
    class song;

/**
 *  This class handles the parsing and writing of MIDI files.  In addition to
 *  the standard MIDI tracks, it also handles some "private" or "proprietary"
 *  tracks specific to Seq24.  It does not, however, handle SYSEX events.
 */

class file final : public filebase
{

private:

    /**
     *  Provides management code for handling big-endian data, which includes
     *  MIDI data.
     */

    util::bytevector m_data { };

    /**
     *  Provides the ratio of the main PPQN to the file PPQN, for use with
     *  scaling.
     *
     *      double m_ppqn_ratio;
     */

    /**
     *  Provides support for SMF 0. This object holds all of the information
     *  needed to split a multi-channel track.
     */

    midi::splitter m_smf0_splitter { };

    /**
     *  Provides the option to split an SMF 0 MIDI file, converting it to an
     *  SMF 1 MIDI file.
     */

    bool m_smf0_split { false };

public:

    file () = delete;
    file
    (
        const std::string & filespec,
        midi::song & p,
        bool smf0split = true                   /* applies only to reading  */
    );
    file (const file &) = delete;
    file (file &&) = delete;
    file & operator = (const file &) = delete;
    file & operator = (file &&) = delete;
    virtual ~file ();

    /*
     * New for midi66 library.
     */

    virtual bool write (bool eventsonly = true);
    virtual bool parse (const std::string & tag = "");

    const util::bytevector & data () const
    {
        return m_data;
    }

    virtual const std::string & error_message () const override
    {
        return m_data.error_message();
    }

    virtual bool error_is_fatal () const override
    {
        return m_data.error_is_fatal();
    }

    bool smf0_split () const
    {
        return m_smf0_split;
    }

protected:

    virtual midi::track * create_track ();
    virtual bool finalize_track (midi::track * trk, int trkno);

    void clear ()
    {
        m_data.clear();
    }

    virtual void clear_errors () override
    {
        m_data.clear_errors();
    }

    void smf0_split (bool f)
    {
        m_smf0_split = f;
    }

    int read_header ();
    bool parse_smf_0 ();
    bool parse_smf_1 ();

    midi::byte read_byte () const
    {
        return m_data.get_byte();
    }

    midi::ulong read_long () const
    {
        return m_data.get_long();
    }

    midi::ushort read_short () const
    {
        return m_data.get_short();
    }

    size_t remainder () const
    {
        return m_data.remainder();
    }

    void set_position (size_t offset)       /* should call it seek()        */
    {
        m_data.seek(offset);
    }

    void put_long (midi::ulong value)
    {
        m_data.put_long(value);
    }

    void put_short (midi::ushort value)
    {
        m_data.put_short(value);
    }

    void put (midi::byte c)
    {
        m_data.put_byte(c);
    }

    bool put_track (/*const*/ midi::track & lst);
    bool put_track_events (/*const*/ midi::track & lst);
    bool put_header (int numtracks, int smfformat = 1);

    virtual bool set_error (const std::string & msg) override
    {
        return m_data.set_error(msg);
    }

    bool set_error_dump (const std::string & msg)
    {
        return m_data.set_error_dump(msg);
    }

    bool set_error_dump (const std::string & msg, unsigned long p)
    {
        return m_data.set_error_dump(msg, p);
    }

    /**
     *  Check for special SysEx ID byte.
     *
     * \param ch
     *      Provides the byte to be checked against 0x7D through 0x7F.
     *
     * \return
     *      Returns true if the byte is SysEx special ID.
     */

    bool is_sysex_special_id (midi::byte ch)
    {
        return ch >= 0x7D && ch <= 0x7F;
    }

};          // class file

/*
 *  Free functions related to MIDI files.
 */

extern file * make_midi_file_object
(
    midi::song & p,
    const std::string & filespec,
    bool smf0split = true
);
extern bool read_midi_file
(
    midi::song & p,
    const std::string & fn,
    std::string & errmsg
);
extern bool write_midi_file
(
    const midi::song & p,
    const std::string & fn,
    std::string & errmsg,
    bool eventsonly = true
);
extern midi::bytes read_raw_midi (const std::string & fname, midi::song & s);
extern bool write_raw_midi
(
    const std::string & fn,
    const midi::bytes & databytes
);

}           // namespace midi

#endif      // MIDI66_MIDI_FILE_HPP

/*
 * file.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */
