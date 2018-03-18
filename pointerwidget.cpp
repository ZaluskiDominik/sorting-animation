#include "pointerwidget.h"
#include <QPainter>

pointerWidget::pointerWidget(QWidget *parent, QString ptrChar)
    :movingWidget(parent)
{
    character=ptrChar;
    positionChanged=true;
    QObject::connect(this, SIGNAL(changePosRequest(int, int)), this, SLOT(onChangePosRequest(int, int)));
}

void pointerWidget::change_pos(int x, int y)
{
    mutex.lock();
    positionChanged=false;
    mutex.unlock();
    emit changePosRequest(x, y);
}

void pointerWidget::onChangePosRequest(int x, int y)
{
    setGeometry(x, y, width(), height());
    mutex.lock();
    positionChanged=true;
    mutex.unlock();
}

void pointerWidget::paintEvent(QPaintEvent *)
{
    QPainter p(this);

    QFont f;
    f.setPointSize(30);
    f.setBold(true);
    p.setFont(f);

    //turquise color of the text
    p.setPen(QColor(64, 224, 208));

    p.drawText(0, 0, width(), height(), Qt::AlignCenter, character);
}
