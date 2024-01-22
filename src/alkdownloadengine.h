#ifndef ALKDOWNLOADENGINE_H
#define ALKDOWNLOADENGINE_H

#include "alkonlinequote.h"

#include <QObject>

class AlkDownloadEngine : public QObject
{
    Q_OBJECT
public:
    explicit AlkDownloadEngine(AlkOnlineQuote *p, QObject *parent = nullptr);

Q_SIGNALS:

private:
    class Private;
    const Private *d;

};

#endif // ALKDOWNLOADENGINE_H
