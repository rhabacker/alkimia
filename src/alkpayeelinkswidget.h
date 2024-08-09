/*
    SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#pragma once

#include <alkimia/alk_export.h>

#include <QWidget>

class ALK_EXPORT AlkPayeeLinksWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AlkPayeeLinksWidget(QWidget *parent = nullptr);

private:
    class Private;
    Private *const d;
};
