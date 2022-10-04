#ifndef ALKPAYEELINKS_H
#define ALKPAYEELINKS_H

#include <alkimia/alk_export.h>

#include <QObject>

class ALK_EXPORT AlkPayeeLinks : public QObject
{
    Q_OBJECT

public:
    explicit AlkPayeeLinks(QObject *parent = nullptr);
    ~AlkPayeeLinks();

private:
    class Private;
    Private *const d;
};

#endif // ALKPAYEELINKS_H
