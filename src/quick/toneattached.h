/*
 *  SPDX-FileCopyrightText: 2026 James Graham <james.h.graham@protonmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#pragma once

#include <QObject>
#include <QtQmlIntegration/qqmlintegration.h>

#include "tones.h"

/*!
 * \class ToneAttached
 *
 * \brief An attached property to allow KEmoji::Tones::Tone data to be accessed
 * in QML.
 *
 * By declaring a Tone object in QML and assigning it a tone value the
 * relevant tone properties can be obtained.
 *
 * \code
 * import org.kde.kemoji as KEmoji
 *
 * ...
 * Repeater {
 *   model: 4
 *   delegate: QQC2.Action {
 *     required property int modelData
 *
 *     KEmoji.Tone.tone: modelData
 *     text: KEmoji.Tone.exampleUnicodeWithName
 *     Accessible.name: KEmoji.Tone.name
 *     onTriggered: {
 *       ...
 *     }
 *   }
 * }
 * ...
 * \endcode
 *
 * While primarily designed for setting the properties of an action it can be used more
 * generally.
 *
 * \code
 * ...
 * Kirigami.ScrollablePage {
 *   KEmoji.Tone.tone: KEmoji.Tone.Dark
 *   title: KEmoji.Tone.name
 * ...
 * \endcode
 */
class ToneAttached : public QObject
{
    Q_OBJECT
    QML_NAMED_ELEMENT(Tone)
    QML_ATTACHED(ToneAttached)

    /*!
     * \brief The \c KEmoji::Tones::Tone the attached property is for.
     *
     * \sa KEmoji::Tones::Tone
     */
    Q_PROPERTY(KEmoji::Tones::Tone tone READ tone WRITE setTone NOTIFY toneChanged)

    /*!
     * \brief The unicode code point of the \c KEmoji::Tones::Tone as a \c QString.
     *
     * \note Since \c KEmoji::Tones::Tone::Neutral has no code point (it's a lcak of tone)
     * an empty string is returned.
     */
    Q_PROPERTY(QString unicode READ unicode NOTIFY toneChanged)

    /*!
     * \brief An example emoji for the \c KEmoji::Tones::Tone as a \c QString.
     *
     * Currently uses the "hand with fingers splayed emoji" as the example, 🖐️.
     */
    Q_PROPERTY(QString exampleUnicode READ exampleUnicode NOTIFY toneChanged)

    /*!
     * \brief The name of the \c KEmoji::Tones::Tone as a \c QString.
     */
    Q_PROPERTY(QString name READ name NOTIFY toneChanged)

    /*!
     * \brief An example emoji with the tone name for the \c KEmoji::Tones::Tone as a \c QString.
     *
     * Currently uses the "hand with fingers splayed emoji" as the example, 🖐️.
     */
    Q_PROPERTY(QString exampleUnicodeWithName READ exampleUnicodeWithName NOTIFY toneChanged)

public:
    explicit ToneAttached(QObject *parent = nullptr);

    KEmoji::Tones::Tone tone() const;
    void setTone(KEmoji::Tones::Tone category);
    QString unicode() const;
    QString exampleUnicode() const;
    QString name() const;
    QString exampleUnicodeWithName() const;

    static ToneAttached *qmlAttachedProperties(QObject *object);

Q_SIGNALS:
    void toneChanged();

private:
    KEmoji::Tones::Tone m_tone = KEmoji::Tones::Neutral;
};
