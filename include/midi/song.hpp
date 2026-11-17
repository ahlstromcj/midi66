#if ! defined MIDI66_MIDI_SONG_HPP
#define MIDI66_MIDI_SONG_HPP

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
 * \file          song.hpp
 *
 *  This module declares/defines a base class for handling a MIDI file.
 *
 * \library       midi66
 * \author        Chris Ahlstrom
 * \date          2026-10-18
 * \updates       2026-11-11
 * \license       GNU GPLv2 or above
 *
 *  The song class holds all the data that can be read from a midi::file
 *  or written to it.
 *
 *  Song and MIDI-file header variables:
 *
 *      -   MThd()          (constant)
 *      -   MThdLength()    (constant)
 *      -   smf_format()
 *      -   track_count()
 *      -   get_ppqn()
 *
 *  We should make a separate structure.
 */

#include "midi/tracklist.hpp"               /* provides a set of tracks     */

namespace midi
{

/**
 *  This class supports the limited performance mode.
 */

class song
{
    friend class file;
    friend class track;
    friend class trackdata;
    friend class tracklist;

public:

    /**
     *  MIDI file header values. Also see the midi::file class.
     *
     *  The MIDI file header "magic" value, "MThd". Also see the midi::file
     *  class.
     */

    using header = struct
    {
        long hdr_MThd;
        long hdr_MThdLength;
        int hdr_smf_format;
        int hdr_track_count;
        midi::pulse hdr_ppqn;
    };

private:

    /**
     *  Holds these data for quicker access.
     */

    header m_song_header
    {
        0x4D546864,     /* hdr_MThd         */
        6,              /* hdr_MThdLength   */
        1,              /* hdr_smf_format   */
        0,              /* hdr_track_count  */
        384             /* hdr_ppqn         */
    };

    /**
     *  Holds the name of the MIDI file used to fill this song object.
     *  This should be the whold file-specification.
     */

    std::string m_file_name { };

    /**
     *  Contains all tracks that are (heh heh) in play.
     */

    tracklist m_track_list { };

    /**
     *  Actually, this value could go up to 2047... 2048 is used
     *  to indicate that there is no background sequence. See track::limit().
     */

    midi::track::number m_track_max { 1024 };

    /**
     *  Indicates the highest-number sequence. This value starts as -1, to
     *  indicate no sequences loaded, and then contains the highest sequence
     *  number hitherto loaded, plus 1 so that it can be used as a for-loop
     *  limit similar to m_track_max. It's maximum value should be
     *  m_track_max.
     *
     *  Currently meant only for limited context to try to squeeze a little
     *  extra speed out of playback. There's no easy way to lower this value
     *  when the highest sequence is deleted, though.
     */

    midi::track::number m_track_high { midi::track::unassigned() };

    /**
     *  If true (the default is false, the events in a track are sorted
     *  when the track is installed.
     */

    bool m_sort_on_install { false };

    /**
     *  Holds the full length of the currently loaded song.
     */

    midi::pulse m_max_extent { 0 };

    /**
     *  Indicates that an internal setup error occurred.  In this case, we will
     *  eventually want to emit an error prompt, though we keep going in order
     *  to populate the "rc" file correctly.
     */

    mutable bool m_error_pending { false };

    /**
     *  Holds the accumulated error messages.
     */

    mutable std::string m_error_messages { };

    /**
     *  It may be a good idea to eventually centralize all of the dirtiness of
     *  a performance here. All the GUIs use a song.
     */

    bool m_modified { false };

    /**
     *  A stronger "modified" flag. It indicates that some user-interface
     *  update would be needed.
     */

    bool m_needs_update { false };

    /**
     *  The main beats/minute for this song.
     */

    midi::bpm m_beats_per_minute { MIDI66_DEFAULT_BPM };

    /**
     *  The main beats-per-measure for this song.
     */

    int m_beats_per_bar { MIDI66_DEFAULT_BEATS_PER_BAR };

    /**
     *  The main beat-width for this song.
     */

    int m_beat_width { MIDI66_DEFAULT_BEAT_WIDTH };

    /**
     *  The main microseconds-per-quarter-note for this song.
     *  Rarely changed.
     */

    midi::microsec m_us_per_quarter_note { MIDI66_DEFAULT_US_PER_Q };

    /**
     *  The main 32nds-per-quarter-note.  Rarely changed.
     */

    int m_32nds_per_quarter { MIDI66_DEFAULT_32NDS_PER_QUARTER };

public:

    song () = default;
    song
    (
        const std::string & filespec,
        midi::pulse ppq     = MIDI66_DEFAULT_PPQN,
        midi::bpm bpminute  = MIDI66_DEFAULT_BPM,
        int beatsperbar     = MIDI66_DEFAULT_BEATS_PER_BAR,
        int beatwidth       = MIDI66_DEFAULT_BEAT_WIDTH
    );
    song (const song &) = default;
    song (song &&) = default;                   /* forced by iothread       */
    song & operator = (const song &) = default;
    song & operator = (song &&) = default;      /* ditto                    */
    virtual ~song ();

    const header & song_header () const
    {
        return m_song_header;
    }

    long MThd () const
    {
        return m_song_header.hdr_MThd;
    }

    std::string MThdString () const
    {
        return std::string("MThd");
    }

    long MThdLength () const
    {
        return m_song_header.hdr_MThdLength;
    }

    virtual bool clear_all (bool clearplaylist = false);

    tracklist & track_list ()
    {
        return m_track_list;
    }

    const tracklist & track_list () const
    {
        return m_track_list;
    }

    void clear ()
    {
        m_song_header.hdr_track_count = 0;
        m_track_high = track::unassigned();

        /*
         * There is more to do!
         */
    }

    int track_count () const
    {
        return m_song_header.hdr_track_count;
    }

    midi::track::number track_high () const
    {
        return m_track_high;
    }

    midi::track::number track_max () const
    {
        return m_track_max;
    }

    void print_tracks (const std::string & tag = "");

    midi::ppqn get_ppqn () const
    {
        return m_song_header.hdr_ppqn;
    }

    /*
     * Convenience function.
     */

    bool set_song_header (int smffmt, int tracks, int ppq)
    {
        m_song_header.hdr_smf_format = smffmt;
        m_song_header.hdr_track_count = tracks;
        m_song_header.hdr_ppqn = ppq;

        // TO DO: VALIDATE

        return true;
    }

    void get_song_header (int & smffmt, int & tracks, int & ppq)
    {
        smffmt = m_song_header.hdr_smf_format;
        tracks = m_song_header.hdr_track_count;
        ppq = m_song_header.hdr_ppqn;
    }

    bool set_ppqn (midi::ppqn ppq, bool user_change = false);

    midi::track::pointer get_track (midi::track::number trk)
    {
        return track_list().at(trk);
    }

    const midi::track::pointer get_track (midi::track::number trk) const
    {
        return track_list().at(trk);
    }

    bool is_track_active (midi::track::number trk) const
    {
        return bool(get_track(trk));
    }

    midi::bpm beats_per_minute () const
    {
        return m_beats_per_minute;
    }

    int beats_per_bar () const
    {
        return m_beats_per_bar;
    }

    int beat_width () const
    {
        return m_beat_width;
    }

    midi::microsec us_per_quarter_note () const
    {
        return m_us_per_quarter_note;
    }

    int get_32nds_per_quarter () const
    {
        return m_32nds_per_quarter;
    }

    bool beats_per_minute (midi::bpm bp, bool userchange = false);
    bool beats_per_bar (int bpmeasure, bool user_change = false);
    bool beat_width (int bw, bool user_change = false);

    void us_per_quarter_note (midi::microsec us)
    {
        m_us_per_quarter_note = us;
    }

    void set_32nds_per_quarter (int tpq)
    {
        m_32nds_per_quarter = tpq;
    }

    bool arm_all_tracks ();             /* useful in the test applications  */
    bool off_tracks ();

    /*
     * Other functions.
     */

    int smf_format () const
    {
        return m_song_header.hdr_smf_format;
    }

    void smf_format (int value)
    {
        m_song_header.hdr_smf_format = value == 0 ? 0 : 1 ;
    }

    bool error_pending () const
    {
        return m_error_pending;
    }

    const std::string & error_messages () const
    {
        return m_error_messages;
    }

    bool modified () const;

    /**
     *  This setter only sets the modified-flag to true. The setter that can
     *  falsify it, unmodify(), is private. No one but song and its friends
     *  should falsify this flag.
     */

    void modify (lib66::notification n = lib66::notification::no)
    {
        (void) n;
        m_modified = true;
    }

    void unmodify ();
    bool read_midi_file
    (
        const std::string & fn,
        std::string & errmsg,
        bool addtorecent = true
    );
    bool write_midi_file
    (
        const std::string & fn,
        std::string & errmsg,
        bool eventsonly = true
    );

    const std::string & file_name () const
    {
        return m_file_name;
    }

    void file_name (const std::string & fn)
    {
        m_file_name = fn;
    }

public:

    midi::pulse max_extent () const
    {
        return m_max_extent;
    }

public:

    bool set_track_name (midi::track::ref s, const std::string & name);
    bool set_midi_channel (midi::track::number trkno, int channel);
    bool new_track
    (
        midi::track::number & finalseq,
        midi::track::number trk = midi::track::unassigned()
    );

    bool request_track (midi::track::number trk = midi::track::unassigned())
    {
        static midi::track::number s_dummy;
        return new_track(s_dummy, trk);
    }

    bool install_track
    (
        midi::track * trk,
        midi::track::number & trkno,
        bool fileload = false
    );

public:

    midi::pulse get_max_extent () const;
    bool at_song_end ();

    bool needs_update () const
    {
        return m_needs_update;
    }

    void needs_update (bool flag = true)
    {
        m_needs_update = flag;
    }

protected:

    bool get_base_timing (track * trk);

private:

    void append_error_message (const std::string & msg) const;
    void reset_tracks (bool pause = false);

};          // class song

}           // namespace midi

#endif      // MIDI66_MIDI_SONG_HPP

/*
 * song.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */
