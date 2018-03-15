#include "numberwidget.h"
#include <QPainter>

int numberWidget::createdNumbers=0;

numberWidget::numberWidget(QWidget *parent)
    :movingWidget(parent)
{
    //randomize a number
    number=std::rand()%100;
    rectColor=Qt::blue;

    nr=createdNumbers++;
}

void numberWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);

    //set number's font
    QFont f;
    f.setPointSize(30);
    f.setBold(true);
    p.setFont(f);

    //yellow number
    p.setPen(Qt::yellow);

    //blue background
    p.fillRect(0, 0, width(), height(), rectColor);
    p.drawText(0, 0, width(), height(), Qt::AlignCenter, QString::number(number));
}
