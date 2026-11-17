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
 * \file          filebase.cpp
 *
 *  This module declares/defines the base class for MIDI files.
 *
 * \library       midi66
 * \author        Chris Ahlstrom
 * \date          2026-11-07
 * \updates       2026-11-16
 * \license       GNU GPLv2 or above
 *
 */

#include <fstream>                      /* std::ifstream & std::ofstream    */
#include <memory>                       /* std::unique_ptr<>                */

#include "midi/filebase.hpp"            /* midi::filebase read/write class  */
#include "midi/song.hpp"                /* midi::song class ("tune")        */

namespace midi
{

/**
 *  Principal constructor.
 *
 * \param filespec
 *      Provides the name of the MIDI file to be read or written. This is the
 *      full path including the file-name.
 *
 * \param p
 *      Provides a mandatory player object, which wll hold some important
 *      global parameters (BPM, PPQN, etc.) about the file.
 *
 * Note that some members are initialized "in-class".
 */

filebase::filebase
(
    const std::string & filespec,
    song & s
) :
    m_tune          (s),                /* accessor tune()                  */
    m_file_spec     (filespec)          /* accessor file_spec()             */
{
    // no other code needed
}

/**
 *  A rote destructor.
 */

filebase::~filebase ()
{
    // empty body
}

/**
 *  Compare this function to song::new_track().
 */

track *
filebase::create_track ()
{
    midi::song & s { tune() };
    track * result { new (std::nothrow) track(&s) };
    if (not_nullptr(result))
    {
        // TODO
    }
    return result;
}

/**
 *  If the track pointer is valid and the "tune" song object exists, then this
 *  function calls player::install_track() to add the track pointer to the
 *  set-manager and hook it into the master bus.
 *
 * \param trk
 *      Provides a pointer to the track to be finalized and installed.
 *
 * \param trkno
 *      The starting track number to use, which might not be available.
 *
 * \return
 *      Returns true only if the track was installed in the player.
 */

bool
filebase::finalize_track (midi::track * trk, int trkno)
{
    bool result { not_nullptr(trk) };
    if (result)
    {
        int preferred_seqnum { trkno };
        result = tune().install_track(trk, preferred_seqnum, true);
    }
    return result;
}

bool
filebase::set_error (const std::string & msg)
{
    m_error_is_active = ! msg.empty();
    if (m_error_is_active)
        m_error_is_fatal = msg.back() == '!';

    m_error_message = msg;
    return m_error_is_active;
}

}           // namespace midi

/*
 * filebase.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */
