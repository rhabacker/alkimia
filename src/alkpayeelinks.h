#ifndef ALKPAYEELINKS_H
#define ALKPAYEELINKS_H

#include <alkimia/alk_export.h>

#include <QDate>
#include <QUrl>
#include <QObject>

class AlkPayeeLinkConfig
{
public:
    QDate timeStamp;
    QString key;
    QString name;
    QString idPattern;
    QString urlTemplate;
    QString notes;

    AlkPayeeLinkConfig() {}
    AlkPayeeLinkConfig(const QString &data, int version)
    {
        QStringList cols = data.split(';');
        if (cols.size() == 6) {
            timeStamp = QDate::fromString(cols[0]);
            key = cols[1];
            name = cols[2];
            idPattern = cols[3];
            urlTemplate= cols[4];
            notes = cols[5];
        }
    }

    static bool match(const QString &data)
    {
        return data.count(';') == 5;
    }

    bool valid()
    {
        return !key.isEmpty() && !name.isEmpty() && !idPattern.isEmpty() && !urlTemplate.isEmpty();
    }

    QStringList matchingLinks(const QString& text) const
    {
      QStringList result;
      if (idPattern.isEmpty() || urlTemplate.isEmpty())
        return result;
      QRegExp rx(idPattern.contains("(") ? QString("%1").arg(idPattern) : QString("(%1)").arg(idPattern));
      if (rx.indexIn(text) == -1)
        return result;
      result = rx.capturedTexts();
      return result;
    }

    QUrl payeeLink(const QString& text) const
    {
        QStringList matches = matchingLinks(text);
        QString result;
        if (matches.size() == 2) {
            return QUrl(urlTemplate.arg(matches[1]));
        } else if (matches.size() == 3) {
            return QUrl(urlTemplate.arg(matches[1], matches[2]));
        } else {
            return QUrl();
        }
    }

    QString decorateLink(const QString& text) const
    {
      QStringList matches = matchingLinks(text);
      if (matches.size() == 0)
        return text;
      QRegExp rx(QString("(%1)").arg(matches[0]));

      QString result = text;
      result.replace(rx, "<u>\\1</u>");
      return result;
    }
};

typedef QList<AlkPayeeLinkConfig> AlkPayeeLinkConfigList;

class ALK_EXPORT AlkPayeeLinks : public QObject
{
    Q_OBJECT

public:
    explicit AlkPayeeLinks(QObject *parent = nullptr);
    ~AlkPayeeLinks();

    AlkPayeeLinkConfigList &payeeLinks();
    AlkPayeeLinkConfig &payeeLink(int i) const;
    const AlkPayeeLinkConfig &payeeLink(const QString &key) const;
    static AlkPayeeLinks &instance();
private:
    class Private;
    Private *const d;
};

#endif // ALKPAYEELINKS_H
