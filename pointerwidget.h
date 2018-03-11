#ifndef POINTERWIDGET_H
#define POINTERWIDGET_H

#include "movingwidget.h"

class pointerWidget : public movingWidget
{
    Q_OBJECT
public:
    pointerWidget(QWidget* parent, QString ptrChar);

    inline void change_pos(int x, int y);

protected:
    void paintEvent(QPaintEvent*);

    QString character;

private slots:
    void onChangePosRequest(int x, int y);

signals:
    void changePosRequest(int x, int y);

};

#endif // POINTERWIDGET_H
