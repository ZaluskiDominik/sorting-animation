#ifndef WINDOW_H
#define WINDOW_H

#include <QDialog>
#include <vector>
#include "interactivebutton.h"
#include "numberwidget.h"
#include "pointerwidget.h"

namespace Ui {
class window;
}

class window : public QDialog
{
    Q_OBJECT

public:
    explicit window(QWidget *parent = 0);
    ~window();

private:
    Ui::window *ui;

    sortState state;

    //play, pause and reload buttons at the bottom of the window
    interactiveButton *playButton, *pauseButton, *reloadButton;

    pointerWidget *iPtr, *jPtr;

    std::vector<numberWidget*> numbers;

    void create_interactive_buttons();
    //draw number to the middle of the window

    //create 'i' and 'j' pointerWidgets
    void create_pointerWidgets();

    void draw_numbers();

    //start sorting numbers
    void start_animation();

    void swap_animation(numberWidget& a, numberWidget& b);

    void wait_for_stoppedState();

    //methods of sorting
    void quick_sort(int l, int r);

    void bubble_sort();

private slots:
    //interactive buttons' slots
    void onPlayClicked();
    void onPauseClicked();
    void onReloadClicked();

    //user selected a new speed from the combo box
    void onSpeedChanged();

    //spin box slot
    void onNumbersNumberChanged();

    void onEnableUI();

signals:
    void enableUI();

};

#endif // WINDOW_H
