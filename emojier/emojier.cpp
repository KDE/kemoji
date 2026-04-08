/*
    SPDX-FileCopyrightText: 2019 Aleix Pol Gonzalez <aleixpol@kde.org>

    SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include <KAboutData>
#include <KLocalizedString>
#include <KSharedConfig>
#include <KWindowConfig>
#include <KWindowSystem>
#include <QAbstractListModel>
#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QIcon>
#include <QList>
#include <QLocale>
#include <QPainter>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickImageProvider>
#include <QQuickStyle>
#include <QQuickWindow>
#include <QSessionManager>

#ifdef HAVE_KDBUSADDONS
#include <KDBusService>
#endif

#ifdef HAVE_KCRASH
#include <KCrash>
#endif

#include "kemoji_version.h"
#include "settings.h"

using namespace Qt::Literals::StringLiterals;

int main(int argc, char **argv)
{
    QCoreApplication::setAttribute(Qt::AA_DisableSessionManager);
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon::fromTheme(QStringLiteral("preferences-desktop-emoticons")));

#ifdef HAVE_KCRASH
    KCrash::initialize();
#endif

    KLocalizedString::setApplicationDomain(QByteArrayLiteral("org.kde.emojier"));

    KAboutData about(QStringLiteral("emojier"),
                     i18n("Emoji Selector"),
                     QStringLiteral(KEMOJI_VERSION_STRING),
                     i18n("Emoji Selector"),
                     KAboutLicense::GPL,
                     i18n("(C) 2019 Aleix Pol i Gonzalez"));
    about.addAuthor(QStringLiteral("Aleix Pol i Gonzalez"), QString(), QStringLiteral("aleixpol@kde.org"));
    about.setTranslator(i18nc("NAME OF TRANSLATORS", "Your names"), i18nc("EMAIL OF TRANSLATORS", "Your emails"));
    //     about.setProductName("");
    about.setProgramLogo(app.windowIcon());
    KAboutData::setApplicationData(about);

    QApplication::setStyle(QStringLiteral("breeze"));
    if (qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE")) {
        QQuickStyle::setStyle(QStringLiteral("org.kde.desktop"));
    }

#ifdef HAVE_KDBUSADDONS
    KDBusService::StartupOptions startup = {};
    {
        QCommandLineParser parser;

        QCommandLineOption replaceOption({QStringLiteral("replace")}, i18n("Replace an existing instance"));
        parser.addOption(replaceOption);
        about.setupCommandLine(&parser);
        parser.process(app);
        about.processCommandLine(&parser);

        if (parser.isSet(replaceOption)) {
            startup |= KDBusService::Replace;
        }
    }

    KDBusService *service = new KDBusService(KDBusService::Unique | startup, &app);
#endif

    // Clean up old window geometry data from before using WindowStateSaver
    KSharedConfig::openConfig()->deleteGroup(QStringLiteral("Window"));

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));
    engine.loadFromModule("org.kde.emojier", "Emojier");

#ifdef HAVE_KDBUSADDONS
    QObject::connect(service, &KDBusService::activateRequested, &engine, [&engine](const QStringList & /*arguments*/, const QString & /*workingDirectory*/) {
        for (QObject *object : engine.rootObjects()) {
            auto w = qobject_cast<QQuickWindow *>(object);
            if (!w)
                continue;

            KWindowSystem::updateStartupId(w);
            KWindowSystem::activateWindow(w);

            w->setVisible(true);
            w->raise();
        }
    });
#endif

    KEmoji::Settings::instance().registerCustomEmoji(QUrl::fromLocalFile(u"/home/jgraham/kde/src/kemoji/autotests/data/360px-Mascot_konqi.png"_s), u"Konqi"_s);

    return app.exec();
}
