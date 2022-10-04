#include "alkpayeelinkswidget.h"
#include "ui_alkpayeelinkswidget.h"

class AlkPayeeLinksWidget::Private : public QWidget, public Ui::AlkPayeeLinksWidget
{
public:
    Private(QWidget *parent)
    {
        setupUi(parent);
    }
};

AlkPayeeLinksWidget::AlkPayeeLinksWidget(QWidget *parent)
    : QWidget(parent)
    , d(new Private(this))
{
}

AlkPayeeLinksWidget::~AlkPayeeLinksWidget()
{
    delete d;
}
