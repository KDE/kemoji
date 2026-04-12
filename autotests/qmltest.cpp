/*
 *  SPDX-FileCopyrightText: 2025 James Graham <james.h.graham@protonmail.com>
 *
 *  SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include <QQmlContext>
#include <QQmlEngine>
#include <QtQuickTest>

#include <KEmoji/Category>
#include <KEmoji/Emoji>
#include <KEmoji/Group>

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
    std::list<KEmoji::Emoji> m_emojis;
    KEmoji::Group m_testGroup;

    void setTestEmojis(QQmlEngine *engine)
    {
        auto emoji = KEmoji::Emoji(u"😀"_s);
        emoji.setName(u"Name 1"_s);
        emoji.setCategory(KEmoji::Categories::Smileys);
        auto it = m_emojis.insert(m_emojis.end(), emoji);
        m_testGroup.add(it);
        engine->rootContext()->setContextProperty(u"emoji1"_s, QVariant::fromValue(emoji));
        emoji = KEmoji::Emoji(u"😃"_s);
        emoji.setName(u"Name 2"_s);
        emoji.setCategory(KEmoji::Categories::Smileys);
        it = m_emojis.insert(m_emojis.end(), emoji);
        m_testGroup.add(it);
        engine->rootContext()->setContextProperty(u"emoji2"_s, QVariant::fromValue(emoji));
        emoji = KEmoji::Emoji(u"😄"_s);
        emoji.setName(u"Name 3"_s);
        emoji.setCategory(KEmoji::Categories::Smileys);
        it = m_emojis.insert(m_emojis.end(), emoji);
        m_testGroup.add(it);
        engine->rootContext()->setContextProperty(u"emoji3"_s, QVariant::fromValue(emoji));
        emoji = KEmoji::Emoji(u"😁"_s);
        emoji.setName(u"Name 4"_s);
        emoji.setCategory(KEmoji::Categories::Smileys);
        it = m_emojis.insert(m_emojis.end(), emoji);
        m_testGroup.add(it);
        engine->rootContext()->setContextProperty(u"emoji4"_s, QVariant::fromValue(emoji));

        engine->rootContext()->setContextProperty(u"testGroup"_s, QVariant::fromValue(m_testGroup));
    }
};

QUICK_TEST_MAIN_WITH_SETUP(KEmoji, Setup)

#include "qmltest.moc"
