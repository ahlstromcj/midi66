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
 * \file          controllers.cpp
 *
 *  This module defines the array of MIDI controller names.
 *
 * \library       midi66 library
 * \author        Chris Ahlstrom
 * \date          2015-12-06
 * \updates       2026-10-20
 * \license       GNU GPLv2 or above
 *
 *  This definition used to reside in the controllers.hpp file, but now more
 *  than one module uses it, so we have to define it here to avoid a linker
 *  error about multiple definitions of this item. Plus, it's the right
 *  thing to do :-).
 */

#include "cpp_types.hpp"                /* lib66::tokenization              */
#include "midi/controllers.hpp"         /* midi::controller_name(), etc.    */
#include "util/strfunctions.hpp"        /* util::tokenize()                 */

namespace midi
{

using namepair = struct
{
    int number;
    std::string name;
};

/**
 *  Provides the default names of MIDI controllers.  This array is used
 *  only by the functions below.
 */

static namepair
s_controller_names [c_byte_data_max] =
{
    {   0, "Bank Select"                          },    // 0x00
    {   1, "Modulation Wheel"                     },
    {   2, "Breath controller"                    },
    {   3, "---"                                  },
    {   4, "Foot Pedal"                           },
    {   5, "Portamento Time"                      },
    {   6, "Data Entry Slider"                    },
    {   7, "Volume"                               },
    {   8, "Balance"                              },
    {   9, "---"                                  },
    {  10, "Pan position"                         },
    {  11, "Expression "                          },
    {  12, "Effect Control 1 "                    },
    {  13, "Effect Control 2 "                    },
    {  14, "---"                                  },
    {  15, "---"                                  },
    {  16, "General Purpose Slider 1"             },    // 0x10
    {  17, "General Purpose Slider 2"             },
    {  18, "General Purpose Slider 3"             },
    {  19, "General Purpose Slider 4"             },
    {  20, "---"                                  },
    {  21, "---"                                  },
    {  22, "---"                                  },
    {  23, "---"                                  },
    {  24, "---"                                  },
    {  25, "---"                                  },
    {  26, "---"                                  },
    {  27, "---"                                  },
    {  28, "---"                                  },
    {  29, "---"                                  },
    {  30, "---"                                  },
    {  31, "---"                                  },
    {  32, "Bank Select (fine)"                   },    // 0x20
    {  33, "Modulation Wheel (fine)"              },
    {  34, "Breath controller (fine)"             },
    {  35, "---"                                  },
    {  36, "Foot Pedal (fine)"                    },
    {  37, "Portamento Time (fine)"               },
    {  38, "Data Entry (fine)"                    },
    {  39, "Volume (fine)"                        },
    {  40, "Balance (fine)"                       },
    {  41, "---"                                  },
    {  42, "Pan position (fine)"                  },
    {  43, "Expression (fine)"                    },
    {  44, "Effect Control 1 (fine)"              },
    {  45, "Effect Control 2 (fine)"              },
    {  46, "---"                                  },
    {  47, "---"                                  },
    {  48, "---"                                  },    // 0x30
    {  49, "---"                                  },
    {  50, "---"                                  },
    {  51, "---"                                  },
    {  52, "---"                                  },
    {  53, "---"                                  },
    {  54, "---"                                  },
    {  55, "---"                                  },
    {  56, "---"                                  },
    {  57, "---"                                  },
    {  58, "---"                                  },
    {  59, "---"                                  },
    {  60, "---"                                  },
    {  61, "---"                                  },
    {  62, "---"                                  },
    {  63, "---"                                  },
    {  64, "Hold Pedal (on/off)"                  },    // 0x40
    {  65, "Portamento (on/off)"                  },
    {  66, "Sustenuto Pedal (on/off)"             },
    {  67, "Soft Pedal (on/off)"                  },
    {  68, "Legato Pedal (on/off)"                },
    {  69, "Hold 2 Pedal (on/off)"                },
    {  70, "Sound Variation"                      },
    {  71, "Sound Timbre"                         },
    {  72, "Sound Release Time"                   },
    {  73, "Sound Attack Time"                    },
    {  74, "Sound Brightness"                     },
    {  75, "Sound Control 6"                      },
    {  76, "Sound Control 7"                      },
    {  77, "Sound Control 8"                      },
    {  78, "Sound Control 9"                      },
    {  79, "Sound Control 10"                     },
    {  80, "General Purpose Button 1 (on/off)"    },    // 0x50
    {  81, "General Purpose Button 2 (on/off)"    },
    {  82, "General Purpose Button 3 (on/off)"    },
    {  83, "General Purpose Button 4 (on/off)"    },
    {  84, "---"                                  },
    {  85, "---"                                  },
    {  86, "---"                                  },
    {  87, "---"                                  },
    {  88, "---"                                  },
    {  89, "---"                                  },
    {  90, "---"                                  },
    {  91, "Effects Level"                        },
    {  92, "Tremulo Level"                        },
    {  93, "Chorus Level"                         },
    {  94, "Celeste Level"                        },
    {  95, "Phaser Level"                         },
    {  96, "Data Button Increment"                },    // 0x60
    {  97, "Data Button Decrement"                },
    {  98, "Non-registered Parameter (fine)"      },
    {  99, "Non-registered Parameter (coarse)"    },
    { 100, "Registered Parameter (fine)"          },
    { 101, "Registered Parameter (coarse)"        },
    { 102, "---"                                  },
    { 103, "---"                                  },
    { 104, "---"                                  },
    { 105, "---"                                  },
    { 106, "---"                                  },
    { 107, "---"                                  },
    { 108, "---"                                  },
    { 109, "---"                                  },
    { 110, "---"                                  },
    { 111, "---"                                  },
    { 112, "---"                                  },    // 0x70
    { 113, "---"                                  },
    { 114, "---"                                  },
    { 115, "---"                                  },
    { 116, "---"                                  },
    { 117, "---"                                  },
    { 118, "---"                                  },
    { 119, "---"                                  },
    { 120, "All Sound Off"                        },
    { 121, "All Controllers Off"                  },
    { 122, "Local Keyboard On/Off"                },
    { 123, "All Notes Off"                        },
    { 124, "Omni Mode Off"                        },
    { 125, "Omni Mode On"                         },
    { 126, "Mono On"                              },
    { 127, "Poly On"                              }     // 0x7F
};

/**
 *  Handles these numbers in /usr/include/alsa/asoundef.h:
 *
 *      MIDI_CTL_MSB_BANK to MIDI_CTL_MONO2
 *
 * \param index
 *      The controller number.
 *
 * \param usehex
 *      If true (the default is false), the index is converted to
 *      hexadecimal.
 *
 * \return
 *      Returns the controller number and name in one string.
 */

std::string
controller_name (int index, bool usehex)
{
    std::string result;
    if (index >= 0 && index < c_byte_data_max)
    {
        std::string name { s_controller_names[index].name };
        if (usehex)
        {
            char tmp[8];
            (void) snprintf(tmp, sizeof tmp, "0x%02x", index);
            result = tmp;
        }
        else
        {
            char tmp[8];
            (void) snprintf(tmp, sizeof tmp, "%3d", index);
            result = tmp;
        }
        result += " ";
        result += name;
    }
    return result;
}

void
set_controller_name (int index, const std::string & newname)
{
    if (index >= 0 && index < c_byte_data_max)
        s_controller_names[index].name = newname;
}

using rpnpair = struct
{
    midi::ushort number;
    std::string name;
};

/*--------------------------------------------------------------------------
 * Control abbreviation support
 *
 * These names are not meant for readability, but for parsing.
 *
 *--------------------------------------------------------------------------*/

using ctrlcontainer = std::map<midi::byte, std::string>;

static ctrlcontainer s_controller_abbreviations
{
    {   0, "Bank"              },    // 0x00
    {   1, "ModWheel"          },
    {   2, "Breath"            },
    {   3, "3"                 },
    {   4, "FootPedal"         },
    {   5, "Portamento"        },
    {   6, "Data"              },
    {   7, "Volume"            },
    {   8, "Balance"           },
    {   9, "9"                 },
    {  10, "Pan"               },
    {  11, "Express"           },
    {  12, "Efx_1"             },
    {  13, "Efx_2"             },
    {  14, "14"                },
    {  15, "15"                },
    {  16, "Slider_1"          },    // 0x10
    {  17, "Slider_2"          },
    {  18, "Slider_3"          },
    {  19, "Slider_4"          },
    {  20, "20"                },
    {  21, "21"                },
    {  22, "22"                },
    {  23, "23"                },
    {  24, "24"                },
    {  25, "25"                },
    {  26, "26"                },
    {  27, "27"                },
    {  28, "28"                },
    {  29, "29"                },
    {  30, "30"                },
    {  31, "31"                },
    {  32, "BankFine"          },    // 0x20
    {  33, "ModWheelFine"      },
    {  34, "BreathFine"        },
    {  35, "35"                },
    {  36, "FootpedalFine"     },
    {  37, "PortamentoFine"    },
    {  38, "DataFine"          },
    {  39, "VolumeFine"        },
    {  40, "BalanceFine"       },
    {  41, "41"                },
    {  42, "PanFine"           },
    {  43, "ExpressFine"       },
    {  44, "Efx_1_Fine"        },
    {  45, "Efx_2_Fine"        },
    {  46, "46"                },
    {  47, "47"                },
    {  48, "48"                },    // 0x30
    {  49, "49"                },
    {  50, "50"                },
    {  51, "51"                },
    {  52, "52"                },
    {  53, "53"                },
    {  54, "54"                },
    {  55, "55"                },
    {  56, "56"                },
    {  57, "57"                },
    {  58, "58"                },
    {  59, "59"                },
    {  60, "60"                },
    {  61, "61"                },
    {  62, "62"                },
    {  63, "63"                },
    {  64, "HoldOnOff"         },    // 0x40
    {  65, "PortamentoOnOff"   },
    {  66, "SustenutoOnOff"    },
    {  67, "SoftOnOff"         },
    {  68, "LegatoOnOff"       },
    {  69, "Hold_2_OnOff"      },
    {  70, "Variation"         },
    {  71, "Timbre"            },
    {  72, "Release"           },
    {  73, "Attack"            },
    {  74, "Brightness"        },
    {  75, "Control_6"         },
    {  76, "Control_7"         },
    {  77, "Control_8"         },
    {  78, "Control_9"         },
    {  79, "Control_10"        },
    {  80, "Button_1"          },    // 0x50
    {  81, "Button_2"          },
    {  82, "Button_3"          },
    {  83, "Button_4"          },
    {  84, "84"                },
    {  85, "85"                },
    {  86, "86"                },
    {  87, "87"                },
    {  88, "88"                },
    {  89, "89"                },
    {  90, "90"                },
    {  91, "EfxLevel"          },
    {  92, "TremuloLevel"      },
    {  93, "ChorusLevel"       },
    {  94, "CelesteLevel"      },
    {  95, "PhaserLevel"       },
    {  96, "DataIncr"          },    // 0x60
    {  97, "DataDecr"          },
    {  98, "NRPN_LSB"          },
    {  99, "NRPN_MSB"          },
    { 100, "RPN_LSB"           },
    { 101, "RPN_MSB"           },
    { 102, "102"               },
    { 103, "103"               },
    { 104, "104"               },
    { 105, "105"               },
    { 106, "106"               },
    { 107, "107"               },
    { 108, "108"               },
    { 109, "109"               },
    { 110, "110"               },
    { 111, "111"               },
    { 112, "112"               },    // 0x70
    { 113, "113"               },
    { 114, "114"               },
    { 115, "115"               },
    { 116, "116"               },
    { 117, "117"               },
    { 118, "118"               },
    { 119, "119"               },
    { 120, "SoundOff"          },
    { 121, "CtrlsOff"          },
    { 122, "KeyboardOnOff"     },
    { 123, "AllNotesOff"       },
    { 124, "OmniOff"           },
    { 125, "OmniOn"            },
    { 126, "MonoOn"            },
    { 127, "PolyOn"            }     // 0x7F
};

/**
 *  Handles these numbers in /usr/include/alsa/asoundef.h:
 *
 *      MIDI_CTL_MSB_BANK to MIDI_CTL_MONO2
 *
 * \param index
 *      The controller number.
 *
 * \return
 *      Returns the controller number and name in one string.
 */

std::string
controller_abbreviation (int index)
{
    std::string result { "Unknown" };
    const auto iter { s_controller_abbreviations.find(index) };
    if (iter != s_controller_abbreviations.end())
        result = iter->second;

    return result;
}

int
controller_number (const std::string & abbreviation)
{
    int result { -1 };
    for (const auto & a : s_controller_abbreviations)
    {
        if (a.second == abbreviation)
        {
            result = a.first;
            break;
        }
    }
    return result;
}

/*--------------------------------------------------------------------------
 * RPN support
 *--------------------------------------------------------------------------*/

static const int c_rpn_value_count { 9 };

/*
 * An MPE Configuration Message (MCM) is a specialized MIDI message used to
 * define and set up MIDI Polyphonic Expression (MPE) zones on compatible
 * hardware and software.
 */

static rpnpair
s_rpn_names [c_rpn_value_count]
{
    {   0x0000,     "Pitchbend range"               },
    {   0x0001,     "Channel fine Tuning"           },
    {   0x0002,     "Channel coarse Tuning"         },
    {   0x0003,     "Tuning program change"         },
    {   0x0004,     "Tuning bank select"            },
    {   0x0005,     "Modulation depth range"        },
    {   0x0005,     "MPE configuration message"     },      /* see above    */
    {   0x007F,     "RPN parameter reset"           },
    {   0x3FFF,     "RPN null"                      }       /* terminator?  */
};

std::string
rpn_name (int value)
{
    std::string result;
    for (auto p : s_rpn_names)
    {
        if (p.number == ushort(value))
        {
            result = p.name;
            break;
        }
    }
    return result;
}

/**
 *  Converts a 14-but RPN number to the MSB and LSB bytes.
 *
 *  Here is the process:
 *
 *  -   In binary, this is a 16-bit number.  0011111110000000.
 *  -   Ignore the two leading zeroes, i.e. it's a 14-bit number.
 *  -   Get the MSB.
 *          -   Get the next 7 bits.
 *          -   Prepend a 0.
 *  -   Get the LSB.
 *          -   Get the last 7 bits.
 *          -   Prepend a 0.
 *
 *        0MMMMMMM0LLLLLLL
 *                01111111 0x7F
 *
 *  Standard MIDI Files adopted the Motorola big-endian standard (also
 *  known as network byte order, which is most significant byte first).
 *
 * \param rpnn
 *      The 14-bit RPN number. It must be greater than zero and less
 *      than 16364 (0x4000).
 *
 * \return
 *      Returns the converted bytes. Holds the two bytes:
 *
 *          MSB: result[0]
 *          LSB: result[1]
 */

midi::bytes
rpn_number_to_bytes (midi::ushort rpnn)
{
    midi::bytes result;
    bool ok { rpnn < c_midishort_14_bad };
    if (ok)
    {
        midi::ushort rpnn_lsb { midi::ushort(rpnn & 0x7F) };
        midi::ushort rpnn_msb { midi::ushort((rpnn >> 7) & 0x7F) };
        result.push_back(midi::byte(rpnn_msb));
        result.push_back(midi::byte(rpnn_lsb));
    }
    return result;
}

/**
 *  Converts the first two bytes to a 14-bit short value.
 *
 * \param in
 *      A vector containing two bytes:
 *
 *          MSB: in[0]
 *          LSB: in[1]
 *
 * \return
 *  Returns the 14-bit value, or c_midishort_14_bad (16384) if an error
 *  occurred.
 */

midi::ushort
bytes_to_rpn_number (const midi::bytes & in)
{
    midi::ushort result { c_midishort_14_bad };  /* 0x4000 in midibytes.hpp  */
    if (in.size() > 1)
    {
        result = midi::ushort(in[0] & 0x7F);    /* the MSB 7 bits           */
        result <<= 7;                           /* multiply by 128          */
        result += midi::ushort(in[1]);          /* the LSB 7 bits           */
    }
    return result;
}

/**
 *  Converts a string to a 14-bit RPN number from 0 to 16383. Conversions
 *  outside this range are rejected. Formats:
 *
 *      -   "0x0" to "0x3FFF. Hexadecimal format.
 *      -   "0" to "16383". Decimal format.
 *      -   "12.2".  This format provides, not a decimal number, but
 *          two numbers, the MSB and LSB, which are combined into a
 *          14-bit value. Other formats are "12:2", "12/2", and "12 2".
 *          Hex digits can be used as well.
 *
 * \param s
 *      Provides the number(s) to be converted.
 *
 * \returns
 *      Returns the 14-bit number, or 16384 (c_midishort_14_bad) if
 *      there was a error.
 */

midi::ushort
string_to_rpn_number (const std::string & s)
{
    midi::ushort result { c_midishort_14_bad };
    lib66::tokenization t { util::tokenize(s, ".:/ ") };
    if (t.size() == 1)
    {
        int value { util::string_to_int(t[0], c_midishort_14_bad) };
        if (value >= 0 && value < int(c_midishort_14_bad))
            result = midi::ushort(value);
    }
    else if (t.size() == 2)             /* e.g. "12.0" --> { "12", "0" }    */
    {
        int msb { util::string_to_int(t[0], -1) };              /* "12"     */
        int lsb { util::string_to_int(t[1], -1) };              /* "0"      */
        bool ok { (msb >= 0 && msb < 0x40) && (lsb >= 0 && lsb < 0x100) };
        if (ok)
        {
            midi::bytes byts;
            byts.push_back(midi::byte(msb));                    /* byte 0   */
            byts.push_back(midi::byte(lsb));                    /* byte 1   */
            result = bytes_to_rpn_number(byts);
        }
    }
    return result;
}

}           // namespace midi

/*
 * controllers.cpp
 *
 * vim: sw=4 ts=4 wm=4 et ft=cpp
 */
