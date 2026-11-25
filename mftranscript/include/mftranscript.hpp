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
 * \file          mftranscript.hpp
 *
 *      This file provides declarations, and shold be #included in
 *      mftranscript.cpp.
 *
 * \application   mftranscript
 * \library       midi66
 * \author        Chris Ahlstrom
 * \date          2026-11-11
 * \updates       2026-11-25
 * \license       See above.
 *
 */

#include <iostream>                     /* std::cout and std::cerr          */
#include <utility>                      /* std::pair template class         */
#include <vector>                       /* std::vector template class       */

#include "cfg/appinfo.hpp"              /* cfg::set_client_name()           */
#include "cli/parser.hpp"               /* cli::parser, etc.                */

namespace   // anonymous
{

/*
 * Application information.
 */

cfg::appinfo s_application_info
{
    cfg::appkind::test,                 // "test"
    "mftranscript",                     // _app_name (mandatory!)
    "0.1",                              // _app_version
    "[mf]",       // to do?             // _main_cfg_section_name
    "",                                 // _home_cfg_directory
    "mf.bogus",                         // _home_cfg_file

    /*
     * This client name is shown in square brackets in status messages
     * written to the terminal.
     */

    "mftranscript",                     // _client_name (fake session wart)
    "",                                 // _app_tag
    "",                                 // _arg_0
    "mftranscript",                     // _package_name
    "",                                 // _session_tag
    "",                                 // _app_icon_name (empty by default)
    "",                                 // _app_version_text derived
    "",                                 // _api_engine (empty by default)
    "0.1",                              // _api_version (empty by default)
    "",                                 // _gui_version (bogus here)
    "mftranscript",                     // _client_name_short
    "tag"                               // _client_name_tag
};

/**
 *  Options.
 */

cfg::options::container s_app_options   /* std::map<string, spec>           */
{
    /*
     *  Name
     *      Code,  Kind, Enabled,
     *      Default, Value, FromCli, Dirty (modified),
     *      Description, Built-in (global)
     */

    {
        "input",
        {
            'i', cfg::options::kind::boolean, cfg::options::enabled,
            "false", "", false, false,
            "If specified, the file will be read.",
            false
        }
    },
    {
        "output-file",
        {
            'o', cfg::options::kind::string, cfg::options::enabled,
            "", "", false, false,
            "Specifies the file name for output. If desired, the path "
            "should be included.",
            false
        }
    },
    {
        "plain",
        {
            cfg::options::code_null, cfg::options::kind::boolean,
            cfg::options::enabled,
            "false", "false", false, false,
            "Specifies to use the original plain (i.e. mostly numeric) "
            "format when creating a transcript.",
            false
        }
    },
    {
        "fancy",
        {
            cfg::options::code_null, cfg::options::kind::boolean,
            cfg::options::enabled,
            "true", "true", false, false,
            "Specifies to use the new fancy (i.e. less numeric) "
            "format when creating a transcript. This is the default.",
            false
        }
    },
};

/*
 * Explanation text.
 */

const std::string s_help_intro
{
    "The mftranscript program converts between MIDI files and the 'transcript'\n"
    "format. That format is a human-readable text representation akin to the \n"
    "text formats of other such converts.\n"
    "\n"
    "Usage:\n"
    "\n"
    "    mftranscript [options] tune.transcript\n"
    "\n"
    "       Read tune.transcript and generate tune.midi from it.\n"
    "\n"
    "    mftranscript [options] tune.midi (or tune.mid)\n"
    "\n"
    "       Read tune.midi and generate tune.transcript from it.\n"
    "\n"
    "    mftranscript [options] --output-file draft.midi tune.transcript\n"
    "    mftranscript [options] --output-file draft.transcript tune.mid\n"
    "\n"
    "       Change the name of the output file.\n"
    "\n"
    "Note that not all basic options are supported by this application.\n"
};

const std::string s_desc_intro
{
    "This application does the conversion, in both directions, between\n"
    "MIDI events and a brief text representation similar to midicvt and\n"
    "other midi-to-text converters. Add --verbose to see the results, or\n"
    "a summary, of the data.\n"
};

}           // namespace anonymous

/*
 * mftranscript.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */
