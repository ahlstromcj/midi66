#if ! defined MIDI66_MIDI_TRACKLIST_HPP
#define MIDI66_MIDI_TRACKLIST_HPP

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
 * \file          tracklist.hpp
 *
 *  This module declares a class for managing a set of midi::tracks.
 *
 * \library       midi66
 * \author        Chris Ahlstrom
 * \date          2022-07-09
 * \updates       2026-10-17
 * \license       GNU GPLv2 or above
 *
 *  This class is meant to hold each "track" (midi::track) as read in
 *  by the midi::file object.
 */

#include <vector>                       /* std::vector<>                    */

#include "midi/track.hpp"               /* midi::track                      */

namespace midi
{

/**
 *  A simple list of tracks.
 */

class tracklist
{

public:

    /**
     *  We use a vector as per the discussion at the top of the cpp module,
     *  instead of the map..
     *
     *      using container = std::map<int, track::pointer>;
     *
     *  Also note that midi::track::pointer is a shared pointer.
     */

    using container = std::vector<midi::track::pointer>;

private:

    /**
     *  Contains the ordered lists of track pointers.  These can
     *  later be transferred to another management object.
     */

    container m_tracks { };

    /**
     *  Indicates that the list has been modified in some way.
     *  We cannot use std::atomic<bool> because atomic values are not
     *  copy-constructible; we need to be able to copy tracklists.
     */

    bool m_modified { false };

    /**
     *  Indicates that the list has been sorted.
     */

    bool m_sorted { false };

public:

    tracklist ();
    tracklist (const tracklist &) = default;
    tracklist (tracklist &&) = default;
    tracklist & operator = (const tracklist &) = default;
    tracklist & operator = (tracklist &&) = default;
    virtual ~tracklist () = default;

    virtual void modify (lib66::notification n = lib66::notification::no)
    {
        (void) n;               /* not used in the base class */
        m_modified = true;
    }

    /*
     * virtual void unmodify (lib66::notification n = lib66::notification::no);
     */

    virtual void unmodify ();

    bool modified () const;
    bool add (midi::track::number trkno, midi::track * trk);
    void sort ();

    size_t size () const
    {
        return tracks().size();
    }

    bool empty () const
    {
        return tracks().empty();
    }

    void clear ()
    {
        if (! empty())
            modify();

        tracks().clear();
    }

    midi::track::pointer at (midi::track::number trkno);
    const midi::track::pointer at (midi::track::number trkno) const;

    container & tracks ()
    {
        return m_tracks;
    }

    const container & tracks () const
    {
        return m_tracks;
    }

protected:

    void set_last_tick (midi::pulse tick = midi::c_null_pulse);

private:

};          // class track

}           // namespace midi

#endif      // MIDI66_MIDI_TRACKLIST_HPP

/*
 * tracklist.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */
