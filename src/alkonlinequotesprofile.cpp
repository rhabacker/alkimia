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

#include "alkonlinequotesprofile.h"

#include "alkonlinequotesprofilemanager.h"
#include "alkonlinequotesource.h"

#include "alkonlinequotesprofile_p.h"

// define static members
QString AlkOnlineQuotesProfile::Private::m_financeQuoteScriptPath;
QStringList AlkOnlineQuotesProfile::Private::m_financeQuoteSources;


AlkOnlineQuotesProfile::AlkOnlineQuotesProfile(const QString &name, Type type,
                                               const QString &ghnsConfigFile)
    : d(new Private(this))
{
    d->m_name = name;
    d->m_GHNSFile = ghnsConfigFile;
    d->m_type = type;
    if (type == Type::KMyMoney5)
        d->m_kconfigFile = QString("%1/.config/kmymoney/kmymoneyrc").arg(QDir::homePath());
    else if (type == Type::KMyMoney4)
        d->m_kconfigFile = QString("%1/.kde4/share/config/kmymoneyrc").arg(QDir::homePath());
    else if (type == Type::Alkimia5)
        d->m_kconfigFile = QString("%1/.config/alkimiarc").arg(QDir::homePath());
    else if (type == Type::Alkimia4)
        d->m_kconfigFile = QString("%1/.kde4/share/config/alkimiarc").arg(QDir::homePath());
    else
        d->m_kconfigFile = "";
    if (!d->m_kconfigFile.isEmpty())
        d->m_config = new KConfig(d->m_kconfigFile);
    if (!d->m_GHNSFile.isEmpty()) {
        KConfig ghnsFile(hotNewStuffConfigFile());
        KConfigGroup group = ghnsFile.group("KNewStuff3");
        d->m_GHNSFilePath = group.readEntry("TargetDir");
        d->checkUpdates();
    }
}

AlkOnlineQuotesProfile::~AlkOnlineQuotesProfile()
{
    delete d;
}

QString AlkOnlineQuotesProfile::name() const
{
    return d->m_name;
}

QString AlkOnlineQuotesProfile::hotNewStuffConfigFile() const
{
    QString configFile = KStandardDirs::locate("config", d->m_GHNSFile);
    if (configFile.isEmpty()) {
        configFile = QString("%1/%2").arg(KNSRC_DIR, d->m_GHNSFile);
    }

    return configFile;
}

QString AlkOnlineQuotesProfile::hotNewStuffReadFilePath(const QString &fileName) const
{
    foreach(const QString &path, hotNewStuffReadPath()) {
        QFileInfo f(path + fileName);
        if (f.exists())
            return f.absoluteFilePath();
    }
    return QString();
}

QString AlkOnlineQuotesProfile::hotNewStuffWriteFilePath(const QString &fileName) const
{
    return KStandardDirs::locateLocal("data", d->m_GHNSFilePath + "/" + fileName);
}

QStringList AlkOnlineQuotesProfile::hotNewStuffReadPath() const
{
    return KStandardDirs().findDirs("data", d->m_GHNSFilePath + "/");
}

QString AlkOnlineQuotesProfile::hotNewStuffWriteDir() const
{
    return KStandardDirs().saveLocation("data", d->m_GHNSFilePath + "/");
}

QString AlkOnlineQuotesProfile::hotNewStuffRelPath() const
{
    return d->m_GHNSFilePath;
}

QString AlkOnlineQuotesProfile::kConfigFile() const
{
    return d->m_kconfigFile;
}

KConfig *AlkOnlineQuotesProfile::kConfig() const
{
    return d->m_config;
}

AlkOnlineQuotesProfile::Type AlkOnlineQuotesProfile::type()
{
    return d->m_type;
}

bool AlkOnlineQuotesProfile::hasGHNSSupport()
{
    return !d->m_GHNSFile.isEmpty();
}

const AlkOnlineQuotesProfile::Map AlkOnlineQuotesProfile::defaultQuoteSources()
{
    return d->defaultQuoteSources();
}

const QStringList AlkOnlineQuotesProfile::quoteSources()
{
    QStringList result;
    switch(d->m_type) {
    case AlkOnlineQuotesProfile::Type::Alkimia4:
    case AlkOnlineQuotesProfile::Type::Alkimia5:
    case AlkOnlineQuotesProfile::Type::KMyMoney4:
    case AlkOnlineQuotesProfile::Type::KMyMoney5:
        result << d->quoteSourcesNative();
        break;
    case AlkOnlineQuotesProfile::Type::Script:
        result << d->quoteSourcesFinanceQuote();
        break;
    case AlkOnlineQuotesProfile::Type::Skrooge:
        result << d->quoteSourcesSkrooge();
        break;
    case AlkOnlineQuotesProfile::Type::None:
        result << d->defaultQuoteSources().keys();
    default:
        break;
    }
    if (hasGHNSSupport())
        result << d->quoteSourcesGHNS();
    return result;
}

void AlkOnlineQuotesProfile::setManager(AlkOnlineQuotesProfileManager *manager)
{
    d->m_profileManager = manager;
}

AlkOnlineQuotesProfileManager *AlkOnlineQuotesProfile::manager()
{
    return d->m_profileManager;
}

#include "alkonlinequotesprofile.moc"
