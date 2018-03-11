#ifndef INTERACTIVEBUTTON_H
#define INTERACTIVEBUTTON_H

#include <QAbstractButton>

class interactiveButton : public QAbstractButton
{
    Q_OBJECT
public:
    interactiveButton(QWidget* parent, QPixmap icon, QPixmap iconHov);

private:
    //button's icon
    QPixmap plainIcon;
    //button's icon while cursor is hovering over the button
    QPixmap hoverIcon;

    //whether cursor is inside button's rect
    bool cursorIn;

protected:
    void paintEvent(QPaintEvent*);

    //cursor entered button's rect
    void enterEvent(QEvent*);

    //cursor left button's rect
    void leaveEvent(QEvent*);

};

#endif // INTERACTIVEBUTTON_H
