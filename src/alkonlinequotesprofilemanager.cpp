/*
    SPDX-FileCopyrightText: 2018 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#include "alkonlinequotesprofilemanager.h"
#include "alkonlinequotesprofile.h"
#include "alkwebpage.h"

#include <QPointer>
#include <QtDebug>

#if QT_VERSION > QT_VERSION_CHECK(5,0,0)
#include <QLocale>
#define initLocale() QLocale()
#else
#include <KGlobal>
#define initLocale() KGlobal::locale()
#endif

class AlkOnlineQuotesProfileManager::Private
{
public:
    AlkOnlineQuotesProfileList m_profiles;
    QPointer<AlkWebPage> m_page;
    bool m_withPage;
    Private()
      : m_withPage(false)
    {
    }

    /**
     * check that profile is not already added
     * @param profile
     * @param presentProfiles
     * @param notPresentProfiles
     * @return
     */
    bool checkProfile(const QString &wantedProfile, const QStringList &presentProfiles, QStringList& notPresentProfiles)
    {
        if (presentProfiles.contains(wantedProfile))
            return false;
        bool result = notPresentProfiles.contains(wantedProfile);
        notPresentProfiles.removeAll(wantedProfile);
        return result;
    }

    bool addProfiles(AlkOnlineQuotesProfileManager &manager, const QStringList &profiles)
    {
        QStringList presentProfiles = manager.profileNames();
        QStringList notPresentProfiles(profiles);
        if (checkProfile("no-config-file", presentProfiles, notPresentProfiles))
            manager.addProfile(new AlkOnlineQuotesProfile("no-config-file", AlkOnlineQuotesProfile::Type::None));
        if (checkProfile("alkimia4", presentProfiles, notPresentProfiles))
            manager.addProfile(new AlkOnlineQuotesProfile("alkimia4", AlkOnlineQuotesProfile::Type::Alkimia4, "alkimia-quotes.knsrc"));
        if (checkProfile("skrooge4", presentProfiles, notPresentProfiles))
            manager.addProfile(new AlkOnlineQuotesProfile("skrooge4", AlkOnlineQuotesProfile::Type::Skrooge4, "skrooge-quotes.knsrc"));
        if (checkProfile("kmymoney4", presentProfiles, notPresentProfiles))
            manager.addProfile(new AlkOnlineQuotesProfile("kmymoney4", AlkOnlineQuotesProfile::Type::KMyMoney4, "kmymoney-quotes.knsrc"));
        if (checkProfile("alkimia5", presentProfiles, notPresentProfiles))
            manager.addProfile(new AlkOnlineQuotesProfile("alkimia5", AlkOnlineQuotesProfile::Type::Alkimia5, "alkimia-quotes.knsrc"));
        if (checkProfile("skrooge5", presentProfiles, notPresentProfiles))
            manager.addProfile(new AlkOnlineQuotesProfile("skrooge5", AlkOnlineQuotesProfile::Type::Skrooge5, "skrooge-quotes.knsrc"));
        if (checkProfile("kmymoney5", presentProfiles, notPresentProfiles))
            manager.addProfile(new AlkOnlineQuotesProfile("kmymoney5", AlkOnlineQuotesProfile::Type::KMyMoney5, "kmymoney-quotes.knsrc"));
        if (checkProfile("Finance::Quote", presentProfiles, notPresentProfiles))
            manager.addProfile(new AlkOnlineQuotesProfile("Finance::Quote", AlkOnlineQuotesProfile::Type::Script));
        if (notPresentProfiles.size() > 0)
            qDebug() << "The profiles '" << notPresentProfiles << "' could not be added";

        return notPresentProfiles.size() == 0;
    }

    ~Private()
    {
        m_page.data()->deleteLater();
    }
};

AlkOnlineQuotesProfileManager::AlkOnlineQuotesProfileManager()
  : d(new Private)
{
}

AlkOnlineQuotesProfileManager::~AlkOnlineQuotesProfileManager()
{
    delete d;
}

bool AlkOnlineQuotesProfileManager::webPageEnabled()
{
    return d->m_withPage;
}

void AlkOnlineQuotesProfileManager::setWebPageEnabled(bool enable)
{
    d->m_withPage = enable;
}

void AlkOnlineQuotesProfileManager::addProfile(AlkOnlineQuotesProfile *profile)
{
    if (!d->m_profiles.contains(profile)) {
        d->m_profiles.append(profile);
        profile->setManager(this);
        connect(profile, SIGNAL(updateAvailable(const QString &, const QString &)), this, SIGNAL(updateAvailable(const QString &, const QString &)));
    }
}

AlkOnlineQuotesProfileList AlkOnlineQuotesProfileManager::profiles()
{
    return d->m_profiles;
}

AlkOnlineQuotesProfile *AlkOnlineQuotesProfileManager::profile(const QString &name)
{
    foreach (AlkOnlineQuotesProfile *profile, profiles()) {
        if (name == profile->name()) {
            return profile;
        }
    }
    return nullptr;
}

QStringList AlkOnlineQuotesProfileManager::profileNames()
{
    QStringList profiles;
    foreach(AlkOnlineQuotesProfile *profile, AlkOnlineQuotesProfileManager::instance().profiles()) {
        profiles.append(profile->name());
    }
    return profiles;
}

AlkWebPage *AlkOnlineQuotesProfileManager::webPage()
{
    if (!d->m_page) {
    // make sure that translations are installed on windows
        initLocale();
        d->m_page = new AlkWebPage;
    }
    return d->m_page;
}

AlkOnlineQuotesProfileManager &AlkOnlineQuotesProfileManager::instance()
{
    static AlkOnlineQuotesProfileManager manager;
    return manager;
}

bool AlkOnlineQuotesProfileManager::addProfiles(const QStringList &profiles)
{
    return d->addProfiles(*this, profiles);
}
