/*
 *  SPDX-FileCopyrightText: 2025 James Graham <james.h.graham@protonmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include <QFontDatabase>
#include <QQmlContext>
#include <QQmlEngine>
#include <QtQuickTest>

#include <KEmoji/Category>
#include <KEmoji/Emoji>
#include <KEmoji/Group>

#include "dict.h"

using namespace Qt::Literals::StringLiterals;

class Setup : public QObject
{
    Q_OBJECT

public:
    Setup()
    {
    }

public Q_SLOTS:
    void qmlEngineAvailable(QQmlEngine *engine)
    {
        setTestEmojis(engine);
    }

private:
    KEmoji::Group m_testGroup;

    void setTestEmojis(QQmlEngine *engine)
    {
        const auto &dict = KEmoji::Dict::instance();

        QSignalSpy spy(&dict, &KEmoji::Dict::loadingChanged);
        while (dict.loading()) {
            QVERIFY(spy.wait(5000));
        }

        auto emoji = KEmoji::Emoji(u"😀"_s);
        m_testGroup.add(emoji);
        engine->rootContext()->setContextProperty(u"emoji1"_s, QVariant::fromValue(emoji));
        emoji = KEmoji::Emoji(u"😃"_s);
        m_testGroup.add(emoji);
        engine->rootContext()->setContextProperty(u"emoji2"_s, QVariant::fromValue(emoji));
        emoji = KEmoji::Emoji(u"😄"_s);
        m_testGroup.add(emoji);
        engine->rootContext()->setContextProperty(u"emoji3"_s, QVariant::fromValue(emoji));
        emoji = KEmoji::Emoji(u"😁"_s);
        m_testGroup.add(emoji);
        engine->rootContext()->setContextProperty(u"emoji4"_s, QVariant::fromValue(emoji));

        engine->rootContext()->setContextProperty(u"testGroup"_s, QVariant::fromValue(m_testGroup));

#ifdef Q_OS_WINDOWS
        bool hasEmojiFont = false;
#else
        bool hasEmojiFont = std::ranges::any_of(QFontDatabase::families(), [](const QString &family) {
            return family.contains(u"emoji", Qt::CaseInsensitive);
        });
#endif
        engine->rootContext()->setContextProperty(u"hasEmojiFont"_s, hasEmojiFont);
    }
};

QUICK_TEST_MAIN_WITH_SETUP(KEmoji, Setup)

#include "qmltest.moc"
