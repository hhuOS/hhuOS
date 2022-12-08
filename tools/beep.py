# Copyright (C) 2018-2022 Heinrich-Heine-Universitaet Duesseldorf,
# Institute of Computer Science, Department Operating Systems
# Burak Akguel, Christian Gesse, Fabian Ruhland, Filip Krakowski, Michael Schoettner
#
# This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public
# License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any
# later version.
#
# This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied
# warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
# details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>

# Code to convert piano letter notes in text, playable by the 'music' command in hhuOS.
# https://pianoletternotes.blogspot.com/
#
# Original code by Nathan Reihs (nathan.reihs@gmail.com)

from itertools import chain
import argparse
import distutils
from distutils import util
import requests
import re

A4 = 440
STEP = 2 ** (1 / 12)
KEYS = "AbcCdDefFgGa"
DISTANCES = [0, 1, -10, -9, -8, -7, -6, -5, -4, -3, -2, -1]
STEPS = [STEP ** i for i in DISTANCES]
XML_REGEX = re.compile(r'<[^>]+>')


def convert_notes_to_string(notes, limit=0):
    """
    Takes a list of notes as (frequency, ms) tuples and converts them into a string,
    that is playable by the 'music' command in hhuOS.
    """
    if limit == 0:
        limit = float('inf')
    time = 0
    string = []

    for frequency, length in notes:
        string.append(f"{int(frequency)},{int(length)}")
        time += length
        if time > limit:
            break

    return "\n".join(string)


def frequency(note, octave):
    """
    Returns frequency for piano key s.
    A key is encoded as a 2 character string.
    The first is which note is played with a-g for whole notes and A,C,D,F,G for sharp notes.
    The second is a number specifying the octave.
    """
    base = A4
    index = KEYS.index(note)
    return base * 2 ** (octave - 4) * STEPS[index]


def normalize(line):
    """
    Normalize Strings from pianoletternotes.blogspot.com.
    Takes a list of strings, matching the hand movements of letter notes.
    Returns a list of tuples like ('a',4) or ('-').
    """
    s = []
    while line[0] not in "0123456789":
        line = line[1:]
    octave = int(line[0])

    for x in line[2:-1]:
        if x == "-":
            s.append(x)
        else:
            s.append((x, octave))

    return s


def merge_note_string(ls):
    """
    Merges multiple octaves, so that only played notes remain.
    If multiple notes are played at the same time, only the highest is chosen.
    """
    merged = []
    ls = list(map(normalize, ls))

    for i in range(26):
        best = "-"
        for x in ls:
            if best == "-":
                best = x[i]
            elif x[i] != "-":
                best = x[i] if frequency(*x[i]) > frequency(*best) else best
        merged.append(best)

    return merged


def to_frequency_and_length(merged, interval):
    """
    Takes a list of notes and a time interval.
    Returns a list of tuples (frequency, duration in ms), representing the notes.
    The given time interval is minimum interval a note is played
    (e.g. 'a' will be played exactly for one interval, 'aa' for two, etc.).
    '-' will be ignored, meaning that a note will be played until the next one follows.
    For example, ['A4','-','-','b4'] would result in [(440,ts*3), (440+2**12,ts)].

    This method is not well-suited for songs with pauses. These work better with
    """
    c = 0
    base = None
    ls = []
    for x in merged:
        if x == '-':
            c += interval
        elif base is not None:
            ls.append((frequency(*base), c))
            c = interval
            base = x
        else:
            base = x
    return ls


def to_frequency_and_length_pauses(merged, interval):
    """
    Similar to 'to_frequency_and_length', but '-' are interpreted as pauses.
    A pause is coded as (0, time interval).
    """
    base = 0
    ls = []
    for x in merged:
        if x == '-':
            if base is None:
                base = interval
            else:
                base += interval
        else:
            if base:
                ls.append((0, base))
                base = None
            ls.append((frequency(*x), interval))
    return ls


def parse_letter_notes(s, pauses, interval):
    """
    Wrapper method, that parses the raw string from https://pianoletternotes.blogspot.com into a list of (frequency, ms) tuples.
    """
    # split into lists of lines played at the same time
    s = map(lambda x: x.split("\n"), s.split("\n\n"))
    # merge simultaneous lines, taking higher note if multiple notes are played at the same time
    s = list(map(merge_note_string, s))
    # concat merged lines into one complete song
    s = list(chain(*s))

    return to_frequency_and_length_pauses(s, interval) if pauses else to_frequency_and_length(s, interval)


parser = argparse.ArgumentParser(prog='music', formatter_class=argparse.ArgumentDefaultsHelpFormatter)
parser.add_argument('-i', '--input', dest='input_url', required=True, help='Link to the letter notes page')
parser.add_argument('-p', '--pauses', dest='pauses', default='false', help='Whether to use pauses or hold a note')
parser.add_argument('-l', '--limit', dest='limit', default='0', help='Maximum length in seconds')
parser.add_argument('-t', '--time', dest='interval', default='125', help='Time interval length')

args = parser.parse_args()

# Get letter notes from the web page
raw_page = requests.get(args.input_url).content.decode('utf-8')
raw_notes = raw_page.split("<code>")[1].split("</code>")[0]
notes = XML_REGEX.sub('', raw_notes).strip()

# Convert the raw letter notes into a frequency-length list
fl_list = parse_letter_notes(notes, bool(distutils.util.strtobool(args.pauses)), int(args.interval))

# Convert the frequency-length list into a string, playable by the 'music' command in hhuOS
print(convert_notes_to_string(fl_list, int(args.limit)))
