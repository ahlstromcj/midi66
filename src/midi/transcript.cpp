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
 * \file          transcript.cpp
 *
 *  This module declares/defines the base class for MIDI files.
 *
 * \library       midi66
 * \author        Chris Ahlstrom
 * \date          2026-11-07
 * \updates       2026-11-14
 * \license       GNU GPLv2 or above
 *
 *  The format of text-based MIDI transcript file is as follows, and the
 *  "magic" line "Midi66TextTune" is checked for validity.
 *
 *      %Midi66 Transcript
 *      MThd smf=1 tracks=1 ppqn=192
 *      MTrk bytes=76
 *      0 Meta 0xFF 0x03 0x09 0x54 0x65 0x6D 0x31 0x30 # Seq Name
 *      0 Meta 0xFF 0x59 0x02 0x00 0x00 # Key Signature
 *      0 Meta 0xFF 0x58 0x04 0x04 0x02 0x18 0x08 # Time Signature
 *      0 Meta 0xFF 0x51 0x03 0x07 0x62 0x1E # Set Tempo
 *      0 ProgChange chan=0 prog=35 # Fretless Bass
 *      0 CtrlChange chan=0 ctrl=7 level=120 # Volume
 *      768 Meta 0xFF 0x2F 0x00 # End of Track
 *
 *      # vim: ft=sh
 *
 *  Blank lines and comments ("# etc.") are ignored, and newlines are
 *  stripped. See cfg66's util::file_read_lines() function.
 */

#include <fstream>                      /* std::ifstream & std::ofstream    */
#include <memory>                       /* std::unique_ptr<>                */

#include "midi/eventspecs.hpp"          /* midi::midi_event_to_string() etc */
#include "midi/song.hpp"                /* midi::song class ("tune")        */
#include "midi/transcript.hpp"          /* midi::transcript R/W class       */
#include "util/filefunctions.hpp"       /* util::file_read_lines(), etc     */
#include "util/msgfunctions.hpp"        /* util::status_message(), etc      */
#include "util/strfunctions.hpp"        /* util::starts_with(), etc         */

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

transcript::transcript
(
    const std::string & filespec,
    song & s
) :
    midi::filebase  (filespec, s),
    m_lines         ()
{
    s.file_name(filespec);
}

/**
 *  A rote destructor.
 */

transcript::~transcript ()
{
    // empty body
}

bool
transcript::parse (const std::string & tag)
{
    (void) tag;

    bool result { util::file_read_lines(file_spec(), lines(), true) };
    if (result)
    {
        result = lines().size() >= 4;
        if (result)
        {
            result = lines()[0] == magic_string();
            if (! result)
            {
                std::string msg { "Unmatched magic string '" };
                msg += magic_string();
                msg += "'!";
                (void) set_error(msg);
            }
        }
        if (result)
        {
            result = util::starts_with(lines()[1], tune().MThdString());
            if (! result)
            {
                std::string msg { tune().MThdString() + "not present!" };
                (void) set_error(msg);
            }
        }
        if (result)
        {
            int smffmt, trackct, ppq;
            string_to_midi_header(lines()[1], smffmt, trackct, ppq);
            tune().set_song_header(smffmt, trackct, ppq);

            midi::track * trk { nullptr };              /* a raw pointer    */
            int trkcounter { 0 };                       /* 1st trk pending  */
            int linecounter { 0 };                      /* to skip lines    */
            for (const auto & eventline : lines())
            {
                if (linecounter < 2)                  /* skip 1st, 2nd    */
                {
                    ++linecounter;
                    continue;
                }
                if (util::starts_with(eventline, "MTrk"))
                {
                    /*
                     * INVESTIGATE using song::new_track() instead.
                     */

                    trk = create_track();
                }
                else
                {
                    midi::event ev = string_to_midi_event(eventline);
                    if (ev.is_valid())
                    {
                        if (not_nullptr(trk))           /* safety first!    */
                        {
                            trk->add_event(ev);
                        }
                        else
                        {
                            std::string msg { "Track " };
                            msg += std::to_string(trkcounter);
                            msg += "failed!";
                            (void) set_error(msg);
                            break;                      /* what to do here? */
                        }

                        if (ev.is_end_of_track())
                        {
                            /*
                             * We finalize the track which installs the
                             * pointer in the song [tune()]. Note: it is
                             * imperative that we do not delete the pointer;
                             * it will be installed into a container of
                             * shared pointers. We do null it out to
                             * indicate it is void.
                             */

                            if (finalize_track(trk, trkcounter))
                            {
                                ++trkcounter;
                                trk = nullptr;
                            }
                            else
                            {
                                delete trk;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    return result;
}

bool
transcript::write (bool eventsonly)
{
    (void) eventsonly;

    lib66::tokenization all_lines;
    all_lines.push_back(magic_string());
    all_lines.push_back("\n");
    int smffmt, trackct, ppq;
    tune().get_song_header(smffmt, trackct, ppq);

    std::string hdr { midi::midi_header_to_string(smffmt, trackct, ppq) };
    all_lines.push_back(hdr);
    all_lines.push_back("\n");

    const tracklist & trklist { tune().track_list() };
    const tracklist::container & tracks { trklist.tracks() };
    bool result { tracks.size() > 0 };
    for (const midi::track::pointer & trk : tracks)
    {
        lib66::tokenization trkevents { midi_track_to_lines(*trk) };
        for (const auto & line : trkevents)
            all_lines.push_back(line);

        all_lines.push_back("\n");
    }
    all_lines.push_back("# vim: ft=sh\n");

#if defined PLATFORM_DEBUG_TMI
    printf("SONG DUMP\n");
    for (const auto & line : all_lines)
        printf("%s", CSTR(line));           /* line already has a newline   */
#endif

    result = util::file_write_lines(file_spec(), all_lines);
    return result;
}

bool
transcript::set_error (const std::string & msg)
{
    std::string errmsg { file_spec() };
    errmsg += ": ";
    errmsg += msg;
    return filebase::set_error(errmsg);
}

/*-------------------------------------------------------------------------
 * Free functions
 *-------------------------------------------------------------------------*/

/**
 *  Read an existing "*.transcript" file into a song object.
 *
 *  If desired, one can use song::print_tracks() to verify visually
 *  the results of the read.
 */

bool
read_midi_transcript (const std::string & transcriptfile, song & s)
{
    midi::transcript t(transcriptfile, s);
    bool result { t.parse("transcript") };
    if (result)
    {
        util::status_message("Success", transcriptfile);
    }
    else
    {
        util::error_message(t.error_message());
    }
    return result;
}

/**
 *  This function assumes that a song object has been filled by a function
 *  such as: song::read_midi_file() [see to_text_test() in the spectest
 *  program.]
 */

bool
write_midi_transcript (const std::string & transcriptfile, const song & s)
{
    song & nc_s { const_cast<song &>(s) };      /* hmmmmmmmm */
    midi::transcript t(transcriptfile, nc_s);
    bool result { t.write() };
    if (result)
    {
        util::status_message("Success", transcriptfile);
    }
    else
    {
        util::error_message(t.error_message());
    }
    return result;
}

#if defined USE_SUPPLEMENTAL_QRPNFRAME_CODE


    midimacros m_macro_events;

    midimacro read_midi_data (const std::string & fn)
    {
        return m_macro_events.read_midi_data(fn);
    }

/**
 *      "Sysex file (*.syx *.sysex)"
 *      ";;Raw file (*.raw)"
 *      ";;Macro file (*.macro *.macros)"
 *
 * We don't need to save the data, which might be huge,
 * whether it's a ".syx" file or ".macro" file.
 *
 * We save the file specification instead.
 */

bool
slot_load_file (const std::string & selectedfile)
{
    bool result { ! selectedfile.empty() };
    if (result)
    {
        midimacros & mco { m_macro_events () };
        midimacro macdata { /*mco.*/ read_midi_data(selectedfile) };
        result = macdata.is_valid();
        if (result)
        {
            std::string base    // TODO: ALREADY SET IN read_midi_data()
            {
                filename_base(selectedfile, true)           /* nix .ext */
            };
            std::string s { "file: " };
            s += selectedfile;  // DITTO
            set_file_name(s);   //set_plaintext_msg(s);
            other_macro_tokens().clear();                   /* be safe  */
            other_macro_tokens().push_back(base);           /* macnam   */
            other_macro_tokens().push_back(s);              /* macfile  */

            /*
             * The "arbitrary macro". Then set the file-name and
             * the raw data buffer. The "other" vector will
             * contain the macro-name and the file-name for use
             * in saving the data.
             */

            macro_filename(s);
        }
    }
}

/**
 *  Whenever we need the data from a macro with a value that is
 *  a filename, we need to read the file.
 *
 *      "Sysex file (*.syx)"
 *      ";;Macro file (*.macro)"
 */

bool
slot_save_file (const std::string & selectedfile)
{
    bool result { ! selectedfile.empty() };
    if (result)
    {
        midicontrolout & mco { perf().midi_control_out() };
        if (other_macro_in_force())
        {
            midimacro mac(other_macro_tokens());
            std::string macnam { macro_name() };
            midibytes byts { mco.expand_midimacro(mac) };
            int sz { int(byts.size()) };
            std::string bcs { std::to_string(sz) };
            ui->line_edit_byte_count->setText(qt(bcs));
            result = sz > 0;
            if (result)
            {
                mac.name(macnam);
                result = mco.write_midi_data(mac, selectedfile);
            }
        }
        else
        {
            /*
             * TODO: The macro is not yet added to the mco unless we
             * do a create-macro operation. A mco.expand_macro(macnam)
             * is not possible until then. Unless we write a function
             * to use the.....
             */

            rpn r(rpn_info());
            std::string macnam { macro_name() };
            int macchannel { m_rpn_channel };
            tokenization name_and_data
            {
                r.create_rpn_macro_string(macnam, macchannel)
            };
            if (name_and_data.size() < 2)
                return;

            r.name(macnam);

            /*
             * Usually the macro is already expanded, and this
             * call has no effect, unless that macro is not in
             * the 'ctrl' file.
             *
             * (void) mco.expand_macro(macnam);
             */

            (void) mco.expand_midimacro(r);
            result = mco.write_midi_data(r, selectedfile);
        }
        if (! result)
            set_plaintext_msg("Error sending macro");
    }
    s_show_slot(__FUNCTION__);
}

More to consider:

qrpnframe::slot_create_macro ()
qrpnframe::set_macro_name (const QString & name)
qrpnframe::slot_delete_macro ()
qrpnframe::modify_macro ()
qrpnframe::send_rpn_macro ()
qrpnframe::send_other_macro ()
qrpnframe::insert_rpn_macro ()
qrpnframe::insert_other_macro ()
qrpnframe::insert_file_macro ()

#endif      // defined USE_SUPPLEMENTAL_QRPNFRAME_CODE

}           // namespace midi

/*
 * transcript.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */
