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
 * \file          mftranscript_test.cpp
 *
 *      This test uses the functions like those of mftranscript.cpp,
 *      but drops all the other stuff.
 *
 * \application   mftranscript
 * \library       midi66
 * \author        Chris Ahlstrom
 * \date          2026-11-14
 * \updates       2026-11-16
 * \license       See above.
 */

#include <iostream>                     /* std::cout and std::cerr          */
#include <utility>                      /* std::pair template class         */
#include <vector>                       /* std::vector template class       */

#include "midi/eventspecs.hpp"          /* midi::conversion functions       */
#include "midi/file.hpp"                /* midi::file class                 */
#include "midi/song.hpp"                /* midi::song class                 */
#include "midi/transcript.hpp"          /* midi::transcript class           */
#include "util/filecomparison.hpp"      /* util::compare...() functions     */
#include "util/filefunctions.hpp"       /* util::file_extension()           */
#include "util/msgfunctions.hpp"        /* util::investigate()              */
#include "util/strfunctions.hpp"        /* util::string_to_int()            */

namespace   // anonymous
{

bool
convert_transcript_to_midi
(
    const std::string & transcriptfile,
    const std::string & midifile
)
{
    midi::song s;
    bool result { midi::read_midi_transcript(transcriptfile, s) };
    if (result)
    {
        std::string errmsg;
        util::status_message("Converting to MIDI", transcriptfile);
        result = midi::write_midi_file(s, midifile, errmsg);
        if (result)
            util::status_message("Success!");
        else
            util::error_message(transcriptfile, errmsg);
    }
    else
        util::error_message("Read failed", transcriptfile);

    return result;
}

bool
convert_midi_to_transcript
(
    const std::string & midifile,
    const std::string & transcriptfile
)
{
    midi::song s;
    std::string errmsg;
    bool result { midi::read_midi_file(s, midifile, errmsg) };
    if (result)
    {
        util::status_message("Converting to transcript", midifile);
        result = midi::write_midi_transcript(transcriptfile, s);
        if (result)
            util::status_message("Success!");
        else
            util::error_message(midifile, errmsg);
    }
    if (! result)
        util::error_message("Read failed", midifile);

    return result;
}

/**
 *  Locations of input and output files for this test. To avoid figuring
 *  out the build setup, we use "build/results/midi" as the destination.
 *  The "./work.sh --clean" command will delete this directory.
 */

const std::string s_test_input_midi         { "tests/data/midi" };
const std::string s_test_input_transcripts  { "tests/data/mftranscript" };
const std::string s_test_result_directory   { "build/results" };

/**
 *  Runs the test on a file.
 *
 *  Given the input file example.midi, we have the following
 *  test-file names for an input file with no path, and then a path:
 *
 *      Convert MIDI to Transcript test:
 *
 *          midiinput:          tests/data/midi/example.midi
 *          transcriptoutput:   build/results/example.transcript
 *
 *          Compare             build/results/example.transcript
 *          to                  tests/data/mftranscript/example.transcript
 *
 *  *If* the tests above succeed:
 *
 *      Convert Transcript to MIDI test:
 *
 *          transcriptinput:    build/results/example.transcript
 *          midioutput:         build/results/example.midi
 *
 *          Compare             build/results/example.midi
 *          to                  tests/data/midi/example.midi
 *
 *
 * \param midifile
 *      Provides the base name of the MIDI file, and the base name only.
 *      Example: smoke.mid or smoke.midi.
 *
 *  \return
 *      Returns true if the test succeeded.
 */

bool
process_midi_file (const std::string & midifile)
{
    std::string ext { util::file_extension(midifile) };
    bool result { ext == "midi" || ext == "mid" || ext == "smf" };
    if (result)
    {
        std::string midiinput           /* tests/data/midi/smoke.mid       */
        {
            util::filename_concatenate(s_test_input_midi, midifile)
        };
        std::string transcriptoutput    /* build/results/smoke.transcript   */
        {
            util::filename_concatenate
            (
                s_test_result_directory,
                util::file_extension_set(midifile, ".transcript")
            )
        };
        result = util::make_directory_path(s_test_result_directory);
        if (result)
            result = convert_midi_to_transcript(midiinput, transcriptoutput);

        if (result)
        {
            /*
             * Compare build/results/smoke.transcript to
             * tests/data/mftranscript/smoke.transcript
             */

            std::string transcript_golden
            {
                util::filename_concatenate
                (
                    s_test_input_transcripts,
                    util::file_extension_set(midifile, ".transcript")
                )
            };
            std::string msg
            {
                "Comparing output " + transcriptoutput +
                " to golden file " + transcript_golden
            };
            util::status_message(msg);
            result = util::compare_line_files
            (
                transcriptoutput, transcript_golden
            );
            if (result)
                util::status_message("Success!");
            else
                util::error_message("Failure!");
        }
        if (result)
        {
            std::string midioutput      /* build/results/smoke.mid          */
            {
                util::filename_concatenate(s_test_result_directory, midifile)
            };

            /*
             * tests/data/mftranscript/smoke.transcript
             */

            std::string transcriptinput
            {
                util::filename_concatenate
                (
                    s_test_result_directory,
                    util::file_extension_set(midifile, ".transcript")
                )
            };
            result = convert_transcript_to_midi(transcriptinput, midioutput);
            if (result)
            {
                /*
                 * Compare build/results/smoke.mid
                 * tests/data/midi/smoke.mid
                 *
                 * We have to get the proper file extension from the MIDI file
                 * to use in the test.
                 */

                std::string ext { util::file_dot_extension(midifile) };
                std::string midi_golden
                {
                    util::filename_concatenate
                    (
                        s_test_input_midi,
                        util::file_extension_set(midifile, ext)
                    )
                };
                std::string msg
                {
                    "Comparing output " + midioutput +
                    " to golden file " + midi_golden
                };
                util::status_message(msg);
                result = util::compare_binary_files(midioutput, midi_golden);
                if (result)
                    util::status_message("Success!");
                else
                    util::error_message("Failure!");
            }
        }
    }
    return result;
}

}           // namespace anonymous

/**
 *  The main routine.
 */

int
main (int argc, char * argv [])
{
    int rcode { EXIT_FAILURE };

    if (argc > 1)
    {
        std::string midifile { std::string(argv[1]) };
        bool result { process_midi_file(midifile) };
        if (result)
        {
            std::cout << "mftranscript test succeeded" << std::endl;
            rcode = EXIT_SUCCESS;
        }
        else
            std::cout << "mftranscript test failed" << std::endl;
    }
    else
    {
        // TODO: use a list of files
    }
    return rcode;
}

/*
 * mftranscript.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */
