/*
    SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef ALKNEWSTUFFENTRY_H
#define ALKNEWSTUFFENTRY_H

#include <alkimia/alk_export.h>

#include <QList>
#include <QStringList>

/**
 * Platform independent wrapper for new stuff entry
 *
 * @author Ralf Habacker
 */
class ALK_EXPORT AlkNewStuffEntry
{
public:
    enum Status {
        Invalid,
        Downloadable,
        Installed,
        Updateable,
        Deleted,
        Installing,
        Updating
    };

    QString category;
    QString id;
    QString name;
    QString providerId;
    QString version;
    QStringList installedFiles;
    Status status;
    QString payload;
};

typedef QList<AlkNewStuffEntry> AlkNewStuffEntryList;

const char *toString(AlkNewStuffEntry::Status status);

QDebug operator<<(QDebug out, const AlkNewStuffEntry &entry);
QDebug operator<<(QDebug out, const AlkNewStuffEntryList &entries);

#endif // ALKNEWSTUFFENTRY_H
