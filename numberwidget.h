#ifndef NUMBERWIDGET_H
#define NUMBERWIDGET_H
#include "movingwidget.h"

class numberWidget : public movingWidget
{
public:
    numberWidget(QWidget* parent);

    int getNumber()
    {
        return number;
    }

    //set color of the number's rectangle
    void setColor(QColor color)
    {
        rectColor=color;
        update();
    }

    int getNr()
    {
        return nr;
    }

private:
    int number;
    QColor rectColor;

    static int createdNumbers;

    //number indicating when widget was created
    int nr;

protected:
    void paintEvent(QPaintEvent*);

};

#endif // NUMBERWIDGET_H
