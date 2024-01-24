/*
    SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef ALKPAYEEREFERENCELINK_H
#define ALKPAYEEREFERENCELINK_H

#include <alkimia/alk_export.h>

#include <QDate>
#include <QDebug>
#include <QObject>

class ALK_EXPORT AlkPayeeReferenceLinkData
{
public:
    QDate date;
    QString idPattern;
    QString name;
    QString urlTemplate;
    QString uuid;
};


ALK_EXPORT QDebug operator<<(QDebug d, const AlkPayeeReferenceLinkData &v);

class AlkNewStuffEngine;

class ALK_EXPORT AlkPayeeReferenceLink : public QObject
{
    Q_OBJECT
public:
    explicit AlkPayeeReferenceLink(const QString &name, AlkNewStuffEngine &engine, QObject *parent = nullptr);
    AlkPayeeReferenceLinkData &data() const;

private:
    class Private;
    Private *d;

};

#endif // ALKPAYEEREFERENCELINK_H
