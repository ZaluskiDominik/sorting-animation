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

    //play, pause and reload buttons at the bottom of the window
    interactiveButton *playButton, *pauseButton, *reloadButton;

    //visible pointers representing iteration variable
    pointerWidget *iPtr, *jPtr;

    std::vector<numberWidget*> numbers;

    //FUNCTIONS

    void create_interactive_buttons();
    //draw number to the middle of the window

    //draw numberWidgets in the middle of the window
    void draw_numbers();

    //************************************************
    //Pointers widget

    //create 'i' and 'j' pointerWidgets
    void create_pointerWidgets();

    void set_pointers_visible(bool yesNo);

    //change position of pointers 'i' and 'j' based on numbers vector's index
    void change_pointers_pos(int x1, int x2);

    //animated move
    void move_pointer_widget(pointerWidget* p, int x);

    void set_ptr_state_if(sortState state, sortState thisState);

    //*************************************************

    //start sorting numbers
    void start_animation();

    void swap_animation(numberWidget& a, numberWidget& b);

    //wait until numberWidgets's thread end
    void wait_for_stoppedState();

    void enable_UI(bool yesNo);

    void mswait(int time);

    //**************************************************
    //methods of sorting
    void quick_sort(int l, int r);

    void bubble_sort();

    void selection_sort();

    //**************************************************

private slots:
    //interactive buttons' slots
    void onPlayClicked();
    void onPauseClicked();
    void onReloadClicked();

    //user selected a new speed from the combo box
    void onSpeedChanged();

    //spin box slot
    void onNumbersNumberChanged();

    //enable using ui elements after the end of sorting
    void onSortThreadEnded();

    //delete was clicked in context menu
    void onNumberDeleteClicked();

    //slot called before ending the app
    void onQuit();

signals:
    void sortThreadEnded();

};

#endif // WINDOW_H
