#include "numberwidget.h"
#include <QPainter>
#include <QMouseEvent>
#include <QContextMenuEvent>
#include <QMenu>
#include "changenumberdialog.h"

extern sortState get_state();

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

    QObject::connect(deleteAction, &QAction::triggered, this, &onDeleteNumberClicked);
    QObject::connect(editAction, &QAction::triggered, this, &onEditNumberClicked);
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
    if (e->reason()==QContextMenuEvent::Mouse && ::get_state()==sortState::NotMoving)
    {
        //create menu
        QMenu menu(this);
        menu.addAction(deleteAction);
        menu.addSeparator();
        menu.addAction(editAction);
        menu.exec(e->globalPos());
    }
}

void numberWidget::onDeleteNumberClicked()
{
    emit numberDeleteClicked();
}

void numberWidget::onEditNumberClicked()
{
    changeNumberDialog* dial=new changeNumberDialog(this);
    dial->set_number(number);
    dial->show();
    QObject::connect(dial, SIGNAL(numberEdited(int)), this, SLOT(onNumberEdited(int)));
}

void numberWidget::onNumberEdited(int newNumber)
{
    number=newNumber;
    update();
}
