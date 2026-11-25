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
 * \file          mftranscript.cpp
 *
 *      Simple program to test text-to-event-to-text conversions of
 *      MIDI messages (events).
 *
 * \application   mftranscript
 * \library       midi66
 * \author        Chris Ahlstrom
 * \date          2026-11-11
 * \updates       2026-11-21
 * \license       See above.
 */

#include <iostream>                     /* std::cout and std::cerr          */
#include <utility>                      /* std::pair template class         */
#include <vector>                       /* std::vector template class       */

#include "mftranscript.hpp"             /* configuration and options        */
#include "cfg/options.hpp"              /* cfg::options class               */
#include "midi/eventspecs.hpp"          /* midi:: spec conversion functions */
#include "midi/file.hpp"                /* midi::file class                 */
#include "midi/song.hpp"                /* midi::song class                 */
#include "midi/transcript.hpp"          /* midi::transcript class           */
#include "util/filefunctions.hpp"       /* util::file_extension()           */
#include "util/msgfunctions.hpp"        /* util::investigate()              */
#include "util/strfunctions.hpp"        /* util::string_to_int()            */

namespace   // anonymous
{

/**
 *  Converts a Midi66 transcript file to MIDI.
 *
 * \param transcriptfile
 *      A
 */

bool
convert_transcript_to_midi
(
    const std::string & transcriptfile,
    const std::string & midifile,
    bool verbose = false
)
{
    midi::song s;
    bool result { midi::read_midi_transcript(transcriptfile, s) };
    if (result)
    {
        if (verbose)
        {
            const midi::tracklist & trklist { s.track_list() };
            const midi::tracklist::container trkcontainer { trklist.tracks() };
            int smffmt, trackct, ppq;
            s.get_song_header(smffmt, trackct, ppq);

            std::string miditext
            {
                midi::midi_header_to_string(smffmt, trackct, ppq)
            };
            miditext += "\n";
            for (const auto & trk : trkcontainer)
            {
                std::string trkstring { midi_track_to_string(*trk) };
                miditext += trkstring;
            }
            std::cout
                << miditext
                << "----------------------"
                << std::endl
                ;
        }
        std::string errmsg;
        result = midi::write_midi_file(s, midifile, errmsg);
        if (! result)
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
    const std::string & transcriptfile,
    bool verbose
)
{
    midi::song s;
    std::string errmsg;
    bool result { midi::read_midi_file(s, midifile, errmsg) };
    if (result)
    {
        result = midi::write_midi_transcript(transcriptfile, s);
        if (verbose)
        {
            std::string tscript { util::file_read_string(transcriptfile) };
            if (! tscript.empty())
                std::cout << tscript << std::endl;
        }
        if (! result)
            util::error_message(midifile, errmsg);
    }
    if (! result)
    {
        util::error_message("Read failed", midifile);
    }
    return result;
}

bool
process_input_file
(
    const std::string & infile,
    bool verbose = false,
    const std::string & outfile = ""
)
{
    bool result { false };
    bool input_is_transcript { true };
    std::string ext { util::file_extension(infile) };
    std::string outputfile { outfile.empty() ? infile : outfile };
    if (ext == "transcript")
    {
        outputfile = util::file_extension_set(outputfile, ".midi");
        result = true;
    }
    else if (ext == "midi" || ext == "mid" || ext == "smf")
    {
        input_is_transcript = false;
        outputfile = util::file_extension_set(outputfile, ".transcript");
        result = true;
    }
    else
        util::error_message("Invalid file extension", infile);

    if (result)
    {
        if (input_is_transcript)
            result = convert_transcript_to_midi(infile, outputfile, verbose);
        else
            result = convert_midi_to_transcript(infile, outputfile, verbose);
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
    cli::parser clip { s_app_options };
    bool success { clip.parse(argc, argv) };
    if (success)
        success = cfg::initialize_appinfo(s_application_info, argv[0]);

    if (success)
    {
        bool show_info { clip.show_information_only() };
        bool can_run { ! show_info };
        if (can_run)
        {
            cfg::options & optset { clip.option_set() };
            bool use_plain { optset.boolean_value("plain") };
            if (! use_plain)
                use_plain = ! optset.boolean_value("fancy");

            midi::set_spec_fancy(! use_plain);
            util::status_message("Transcript mode", midi::spec_mode());
            if (clip.verbose_request())
            {
                std::cout << "Verbose operation" << std::endl;
            }
            if (clip.inspect_request())
            {
                util::error_message("--inspect unsupported");
                success = false;
            }
            if (clip.investigate_request())
            {
                util::error_message("--investigate unsupported");
                success = false;
            }
            if (clip.use_log_file())
            {
                std::cout
                    << "Using log file '" << clip.log_file() << "'"
                    << std::endl
                    ;
            }
            if (success)
            {
                if (argc > 1)
                {
                    /*
                     * TODO: we need a cli parser method to determine
                     * if the last argument is not an option AND
                     * not preceded by an option that has a parameter.
                     */

                    std::string lastarg { argv[argc - 1] };
                    if (lastarg[0] != '-')    /* last arg is not an option  */
                    {
                        std::string outfile { optset.value("output-file") };
                        if (outfile.empty())
                        {
                            success = process_input_file
                            (
                                lastarg, clip.verbose()
                            );
                        }
                        else
                        {
                            success = process_input_file
                            (
                                lastarg, clip.verbose(), outfile
                            );
                        }
                    }
                    else
                    {
                        success = false;
                        std::cerr << "A file-name is required." << std::endl;
                    }
                }
                else
                {
                    std::cout << s_help_intro << std::endl;
                }
            }
            if (success)
            {
                std::cout << "mftranscript succeeded" << std::endl;
                rcode = EXIT_SUCCESS;
            }
            else
                std::cerr << "mftranscript failed" << std::endl;
        }
        else
        {
            rcode = EXIT_SUCCESS;
            if (clip.help_request())        /* also shows the --help        */
            {
                std::cout << s_help_intro << std::endl;
            }
            if (clip.description_request())
            {
                std::cout << s_desc_intro << std::endl;
            }
            if (clip.version_request())
            {
                // Anything?
            }
        }
    }
    return rcode;
}

/*
 * mftranscript.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */
