/***************************************************************************
 *   Copyright 2019 Ralf Habacker <ralf.habacker@freenet.de>               *
 *                                                                         *
 *   This file is part of libalkimia.                                      *
 *                                                                         *
 *   libalkimia is free software; you can redistribute it and/or           *
 *   modify it under the terms of the GNU General Public License           *
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

#include "alkonlinequotesource.h"
#include "alkonlinequotesprofile.h"
#include "alkonlinequotesprofilemanager.h"

#include <QFile>
#include <QString>
#include <QTextStream>

static inline QTextStream& qStdout()
{
    static QTextStream r{stdout};
    return r;
}

static inline QTextStream& qStderr()
{
    static QTextStream r{stderr};
    return r;
}

int main()
{
    AlkOnlineQuotesProfileManager &manager = AlkOnlineQuotesProfileManager::instance();

    AlkOnlineQuotesProfile *profile;
    profile = new AlkOnlineQuotesProfile("kmymoney4", AlkOnlineQuotesProfile::Type::KMyMoney4);
    manager.addProfile(profile);

    QFile f(QString(CMAKE_QUOTES_SOURCE_DIR "/CMakeLists.txt"));
    if (!f.open(QFile::WriteOnly | QFile::Truncate))
        return 3;
    QTextStream c(&f);

    foreach(const QString &source, profile->quoteSources())
    {
        AlkOnlineQuoteSource quoteSource(source, profile);
        QString baseName = quoteSource.name();
        baseName.replace(QRegExp("[. ]"), "-");
        baseName.replace(QRegExp("[()]"), "");
        const QString fileName = QString("%1.txt").arg(baseName);
        const QString outFile = QString(CMAKE_QUOTES_SOURCE_DIR "/%1").arg(fileName);
        if (!quoteSource.write(outFile)) {
            qStderr() << "could not write to '" << outFile << "'\n";
            return 4;
        }
        qStdout() << "saving online quote '" << quoteSource.name() << "' to '" << outFile << "'\n";
        c << "install(FILES " << fileName << " DESTINATION ${QUOTES_INSTALL_DIR})\n";
    }
    return 0;
}
