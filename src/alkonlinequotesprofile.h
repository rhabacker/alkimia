/***************************************************************************
 *   Copyright 2018  Ralf Habacker <ralf.habacker@freenet.de>              *
 *                                                                         *
 *   This file is part of libalkimia.                                      *
 *                                                                         *
 *   libalkimia is free software; you can redistribute it and/or           *
 *   modify it under the terms of the GNU Lesser General Public License    *
 *   as published by the Free Software Foundation; either version 2.1 of   *
 *   the License or (at your option) version 3 or any later version.       *
 *                                                                         *
 *   libalkimia is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>  *
 ***************************************************************************/

#ifndef ALKONLINEQUOTESPROFILE_H
#define ALKONLINEQUOTESPROFILE_H

#include <alkimia/alk_export.h>
#include <alkimia/alkonlinequotesource.h>

#include <QString>
#include <QMap>

class KConfig;

class AlkOnlineQuoteSource;
class AlkOnlineQuotesProfileManager;

class ALK_EXPORT AlkOnlineQuotesProfile
{
public:
    typedef QMap<QString, AlkOnlineQuoteSource> Map;
    enum class Type {
        Undefined, None, Alkimia4, Alkimia5, KMyMoney4, KMyMoney5, Skrooge4, Skrooge5, Script
    };
    AlkOnlineQuotesProfile(const QString &name = "alkimia", Type type = Type::None,
        const QString &ghnsConfigFile = QString());
    ~AlkOnlineQuotesProfile();

    QString name() const;
    QString hotNewStuffConfigFile() const;
    QString hotNewStuffRelPath() const;
    QStringList hotNewStuffReadPath() const;
    QString hotNewStuffWriteDir() const;
    QString hotNewStuffReadFilePath(const QString &fileName) const;
    QString hotNewStuffWriteFilePath(const QString &fileName) const;
    QString kConfigFile() const;
    void setManager(AlkOnlineQuotesProfileManager *manager);
    AlkOnlineQuotesProfileManager *manager();
    KConfig *kConfig() const;
    Type type();
    bool hasGHNSSupport();

    /**
     * return available default sources of this profile
     */
    const Map defaultQuoteSources();

    /**
     * return available sources of this profile
     */
    const QStringList quoteSources();

    QString scriptPath();

private:
    class Private;
    Private *const d;
};

#endif // ALKONLINEQUOTESPROFILE_H
