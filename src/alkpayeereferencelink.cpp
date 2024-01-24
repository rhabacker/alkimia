/*
    SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "alkpayeereferencelink.h"

#include "alknewstuffengine.h"

#include <QDate>
#include <QFileInfo>
#include <QTextStream>

QDebug operator<<(QDebug d, const AlkPayeeReferenceLinkData &v)
{
    d << "AlkPayeeReferenceLinkData:"
      << v.date
      << v.idPattern
      << v.name
      << v.urlTemplate
      << v.uuid;
    return d;
}

class AlkPayeeReferenceLink::Private : public AlkPayeeReferenceLinkData
{
public:
    bool read(const QString &file);
};

AlkPayeeReferenceLink::AlkPayeeReferenceLink(const QString &name, AlkNewStuffEngine &engine, QObject *parent)
    : QObject{parent}
    , d(new Private)
{
    for (const AlkNewStuffEntry &entry : engine.installedEntries()) {
        if (entry.name == name) {
            if (entry.installedFiles.size() > 0) {
                d->read(entry.installedFiles[0]);
            }
        }
    }
}

AlkPayeeReferenceLinkData &AlkPayeeReferenceLink::data() const
{
    return dynamic_cast<AlkPayeeReferenceLinkData&>(*d);
}

bool AlkPayeeReferenceLink::Private::read(const QString &fileName)
{
    QFileInfo f(fileName);
    QFile file(f.absoluteFilePath());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return false;

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        int index = line.indexOf("=");
        if (index == -1)
            return false;
        QString key = line.left(index);
        QString value = line.mid(index+1);
        if (key == "date")
            date = QDate::fromString(value, "yyyy-MM-dd");
        else if (key == "idpattern") {
            idPattern = value;
        } else if (key == "urltemplate") {
            urlTemplate = value;
        } else if (key == "uuid") {
            uuid = value;
        } else if (key == "name") {
            name = value;
        }
    }

    return true;
}

