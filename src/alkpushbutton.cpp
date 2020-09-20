#include "alkpushbutton.h"
#include <KGuiItem>

AlkPushButton::AlkPushButton(QWidget *widget) : QPushButton(widget)
{
}

void AlkPushButton::setGuiItem(const KGuiItem &item)
{
   KGuiItem::assign(this, item);
}
