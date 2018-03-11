#include "interactivebutton.h"
#include <QPixmap>
#include <QPainter>

interactiveButton::interactiveButton(QWidget *parent, QPixmap icon, QPixmap iconHov)
    :QAbstractButton(parent)
{
    cursorIn=false;
    plainIcon=icon;
    hoverIcon=iconHov;

    //set a circular shape to this button
    setMask(QRegion(-1, -1, icon.width()+1, icon.height()+1, QRegion::Ellipse));
}

void interactiveButton::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    if (cursorIn)
        //if cursor hover over then draw hoverIcon
        p.drawPixmap(0, 0, width(), height(), hoverIcon);
    else
        //else draw plainIcon
        p.drawPixmap(0, 0, width(), height(), plainIcon);
}

void interactiveButton::enterEvent(QEvent *)
{
    cursorIn=true;
}

void interactiveButton::leaveEvent(QEvent *)
{
    cursorIn=false;
}
