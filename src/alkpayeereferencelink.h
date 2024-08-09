/*
    SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef ALKPAYEEREFERENCELINK_H
#define ALKPAYEEREFERENCELINK_H

#include <alkimia/alk_export.h>

#include <QDate>
#include <QDebug>
#include <QObject>

class QLabel;
class ALK_EXPORT AlkPayeeReferenceLinkData
{
public:
    QString name;
    QString idPattern;
    QString urlTemplate;
    QString testPattern;
    QString searchReplacePattern;
    QString uuid;
    QDate date;
};

Q_DECLARE_METATYPE(AlkPayeeReferenceLinkData);

ALK_EXPORT QDebug operator<<(QDebug d, const AlkPayeeReferenceLinkData &v);

class AlkNewStuffEngine;

class ALK_EXPORT AlkPayeeReferenceLink : public QObject
{
    Q_OBJECT
public:
    explicit AlkPayeeReferenceLink(const QString &name, AlkNewStuffEngine &engine, QObject *parent = nullptr);
    AlkPayeeReferenceLink(const AlkPayeeReferenceLinkData &data, QObject *parent = nullptr);
    AlkPayeeReferenceLinkData &data() const;

    /**
     * Generate url from given text
     * The method parses the given text and extracts an identifier when possible
     * @param text
     * @return url
     */
    QUrl link(const QString &text) const;

    /**
     * Return given text with decored identifier matching the identifier pattern stored
     * @param text
     * @return
     */
    QString decorateLink(const QString &text) const;

    QStringList matchingLinks(const QString &text) const;

    void setupLabel(QLabel *label, const QUrl &url);

private:
    class Private;
    Private *d;
};

#endif // ALKPAYEEREFERENCELINK_H
