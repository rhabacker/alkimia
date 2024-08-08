#ifndef ALKPAYEELINKSWIDGET_H
#define ALKPAYEELINKSWIDGET_H

#include <alkimia/alk_export.h>

#include <QWidget>

class ALK_EXPORT AlkPayeeLinksWidget : public QWidget
{
    Q_OBJECT
public:
    explicit AlkPayeeLinksWidget(QWidget *parent = nullptr);

private:
    class Private;
    Private *const d;
};

#endif // ALKPAYEELINKSWIDGET_H
