# README for Library Midi66 0.2 2026-11-17

__Midi66__ is a library for handling MIDI files and data. It derives
from __Seq66__ and __Rtl66__, the code being pulled out for usage in
those projects and other projects, in the future. It does not handle
MIDI playback and recording.

Also provided is the __mftranscript__ command-line application, yet
another MIDI-to-text converter. It is meant to fill in gaps in
other converters, provide more human-readable output, and help the
author in tracking MIDI file-handling issues in __Seq66__.

Support sites (still in progress):

    *   https://ahlstromcj.github.io/
    *   https://github.com/ahlstromcj/ahlstromcj.github.io/wiki

# Major Features

    *   A *calculations* module to centralize functions relataed to
        MIDI concepts
    *   Modules and classes for MIDI data types, controllers, drums,
        MIDI events and "editable events", patches, scales, RPN
        controllers, message bytes, measures, macros, and timing
        concepts.
    *   A MIDI *file* class; all classes are in the "midi" namespace.
    *   A *song* class that holds the MIDI header data and MIDI tracks.
    *   A *splitter* class to convert SMF 0 files into an SMF 1 file
        with multiple tracks.
    *   Classes for MIDI tracks and track data.
    *   The *eventspecs* module provides conversions between MIDI
        data items and text-based formats.
    *   The *transcript* class supports conversions between MIDI
        format and the "midi66" text format.
    *   The installable *mftranscript* application implements the
        MIDI-to-text and text-to-MIDI conversions. It is intended
        to eventually handle everything about MIDI, but is fairly
        comprehensive already.

##  Library Features

    *   Can be built using GNU C++ or Clang C++.
    *   Basic dependencies: Meson 1.4 and above; C++17 and above.
    *   The build system is Meson, and sample *wrap* files are provided
        for using Midi66 as a C++ subproject. [TO DO]
    *   PDF documentation built from LaTeX. [TO DO]

##  Code

    *   The code is a mix of hard-core C++ and C-like functions.
    *   Name spaces are used heavily. For example, a MIDI file is
        represented by a midi::file object.
    *   The C++ STL and advanced language features are used as much as
        possible.
    *   C++17 is required for some of its features.
    *   The GNU and Clang C++ compilers are supported.
    *   Many modules for easier maintenance.

##  Fixes

    *   Many things done to support new test applications.

##  Documentation

    *   A PDF developers guide is in progress. [TO DO]

## To Do

    *   Beef up testing.
    *   Start up the LaTeX documentation.

## Recent Changes

    For all changes, see the NEWS file.

    *   Version 0.1.0:
        *   The project is somewhat complete.

// vim: sw=4 ts=4 wm=2 et ft=markdown
