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

#ifndef ALKONLINEQUOTESPROFILEMANAGER_H
#define ALKONLINEQUOTESPROFILEMANAGER_H

#include "alk_export.h"

#include <QObject>

class AlkOnlineQuotesProfile;

typedef QList<AlkOnlineQuotesProfile *> AlkOnlineQuotesProfileList;

class ALK_EXPORT AlkOnlineQuotesProfileManager
{
public:
    AlkOnlineQuotesProfileManager();
    ~AlkOnlineQuotesProfileManager();

    void addProfile(AlkOnlineQuotesProfile *profile);
    AlkOnlineQuotesProfileList profiles();

    static AlkOnlineQuotesProfileManager &instance();
private:
    class Private;
    Private *d;
};

#endif // ALKONLINEQUOTESPROFILEMANAGER_H