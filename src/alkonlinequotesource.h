/*
    SPDX-FileCopyrightText: 2018 Ralf Habacker <ralf.habacker@freenet.de>

    This file is part of libalkimia.

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef ALKONLINEQUOTESOURCE_H
#define ALKONLINEQUOTESOURCE_H

#include <alkimia/alkonlinequotesprofile.h>

#include <QString>

class AlkOnlineQuotesProfile;

/**
  * @author Thomas Baumgart & Ace Jones
  *
  * This is a helper class to store information about an online source
  * for stock prices or currency exchange rates.
  */
class ALK_EXPORT AlkOnlineQuoteSource
{
public:

    AlkOnlineQuoteSource();
    explicit AlkOnlineQuoteSource(const QString &name, AlkOnlineQuotesProfile *profile);
    explicit AlkOnlineQuoteSource(const QString &name, const QString &url, const QString &sym,
                         const QString &price, const QString &date, const QString &dateformat,
                         bool skipStripping = false);
    ~AlkOnlineQuoteSource();

    AlkOnlineQuoteSource(const AlkOnlineQuoteSource &other);
    AlkOnlineQuoteSource &operator=(AlkOnlineQuoteSource other);

    friend void swap(AlkOnlineQuoteSource& first, AlkOnlineQuoteSource& second);

    bool isEmpty();
    bool isValid();

    bool read();
    bool write();
    void rename(const QString &name);
    void remove();

    QString name() const;
    QString url() const;
    QString sym() const;
    QString price() const;
    QString date() const;
    QString dateformat() const;
    QString financeQuoteName() const;

    bool skipStripping() const;
    bool isGHNS();
    bool isReadOnly();
    bool isFinanceQuote() const;
    static bool isFinanceQuote(const QString &name);

    void setName(const QString &name);
    void setUrl(const QString &url);
    void setSym(const QString &symbol);
    void setPrice(const QString &price);
    void setDate(const QString &date);
    void setDateformat(const QString &dateformat);
    void setSkipStripping(bool state);
    void setGHNS(bool state);

    QString ghnsWriteFileName();
    void setProfile(AlkOnlineQuotesProfile *profile);
    AlkOnlineQuotesProfile *profile();

protected:
    class Private;
    Private *d;
};

inline void swap(AlkOnlineQuoteSource& first, AlkOnlineQuoteSource& second) // krazy:exclude=inline
{
  using std::swap;
  swap(first.d, second.d);
}

#endif // ALKONLINEQUOTESOURCE_H
