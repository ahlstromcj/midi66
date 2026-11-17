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
 * \file          testfiles.cpp
 *
 *      Simple program to test basic MIDI file processing.
 *
 * \library       midi66
 * \author        Chris Ahlstrom
 * \date          2026-10-19
 * \updates       2026-10-19
 * \license       See above.
 *
 *      Provides a play test for reading and writing MIDI files.
 *      It tests:
 *
 *          -   midi::file
 *          -   midi::track
 *          -   midi::song
 *
 *      and their dependencies.
 *
 *  This module is still very much a work in progress.
 *
 *  It assumes it is run from the top-level directory of the midi66 project.
 */

#include <cctype>                       /* std::isdigit()                   */
#include <iostream>                     /* std::cout and std::cerr          */

#include "cfg/appinfo.hpp"              /* cfg::set_client_name()           */
#include "midi/song.hpp"                /* midi::song class                 */
#include "util/strfunctions.hpp"        /* util::string_to_int()            */

namespace   // anonymous
{

/**
 *  Tests of MIDI file parsing and writing for various files.
 */

const std::string s_base_directory { "tests/data/midi" };

const lib66::tokenization s_test_files
{
    "1Bar-export.mid",                      /* a simple standard MIDI file  */
    "1Bar.midi",
    "simpleblast-ch1-8th-notes.midi",
    "simpleblast-ch1-8th-notes-960.midi",
    "smoke.mid"
};

/**
 *  Assumes the song has already been set up.
 */

bool
test_it (midi::song & s, std::string & errmsg)
{
    bool result { s.arm_all_tracks() };
    if (result)
    {
        // TODO
    }
    if (! result)
    {
        if (s.error_pending())
            errmsg = s.error_messages();
    }
    return result;
}

/**
 *  Assumes the song has already been set up.
 */

bool
test (midi::song & s, const std::string & file)
{
    std::string errmsg;
    bool result { s.read_midi_file(file, errmsg, false) };
    if (result)
    {
        result = test_it(s, errmsg);
        if (result)
        {
            util::status_message("Success", file);
        }
        else
        {
            result = false;
            util::error_message("Failure", file);
            util::error_message("Error", errmsg);
        }
    }
    else
    {
        result = false;
        std::cerr << "Failed to read " << file << std::endl;
    }
    return result;
}

}           // namespace anonymous

/**
 *  The main routine. It first checks if a port was specified and queries
 *  the user if not. If valid, then the test files are opened and played.
 *
 *  rt_choose_port_number() in test_helpers opens temporary I/O ports to get
 *  data, and lets the user choose a port number.
 *
 *  We would like to support setting up a player, getting the port list from it,
 *  letting the user choose one, then launching the player.  TODO.
 */

int
main (int argc, char * argv [])
{
    (void) argc;
    (void) argv;
    int rcode { EXIT_FAILURE };
    std::string single_filename { /* rt_test_name() */ };
    bool can_run { true };
    if (can_run)
    {
        if (single_filename.empty())
        {
            for (const auto & file : s_test_files)
            {
                std::string testfile { s_base_directory };
                testfile += "/";
                testfile += file;

                midi::song s(testfile);         /* many default parameters  */
                bool success { test(s, testfile) };
                if (! success)
                {
                    rcode = EXIT_FAILURE;
                    break;
                }
            }
        }
        else
        {
            midi::song s(single_filename);      /* many default parameters  */
            bool success { test(s, single_filename) };
            if (! success)
                rcode = EXIT_FAILURE;
        }
    }
    return rcode;
}

/*
 * testfiles.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */
