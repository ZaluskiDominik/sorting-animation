#ifndef POINTERWIDGET_H
#define POINTERWIDGET_H

#include "movingwidget.h"

class pointerWidget : public movingWidget
{
    Q_OBJECT
public:
    pointerWidget(QWidget* parent, QString ptrChar);

    //immediately change widget's position
    void change_pos(int x, int y);

    bool position_changed()
    {
        mutex.lock();
        bool res=positionChanged;
        mutex.unlock();
        return res;
    }

protected:
    void paintEvent(QPaintEvent*);

private:
    //displayed name of the pointerWidget
    QString character;

    //tells if onChangePositionRequest() signal is already done
    bool positionChanged;

private slots:
    //signal fired off when change_pos() is called
    void onChangePosRequest(int x, int y);

signals:
    void changePosRequest(int x, int y);

};

#endif // POINTERWIDGET_H
