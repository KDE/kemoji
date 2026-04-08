#!/usr/bin/env python3
#
# Helper script to generate Emoji dictionary file from unicode data.
#
# SPDX-FileCopyrightText: 2022 Weng Xuetian <wengxt@gmail.com>
# SPDX-FileCopyrightText: 2026 James Graham <james.h.graham@protonmail.com>
# SPDX-License-Identifier: LGPL-2.0-or-later
#
from dataclasses import dataclass
from enum import Enum
import collections
import requests
import io
import zipfile
import os
import glob
import xml.dom.minidom
from PySide6.QtCore import QFile, QDataStream, QByteArray, QIODevice, qCompress

class Categories(Enum):
    No = 0
    All = 1
    Recent = 2
    Favorite = 3
    Custom = 4
    Smileys = 5
    People = 6
    Animals = 7
    Food = 8
    Travel = 9
    Activities = 10
    Objects = 11
    Symbols = 12
    Flags =13

categoryNames = {
    b"Smileys & Emotion": Categories.Smileys.value,
    b"People & Body": Categories.People.value,
    b"Animals & Nature": Categories.Animals.value,
    b"Food & Drink": Categories.Food.value,
    b"Travel & Places": Categories.Travel.value,
    b"Activities": Categories.Activities.value,
    b"Objects": Categories.Objects.value,
    b"Symbols": Categories.Symbols.value,
    b"Flags": Categories.Flags.value,
    b"Component": Categories.No.value,
}

EMOJI_VERSION = "16.0"
EMOJI_TEST_FILE = "emoji-test.txt"
EMOJI_TEST_URL = f"https://www.unicode.org/Public/emoji/{EMOJI_VERSION}/{EMOJI_TEST_FILE}"

CLDR_VERSION = "46.0"
CLDR_FILE = f"cldr-common-{CLDR_VERSION}.zip"
CLDR_URL = f"https://unicode.org/Public/cldr/{CLDR_VERSION.split('.')[0]}/{CLDR_FILE}"
CLDR_ANNOTATIONS_DIR = "common/annotations"
CLDR_ANNOTATIONS_DERIVED_DIR = "common/annotationsDerived"


def partsFromEmojiLine(line):
    segments = line.split(b";")
    if len(segments) != 2:
        raise ValueError("Input line is not in the right format, expected format: code points; status # emoji name.", line)

    codePoints = segments[0].strip().split(b" ")
    unicode = "".join(chr(int(code, 16)) for code in codePoints)

    metadata = segments[1].split(b"#")
    if len(metadata) != 2:
        raise ValueError("Input line is not in the right format, expected format: code points; status # emoji name.", line)

    idParts = metadata[1].strip().split(b" E", 1)[1].split(b" ", 1)
    id = idParts[1]

    return unicode, metadata[0].strip(), id

@dataclass
class Emoji(object):
   id: str
   unqualifiedUnicode: str
   category: str

class EmojiAnnotation(object):
    def __init__(self):
        self.name = ""
        self.altNames = []

class EmojiParser(object):
    def __init__(self):
        self.variantMapping = dict()
        self.emojis = collections.OrderedDict()

    def parseEmojiTest(self, emojiTestData):
        nameMapping = dict()
        currentCategory = Categories.No;
        GROUP_TAG = b"# group: "
        for line in emojiTestData.split(b"\n"):
            line = line.strip()
            if line.startswith(GROUP_TAG):
                currentCategory = categoryNames[line[len(GROUP_TAG):]]
                continue

            if line.startswith(b"#"):
                continue;

            # line format: code points; status # emoji name
            try:
                unicode, status, id = partsFromEmojiLine(line)
                if status == b"fully-qualified":
                    self.emojis[unicode] = Emoji(id, "", currentCategory)
                    nameMapping[id] = unicode
                else:
                    fullyQualified = nameMapping.get(id, None);
                    if fullyQualified:
                        self.emojis[fullyQualified].unqualifiedUnicode = unicode
                        self.variantMapping[unicode] = fullyQualified;
            except ValueError as e:
                pass

    def parseCldr(self, cldrList):
        annotations = dict()
        for data in cldrList:
            with xml.dom.minidom.parseString(data) as doc:
                annotationNodes = doc.getElementsByTagName("annotation")
                for annotationNode in annotationNodes:
                    if "cp" not in annotationNode.attributes:
                        continue
                    emoji = annotationNode.attributes["cp"].nodeValue
                    if emoji not in self.emojis:
                        emoji = self.variantMapping.get(emoji, None)
                    if not emoji:
                        continue
                    if len(annotationNode.childNodes) != 1 or annotationNode.childNodes[0].nodeType != xml.dom.minidom.Node.TEXT_NODE:
                        continue
                    if emoji not in annotations:
                        annotations[emoji] = EmojiAnnotation()
                    annotation = annotations[emoji]
                    if "type" in annotationNode.attributes and annotationNode.attributes["type"].nodeValue == "tts":
                        annotation.name = annotationNode.childNodes[0].nodeValue
                    else:
                        annotation.altNames = annotationNode.childNodes[0].nodeValue.split(" | ")
        return annotations

print("Removing old *.dict files")
for olddict in glob.glob("*.dict"):
    os.remove(olddict)

parser = EmojiParser()

print(f"Downloading {EMOJI_TEST_URL}")
response = requests.get(EMOJI_TEST_URL)
print(f"Parsing {EMOJI_TEST_FILE}")
parser.parseEmojiTest(response.content)

print(f"Downloading {CLDR_URL}")
response = requests.get(CLDR_URL)

with zipfile.ZipFile(io.BytesIO(response.content)) as thezip:
    annotationsFiles = set()
    annotationsDerivedFiles = set()
    for zipinfo in thezip.infolist():
        dirname = os.path.dirname(zipinfo.filename)
        basename = os.path.basename(zipinfo.filename)
        if not basename.endswith(".xml"):
            continue
        if dirname == CLDR_ANNOTATIONS_DIR:
            annotationsFiles.add(basename)
        elif dirname == CLDR_ANNOTATIONS_DERIVED_DIR:
            annotationsDerivedFiles.add(basename)

    files = annotationsFiles.intersection(annotationsDerivedFiles)
    for langfile in files:
        dictfilename = langfile[:-4] + ".dict"
        print(f"Generating {dictfilename}")
        annotations = dict()
        with thezip.open(os.path.join(CLDR_ANNOTATIONS_DIR, langfile)) as annotationsFile, thezip.open(os.path.join(CLDR_ANNOTATIONS_DERIVED_DIR, langfile)) as annotationsDerivedFile:
            annotations = parser.parseCldr([annotationsFile.read(), annotationsDerivedFile.read()])

        filtered_emojis = [(unicode, emoji) for (unicode, emoji) in parser.emojis.items() if unicode in annotations and annotations[unicode].name]
        # There's indeed some annotations file with 0 entries.
        if not filtered_emojis:
            print(f"Skipping {dictfilename}")
            continue

        dictfile = QFile(dictfilename)
        if not dictfile.open(QIODevice.WriteOnly):
            continue

        buf = QByteArray()
        stream = QDataStream(buf, QIODevice.WriteOnly)
        stream.writeUInt32(len(filtered_emojis))
        for unicode, emoji in filtered_emojis:
            stream.writeQString(unicode)
            stream.writeQString(emoji.unqualifiedUnicode)
            annotation = annotations[unicode]
            stream.writeQString(annotation.name)
            stream.writeInt32(emoji.category)
            # Write QList<QByteArray>
            stream.writeUInt32(len(annotation.altNames))
            for item in annotation.altNames:
                stream.writeQString(item)
        compressed = qCompress(buf)
        dictfile.write(compressed)
        dictfile.close()

print("Update Finished, please also update CategoryAction.qml with new category name.")
