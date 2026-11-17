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
 * \file          song.cpp
 *
 *  This module defines the base class for a song of MIDI patterns.
 *
 * \library       midi66
 * \author        Chris Ahlstrom and others
 * \date          2026-10-18
 * \updates       2026-11-08
 * \license       GNU GPLv2 or above
 *
 */

#include <algorithm>                    /* std::find() for std::vector      */

#include "c_macros.h"                   /* not_nullptr macro                */
#include "midi/calculations.hpp"        /* midi::tempo_us_from_bpm()        */
#include "midi/file.hpp"                /* midi::read_midi_file()           */
#include "midi/song.hpp"                /* midi::song, this class           */
#include "util/msgfunctions.hpp"        /* util::warn_message() etc.        */
#include "util/filefunctions.hpp"       /* util::filename_base(), etc.      */

namespace midi
{

/**
 *  Indicates how much of a long file-path we will show using the
 *  shorten_file_spec() function.
 *
 *      static const int c_long_path_max = 56;
 */

/**
 *  Default constructor. Note that most of the members are default in
 *  the class header (i.e. "in-class").
 */

song::song
(
    const std::string & filespec,
    midi::pulse ppq,
    midi::bpm bpminute,
    int beatsperbar,
    int beatwidth
) :
    m_file_name         { filespec },
    m_beats_per_minute  { bpminute },
    m_beats_per_bar     { beatsperbar },
    m_beat_width        { beatwidth }
{
    set_ppqn(ppq);
}

/**
 *  The destructor sets some running flags to false, signals this condition,
 *  then joins the input and output threads if the were launched.
 *
 *  A thread that has finished executing code, but has not yet been joined is
 *  still considered an active thread of execution and is therefore joinable.
 */

song::~song ()
{
    // TODO
}

/**
 *  This function emits an error message to cerr via the function
 *  util::error_message().
 */

void
song::append_error_message (const std::string & msg) const
{
    static std::vector<std::string> s_old_msgs;
    std::string newmsg { msg };
    m_error_pending = true;                         /* a mutable boolean    */
    if (newmsg.empty())
        newmsg = "Player error";

    if (! m_error_messages.empty())
    {
        const auto finding
        {
            std::find(s_old_msgs.cbegin(), s_old_msgs.cend(), newmsg)
        };
        if (finding == s_old_msgs.cend())
        {
            m_error_messages += " ";
            m_error_messages += newmsg;
            s_old_msgs.push_back(newmsg);
            util::error_message("Player", newmsg);
        }
    }
    else
    {
        m_error_messages = newmsg;
        s_old_msgs.push_back(newmsg);
        util::error_message("Player", newmsg);
    }
}

/**
 *  Unmodify this object and all the tracks it contains.
 */

void
song::unmodify ()
{
    m_modified = false;                     /* m_needs_update = false;  */
    track_list().unmodify();
}

/**
 *  This improved version checks all of the tracks. This allows the user to
 *  unmodify a track without using song::modify().
 */

bool
song::modified () const
{
    bool result { m_modified };
    if (! result)
        result = track_list().modified();

    return result;
}

/*
 * -------------------------------------------------------------------------
 *  Sequence Creation/Installation
 * -------------------------------------------------------------------------
 */

/**
 *  Helper function.
 */

bool
song::get_base_timing (track * trk)
{
    bool result { not_nullptr(trk)  };
    if (result)
    {
        /*
         * This is a better location than track::set_parent().
         */

        result = beats_per_bar(trk->beats_per_bar());
        if (result)
            result = beat_width(trk->beat_width());

        if (result)
            result = beats_per_minute(trk->beats_per_minute());
    }
    return result;
}

/**
 *  It assumes values have already been checked.  It does not set the "is
 *  modified" flag, since adding a track by loading a MIDI file should not
 *  set it.  This function *does not* delete the track already present with
 *  the given track number; instead, it keeps incrementing the track number
 *  until an open slot is found.
 *
 * \param seq The pointer to the pattern/track to add.
 *
 * \param trkno
 *      The track number of the pattern to be added.  Not validated, to
 *      save some time.  This is only the starting value; if already filled,
 *      then next open slot is used, and this value will be updated to the
 *      actual number.
 *
 * \param fileload
 *      If true (the default is false), the modify flag will not be set.
 *
 * \return
 *      Returns true if the track was successfully added.
 */

bool
song::install_track
(
    track * trk,
    midi::track::number & trkno,
    bool fileload
)
{
    bool result { track_list().add(trkno, trk) };   /* add to track-list    */
    if (result)
    {
        trkno = trk->track_number();            /* side-effect              */
        if (trkno == 0)                         /* first track/tempo track  */
        {
            result = get_base_timing(trk);
        }
        if (trkno > track_high())
            m_track_high = trkno;

        if (result)
        {
#if defined PLATFORM_DEBUG_TMI
            std::string label { "Installed track " };
            label += std::to_string(trk->track_number());
            label += ": ";
            label += trk->track_name();
            printf("%s\n", V(label));
            lib66::toggler sorting
            {
                m_sort_on_install ? lib66::toggler::on : lib66::toggler::off
            };
//          trk->set_parent(this, sorting);     /* also sets a lot of stuff */
#endif
            if (! fileload)
                modify();
        }
    }
    return result;
}

/**
 *  Creates a new pattern/track for the given slot, and sets the new
 *  pattern's master MIDI bus address.  Then it activates the pattern [this is
 *  done in the install_track() function].  It doesn't deal with thrown
 *  exceptions.
 *
 *  This function is called by the seqmenu and mainwnd objects to create a new
 *  track.  We now pass this track to install_track() to better
 *  handle potential memory leakage, and to make sure the track gets
 *  counted.  Also, adding a new track from the user-interface is a
 *  significant modification, so the "is modified" flag gets set.
 *
 *  If enabled, wire in the MIDI buss override.
 *
 *  Do not worry about this warning (yet):
 *
 *      Potential leak of memory pointed to by 'trkptr'
 *
 * \param seq
 *      The prospective track number of the new track.  If not set to
 *      track::unassigned() (-1), then the track is also installed, and this
 *      value will be updated to the actual number.
 *
 * \return
 *      Returns true if the track is valid.  Do not use the
 *      track if false is returned, it will be null.
 */

bool
song::new_track (track::number & finalseq, track::number trkno)
{
    track * trkptr { new (std::nothrow) track(this) };
    bool result { not_nullptr(trkptr) };
    if (result && trkno != track::unassigned())
    {
        result = install_track(trkptr, trkno);
        if (result)                                 /* new 2021-10-01   */
        {
            const track::pointer t { get_track(trkno) };
            result = bool(t);
            if (result)
            {
                t->set_dirty();
                finalseq = t->track_number();
                // notify_track_change(finalseq, change::recreate);
            }
        }
    }
    return result;
}

/*
 * -------------------------------------------------------------------------
 *  NEXT
 * -------------------------------------------------------------------------
 */

/**
 *  Sets the PPQN for the master buss, JACK assistant, and the song.
 *  Note that we do not set the modify flag or do notification notification
 *  here.  See the change_ppqn() function instead.
 *
 * \setter ppqn
 *      Also sets other related members.
 *
 *      While running it is better to call change_ppqn(), in order to run
 *      though ALL patterns and user-interface objects to fix them.
 *
 * \param ppq
 *      Provides a PPQN that should be different from the current value and be
 *      in the legal range of PPQN values.
 */

bool
song::set_ppqn (midi::ppqn p, bool user_change)
{
    bool result { p != get_ppqn() || ! user_change };
    if (result)
    {
        if (result)
        {
            m_song_header.hdr_ppqn = p;
        }
    }
    return result;
}

bool
song::arm_all_tracks ()
{
    auto & trklist { track_list().tracks() };
    bool result { ! trklist.empty() };
    if (result)
    {
        for (auto trk : trklist)                    /* pointers */
        {
            if (trk->active())
            {
                result = true;
                trk->set_armed(true);
            }
        }
    }
    return result;
}

bool
song::off_tracks ()
{
    auto & trklist { track_list().tracks() };
    bool result { ! trklist.empty() };
    if (result)
    {
        for (auto trk : trklist)                    /* pointers */
        {
            if (trk->active())
            {
                result = true;
                trk->set_armed(false);
            }
        }
    }
    return result;
}

/**
 *  Sets the value of the BPM into the master MIDI buss, after making
 *  sure it is squelched to be between 20 and 500.  Replaces
 *  song::set_bpm() from seq24.
 *
 *  The value is set only if neither JACK nor this song object are
 *  running.
 *
 *  It's not clear that we need to set the "is modified" flag just because we
 *  changed the beats per minute.  This setting does get saved to the MIDI
 *  file, with the c_bpmtag.
 *
 *  Do we need to adjust the BPM of all of the tracks, including the
 *  potential tempo track???  It is "merely" the putative main tempo of the
 *  MIDI tune.  Actually, this value can now be recorded as a Set Tempo event
 *  by user action in the main window (and, later, by incoming MIDI Set Tempo
 *  events).
 *
 * \param bpmin
 *      Provides the beats/minute value to be set.  Checked for validity.  It
 *      is a wide range of speeds, well beyond what normal music needs.
 *
 * \param userchange
 *      If true, the user changed the parameter.  Not used in this base class
 *      function.
 */

bool
song::beats_per_minute (midi::bpm bpmin, bool user_change)
{
    bool result
    {
        bpmin != beats_per_minute() || ! user_change
    };
    if (result)
    {
        bpmin = fix_tempo(bpmin);
        m_beats_per_minute = bpmin;
    }
    return result;
}

/**
 *  Clears all of the patterns/tracks. Attempts to reset the song to
 *  its startup condition when no MIDI file is loaded
 *
 *  The mainwnd module calls this function, and the midifile and wrkfile
 *  classes, if a play-list is being loaded and verified.  Note that perform
 *  now handles the "is modified" flag on behalf of all external objects, to
 *  centralize and simplify the dirtying of a MIDI tune.
 *
 *  Anything else to clear?  What about all the other track flags?  We can
 *  beef up remove_track() for them, at some point.
 *
 *  Added stazed code from 1.0.5 to abort clearing if any of the tracks are
 *  in editing.
 *
 * \warning
 *      We have an issue.  Each GUIs conditional_update() function can call
 *      this one, potentially telling later GUIs that they do not need to
 *      update.  This affect the needs_update() function when not running
 *      playback.
 *
 * \param clearplaylist
 *      Defaults to false. Not supported in the base class.
 *
 * \return
 *      Returns true if the clear-all operation could be performed.  If false,
 *      then at least one active track was in editing mode (derived classes
 *      only).
 */

bool
song::clear_all (bool clearplaylist)
{
    (void) clearplaylist;
    track_list().clear();
    m_max_extent = 0;               /* force an "empty" song            */
    unmodify();                     /* new, we start afresh             */
    return true;
}

/**
 *  For all active patterns/tracks, get its playing state, turn off the
 *  playing notes, set playing to false, zero the markers, and, if not in
 *  playback mode, restore the playing state.  Note that these calls are
 *  folded into one member function of the track class.  Finally, flush the
 *  master MIDI buss.
 *
 *  Could use a member function pointer to avoid having to code two loops.
 *  We did it.  Note that std::shared_ptr does not support operator::->*, so
 *  we have to get() the pointer.
 *
 *  The loop also flushes.
 *
 * \param p
 *      Try to prevent notes from lingering on pause if true.  By default, it
 *      is false.
 */

void
song::reset_tracks (bool p)
{
    void (track::* f) (bool) { p ? &track::pause : &track::stop };
    bool songmode { false };
    for (auto & trk : track_list().tracks())
    {
        track * trkptr { trk.get() };
        (trkptr->*f)(songmode);
    }
}

/**
 *  Sets the beats per measure.
 *
 *  Note that a lambda function is used to make the changes.
 */

bool
song::beats_per_bar (int bpmeasure, bool user_change)
{
    bool result
    {
        bpmeasure != beats_per_bar() || ! user_change
    };
    if (result)
    {
        m_beats_per_bar = bpmeasure;
    }
    return result;
}

/**
 *
 * \param bw
 *      Provides the value for beat-width.
 *
 *      TODO: Also used to set the
 *      beat-width in the JACK assistant object.
 */

bool
song::beat_width (int bw, bool user_change)
{
    bool result
    {
        bw != beat_width() || ! user_change
    };
    if (result)
    {
        m_beat_width = bw;
    }
    return result;
}

/**
 *  The only legal values for channel are 0 through 15, and null_channel(),
 *  which is 0x80, and indicates a "Free" channel (i.e. the pattern is
 *  channel-free.
 *
 *  The live-grid popup-menu calls this function, while the pattern-editor
 *  dropdown calls track::midi_channel() directly.  Both calls set the
 *  user-change flag.
 *
 * \param trkno
 *      Provides the track number for the channel setting.
 *
 * \paramm channel
 *      Provides the channel setting, 0 to 15.  If greater than that, it is
 *      coerced to the null-channel (0x80).
 */

bool
song::set_midi_channel (track::number trkno, int channel)
{
    track::pointer s { get_track(trkno) };
    bool result { bool(s) };
    if (result)
    {
        if (channel >= c_channel_max)                   /* 0 to 15, Free    */
            channel = null_channel();                   /* Free             */

        result = s->midi_channel(midi::byte(channel), true);  /* user ch. */
    }
    return result;
}

/**
 *  Also modify()'s.
 */

bool
song::set_track_name (track::ref t, const std::string & name)
{
    bool result { name != t.track_name() };
    if (result)
    {
        t.track_name(name);
    }
    return result;
}

midi::pulse
song::get_max_extent () const
{
    midi::pulse result { 0 };
    for (const auto & trk : track_list().tracks())
    {
        midi::pulse t { trk->get_max_timestamp() };
        if (t > result)
            result = t;
    }
    return result;
}

/*
 * -------------------------------------------------------------------------
 *  File handling
 * -------------------------------------------------------------------------
 */

/**
 *  This function calls the midi::read_midi_file() free function, and then
 *  sets the PPQN value.
 *
 * \param fn
 *      Provides the full path file-specification for the MIDI file.
 *
 * \param [out] errmsg
 *      Provides the destination for an error message, if any.
 *
 * \param addtorecent
 *      Not used here, as there is not recent-file support in this potential
 *      base class. However, true by default.
 *
 * \return
 *      Returns true if the function succeeded.  If false is returned, there
 *      should be an error message to display.
 */

bool
song::read_midi_file
(
    const std::string & fn,
    std::string & errmsg,
    bool addtorecent
)
{
    errmsg.clear();

    bool result { midi::read_midi_file(*this, fn, errmsg) };
    if (result)
    {
        (void) addtorecent;
        m_max_extent = get_max_extent();

        // TODO? Get PPQN and BPM? Update clientinfo?
    }
    return result;
}

/**
 *  This function calls the midi::write_midi_file() free function.
 *
 * \param fn
 *      Provides the full path file-specification for the MIDI file.
 *
 * \param [out] errmsg
 *      Provides the destination for an error message, if any.
 *
 * \param eventsonly
 *      If true, only events existing in the tracks are written.
 *      Of course, the MIDI header is still written.
 *
 * \return
 *      Returns true if the function succeeded.  If false is returned, there
 *      should be an error message to display.
 */

bool
song::write_midi_file
(
    const std::string & fn,
    std::string & errmsg,
    bool eventsonly
)
{
    errmsg.clear();

    bool result { midi::write_midi_file(*this, fn, errmsg, eventsonly) };
    if (result)
    {
        // to do?
    }
    return result;
}

void
song::print_tracks (const std::string & tag)
{
    if (tag.empty())
        printf("Song tracks:\n");
    else
        printf("Song tracks %s:\n", V(tag));

    printf
    (
        "Header: SMF %d, %d tracks, PPQN %d\n\n",
        smf_format(), track_count(), int(get_ppqn())
    );
    for (const auto & trk : track_list().tracks())
    {
        if (trk)
        {
            std::string trkstring { trk->to_string() };
            printf("%s\n", V(trkstring));
        }
        else
        {
            printf("Bad track\n");
        }
    }
}

}           // namespace midi

/*
 * song.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */
