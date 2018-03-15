#ifndef NUMBERWIDGET_H
#define NUMBERWIDGET_H
#include "movingwidget.h"

class numberWidget : public movingWidget
{
    Q_OBJECT
public:
    numberWidget(QWidget* parent);

    //return the number which is shown on the widget
    int get_number() const
    {
        return number;
    }

    //set color of the number's rectangle
    void set_color(QColor color)
    {
        rectColor=color;
        update();
    }

    //return current color of widget's rectangle
    QColor get_color() const
    {
        mutex.lock();
        QColor c=rectColor;
        mutex.unlock();
        return c;
    }

    int get_nr() const
    {
        return nr;
    }

private:
    int number;
    QColor rectColor;

    //counter of created numberWidgets since start of application
    static int createdNumbers;

    //number indicating when widget was created
    //it's used for reseting order of nubers before sorting
    int nr;

protected:
    void paintEvent(QPaintEvent*);

};

#endif // NUMBERWIDGET_H
