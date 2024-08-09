/*
    SPDX-FileCopyrightText: 2024 Ralf Habacker ralf.habacker @freenet.de

    This file is part of libalkimia.

    SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "alkpayeereferencelink.h"

#include "alknewstuffengine.h"
#include "klocalizedstring.h"

#include <QDate>
#include <QFileInfo>
#include <QLabel>
#include <QRegularExpression>
#include <QTextStream>
#include <QUrl>

QDebug operator<<(QDebug d, const AlkPayeeReferenceLinkData &v)
{
    d << "AlkPayeeReferenceLinkData:" << v.date << v.idPattern << v.name << v.urlTemplate << v.uuid;
    return d;
}

class AlkPayeeReferenceLink::Private : public AlkPayeeReferenceLinkData
{
public:
    QRegularExpression m_regex;

    QRegularExpression &setRegExp(const QString &pattern)
    {
        m_regex.setPattern(pattern);
        return m_regex;
    }

    QRegularExpression &idPatternRegex()
    {
        if (idPattern.contains(QStringLiteral("(")))
            return setRegExp(idPattern);
        else
            return setRegExp(QStringLiteral("(%1)").arg(idPattern));
    }

    bool read(const QString &fileName)
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
            QString value = line.mid(index + 1);
            if (key == "date") {
                date = QDate::fromString(value, "yyyy-MM-dd");
            } else if (key == "idpattern") {
                idPattern = value;
            } else if (key == "name") {
                name = value;
            } else if (key == "searchreplacepattern") {
                searchReplacePattern = value;
            } else if (key == "testpattern") {
                testPattern = value;
            } else if (key == "urltemplate") {
                urlTemplate = value;
            } else if (key == "uuid") {
                uuid = value;
            }
        }

        return true;
    }

    bool write(const QString &fileName)
    {
        QFileInfo f(fileName);
        QFile file(f.absoluteFilePath());
        if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
            return false;

        QTextStream out(&file);
        out << "date=" << date.toString(Qt::ISODate) << "\n"
            << "idpattern=" << idPattern << "\n"
            << "name=" << name << "\n"
            << "searchreplacepattern=" << searchReplacePattern << "\n"
            << "testpattern=" << testPattern << "\n"
            << "urltemplate=" << urlTemplate << "\n"
            << "uuid=" << uuid << "\n";

        return true;
    }

    QStringList searchReplace(const QStringList &texts)
    {
        if (searchReplacePattern.isEmpty())
            return texts;

        QStringList result(texts);
        for (const auto &sp : searchReplacePattern.split(";")) {
            QStringList p = sp.split("/");
            if (sp.size() == 0 || p.at(0) != "s")
                continue;
            QRegularExpression &re = setRegExp(p.at(1));
            for (int i = 0; i < result.size(); i++)
                result[i].replace(re, p.at(2));
        }
        return result;
    }
};

AlkPayeeReferenceLink::AlkPayeeReferenceLink(const QString &name, AlkNewStuffEngine &engine, QObject *parent)
    : QObject(parent)
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

AlkPayeeReferenceLink::AlkPayeeReferenceLink(const AlkPayeeReferenceLinkData &data, QObject *parent)
    : QObject(parent)
    , d(new Private)
{
    *dynamic_cast<AlkPayeeReferenceLinkData *>(d) = data;
}

AlkPayeeReferenceLinkData &AlkPayeeReferenceLink::data() const
{
    return dynamic_cast<AlkPayeeReferenceLinkData &>(*d);
}

QUrl AlkPayeeReferenceLink::link(const QString &text) const
{
    QStringList matches = d->searchReplace(matchingLinks(text));
    if (matches.size() == 1) {
        return QUrl(d->urlTemplate.arg(matches[0]));
    } else if (matches.size() == 3) {
        return QUrl(d->urlTemplate.arg(matches[0], matches[1]));
    } else {
        return QUrl();
    }
}

QString AlkPayeeReferenceLink::decorateLink(const QString &text) const
{
    QStringList matches = matchingLinks(text);
    if (matches.size() == 0 || matches[0].isEmpty())
        return text;
    QRegularExpression &rx = d->setRegExp(QStringLiteral("(%1)").arg(matches[0]));

    QString result = text;
    result.replace(rx, QStringLiteral("<\\1>"));
    return result;
}

QStringList AlkPayeeReferenceLink::matchingLinks(const QString &text) const
{
    QRegularExpressionMatch match = d->idPatternRegex().match(text);
    if (!match.hasMatch())
        return QStringList();
    QStringList result = match.capturedTexts();
    result.takeFirst();
    return result;
}

void AlkPayeeReferenceLink::setupLabel(QLabel *label, const QUrl &url)
{
    if (url.isEmpty()) {
        label->setText(QString());
        return;
    }
    label->setTextFormat(Qt::RichText);
    label->setTextInteractionFlags(Qt::TextBrowserInteraction);
    label->setOpenExternalLinks(true);
    label->setText(QString("<a href=\"%1\">%2</a>").arg(url.toString(), url.toString()));
    return;
}
