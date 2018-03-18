#include "numberwidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QContextMenuEvent>
#include <QMenu>

int numberWidget::createdNumbers=0;

numberWidget::numberWidget(QWidget *parent)
    :movingWidget(parent)
{
    setToolTip("Right click to show a menu");

    //randomize a number
    number=std::rand()%100;
    rectColor=Qt::blue;

    //create menu's actions
    deleteAction=new QAction("Delete number", this);
    editAction=new QAction("Edit number", this);

    nr=createdNumbers++;

    QObject::connect(deleteAction, &QAction::triggered, this, &onDeleteNumber);
    QObject::connect(editAction, &QAction::triggered, this, &onEditNumber);
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

void numberWidget::contextMenuEvent(QContextMenuEvent *e)
{
    if (e->reason()==QContextMenuEvent::Mouse)
    {
        //create menu
        QMenu menu(this);
        menu.addAction(deleteAction);
        menu.addSeparator();
        menu.addAction(editAction);
        menu.exec(e->globalPos());
    }
}

void numberWidget::onDeleteNumber()
{
    emit numberDeleted();
}

void numberWidget::onEditNumber()
{

}

