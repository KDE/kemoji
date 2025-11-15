/*
    SPDX-FileCopyrightText: 2021 Aleix Pol Gonzalez <aleixpol@blue-systems.com>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#pragma once

#include <QObject>
#include <QQmlEngine>

class CopyHelperPrivate : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    QML_NAMED_ELEMENT(CopyHelper)
    QML_SINGLETON
public:
    Q_INVOKABLE static void copyTextToClipboard(const QString &text);
};
