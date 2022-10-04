#include "alkpayeelinks.h"

class AlkPayeeLinks::Private : public QObject
{
public:
    Private(QObject *parent)
        : QObject(parent)
    {
    }
};

AlkPayeeLinks::AlkPayeeLinks(QObject *parent)
    : d(new Private(this))
{
}

AlkPayeeLinks::~AlkPayeeLinks()
{
}
