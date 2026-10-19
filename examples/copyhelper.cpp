/*
    SPDX-FileCopyrightText: 2019 Aleix Pol Gonzalez <aleixpol@kde.org>

    SPDX-License-Identifier: LGPL-2.0-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
*/

#include "copyhelper.h"

#include <QClipboard>
#include <QGuiApplication>

void CopyHelperPrivate::copyTextToClipboard(const QString &text)
{
    QClipboard *clipboard = qGuiApp->clipboard();
    clipboard->setText(text, QClipboard::Clipboard);
    clipboard->setText(text, QClipboard::Selection);
}

#include "moc_copyhelper.cpp"
