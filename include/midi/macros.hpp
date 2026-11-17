#if ! defined MIDI66_MIDI_MACROS_HPP
#define MIDI66_MIDI_MACROS_HPP

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
 * \file          midi::macros.hpp
 *
 *  This module declares/defines the base class for handling MIDI control
 *  I/O of the application.
 *
 * \library       midi66 application
 * \author        C. Ahlstrom
 * \date          2021-11-22
 * \updates       2026-11-13
 * \license       GNU GPLv2 or above
 *
 *  ///// Provides the base class for midicontrolout. /////
 *
 * Warning:
 *
 *      It is NOT a base class for midicontrol or midicontrolin!
 */

#include <map>                          /* std::map container class         */

#include "midi/macro.hpp"               /* midi::macro class                */

namespace midi
{

/**
 *  Represents a string of midi::bytes and provides the infrastructure for
 *  reading them.
 */

class macros
{

public:

    static const std::string footer;
    static const std::string header;
    static const std::string reset;
    static const std::string startup;
    static const std::string shutdown;
    static const std::string macro_header;

    using container = std::map<std::string, midi::macro>;

private:

    /**
     *  This is a list of tokens making up the macro. Although it can take up
     *  extra space, it is useful to write the macro back to the configuration
     *  file. Also see the tokenize() function in the strfunctions module.
     */

    container m_macros;

    /**
     *  We need a way to not emit startup and exit macros if the user doesn't
     *  want that.
     */

    bool m_active;

public:

    macros ();
    macros (const midi::macros &) = default;
    macros & operator = (const midi::macros &) = default;
    macros (midi::macros &&) = default;
    macros & operator = (midi::macros &&) = default;
    ~macros () = default;

    void clear ()
    {
        m_macros.clear();
    }

    bool add (const lib66::tokenization & tokens);  /* data from ctrl file  */
    bool modify (const lib66::tokenization & tokens);
    bool remove (const std::string & macnam);
    bool find (const std::string & macnam) const;

    int count () const
    {
        return int(m_macros.size());
    }

    bool active () const
    {
        return m_active;
    }

    void active (bool flag)
    {
        m_active = flag;
    }

    bool expand ();
    bool expand (const std::string & name);
    midi::bytes expand_macro (midi::macro & m);
    midi::bytes bytes (const std::string & name) const;
    const midi::macro & macro (const std::string & name) const;
    std::string lines () const;
    lib66::tokenization names () const;
    std::string byte_strings () const;
    bool make_defaults ();
    midi::macro read_midi_data (const std::string & fn);
    midi::macro read_macro_file (const std::string & fn);
    bool get_midi_data
    (
        midi::macro & mac,
        const std::string & fn = ""
    );
    bool write_midi_data
    (
        midi::macro & macro,
        const std::string & fn = ""
    );
    bool write_macro_file
    (
        const midi::macro & macro,
        const std::string & fn = ""
    );

private:

    midi::macro::events expand_events (midi::macro & m);

};          // class midi::macros

}           // namespace midi

#endif      // MIDI66_MIDI_MACROS_HPP

/*
 * macros.hpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */
