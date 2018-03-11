#include "window.h"
#include "ui_window.h"
#include <thread>

window::window(QWidget *parent) :
    QDialog(parent, Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint),
    ui(new Ui::window)
{
    ui->setupUi(this);

    state=sortState::NotMoving;

    numbers.resize(10);
    //create numberWidgets and draw them
    for (auto& v : numbers)
        v=new numberWidget(this);
    draw_numbers();

    create_interactive_buttons();

    create_pointerWidgets();

    QObject::connect(ui->speedBox, SIGNAL(currentTextChanged(QString)), this, SLOT(onSpeedChanged()));
    QObject::connect(ui->spinBox, SIGNAL(valueChanged(int)), this, SLOT(onNumbersNumberChanged()));
    QObject::connect(this, SIGNAL(enableUI()), this, SLOT(onEnableUI()));
}

window::~window()
{
    delete ui;
}

void window::create_interactive_buttons()
{
    //create buttons
    playButton=new interactiveButton(this, QPixmap(":/img/img/play.png"), QPixmap(":/img/img/play_hover.png"));
    pauseButton=new interactiveButton(this, QPixmap(":/img/img/pause.png"), QPixmap(":/img/img/pause_hover.png"));
    reloadButton=new interactiveButton(this, QPixmap(":/img/img/reload.png"), QPixmap(":/img/img/reload_hover.png"));

    //set their position and size
    const int space=16;
    playButton->setGeometry(width()/2 - 96 - space, height() - 130, 64, 64);
    pauseButton->setGeometry(playButton->x() + space + 64, height() - 130, 64, 64);
    reloadButton->setGeometry(pauseButton->x() + space + 64, height() - 130, 64, 64);

    //connect to slots
    connect(playButton, SIGNAL(clicked(bool)), this, SLOT(onPlayClicked()));
    connect(pauseButton, SIGNAL(clicked(bool)), this, SLOT(onPauseClicked()));
    connect(reloadButton, SIGNAL(clicked(bool)), this, SLOT(onReloadClicked()));
}

void window::create_pointerWidgets()
{
    iPtr=new pointerWidget(this, "i");
    jPtr=new pointerWidget(this, "j");
    iPtr->setGeometry(100, 100, numbers[0]->width(), 60);
    jPtr->setGeometry(100, 200, numbers[0]->width(), 60);
    iPtr->setVisible(false);
    jPtr->setVisible(false);
}

void window::draw_numbers()
{
    const int w=60, h=90, space=20;
    for (unsigned int i=0 ; i<numbers.size() ; i++)
        numbers[i]->setGeometry(width()/2 - ((w + space) * (numbers.size()/2)) + ((w + space)*i), height()/2 - (h/2), w, h);
}

void window::start_animation()
{
    state=sortState::InProgress;

    if (ui->quickRadio->isChecked())
        quick_sort(0, numbers.size()-1);
    else if (ui->bubbleRadio->isChecked())
        bubble_sort();

    state=sortState::NotMoving;
    emit enableUI();
}

void window::swap_animation(numberWidget &a, numberWidget &b)
{
    if (state==sortState::NotMoving)
        return;

    const int ax=a.x(), ay=a.y();

    //change those widgets' color
    a.setColor(Qt::green);
    b.setColor(Qt::green);

    //start moving both widgets
    a.move( std::list<QPoint>({ QPoint(ax, ay + (1.2 * a.height())), QPoint(b.x(), ay + (1.2 * a.height())), QPoint(b.x(), b.y()) }) );
    b.move( std::list<QPoint>({ QPoint(b.x(), b.y() - (1.2 * b.height())), QPoint(ax, b.y() - (1.2 * b.height())), QPoint(ax, ay) }) );

    //wait till they swap and stop moving
    while (a.getState()!=sortState::NotMoving || b.getState()!=sortState::NotMoving);

    //reset theirs' color
    a.setColor(Qt::blue);
    b.setColor(Qt::blue);
}

void window::wait_for_stoppedState()
{
    unsigned int i;
    while (true)
    {
        for (i=0 ; i<numbers.size() && numbers[i]->getState()==sortState::NotMoving; i++);
        if (i==numbers.size())
            break;
    }
}

//*******************************************************************
//Sorting methods

void window::quick_sort(int l, int r)
{
    if (l>=r || state==sortState::NotMoving)
        return;

    //choose the middle number as a pivot
    const int piv=numbers[(l+r)/2]->getNumber();
    int i=l-1, j=r+1;
    while (i<j && state!=sortState::NotMoving)
    {
        while (numbers[++i]->getNumber() < piv)
        {
            //move i pointer
        }
        while (numbers[--j]->getNumber() > piv)
        {
            //move j pointer
        }

        if (i<j)
        {
            //start moving widgets
            swap_animation(*numbers[i], *numbers[j]);
            if (state!=sortState::NotMoving)
                std::swap(numbers[i], numbers[j]);
        }
    }

    quick_sort(l, j);
    quick_sort(j+1, r);
}

void window::bubble_sort()
{
    for (int i=numbers.size()-1 ; i ; i--)
    {
        for (int j=0 ; j<i ; j++)
        {
            if (numbers[j]->getNumber() > numbers[j+1]->getNumber())
            {
                swap_animation(*numbers[j], *numbers[j+1]);
                if (state!=sortState::NotMoving)
                    std::swap(numbers[j], numbers[j+1]);
            }
        }
    }
}

//**********************************************************************

void window::onPlayClicked()
{
    if (state==sortState::NotMoving)
    {
        //disable an option for changing the number of numberWidgets
        ui->spinBox->setDisabled(true);
        ui->groupBox->setDisabled(true);

        std::thread thread(start_animation, this);
        thread.detach();
    }
    else if (state==sortState::Paused)
    {
        state=sortState::InProgress;
        for (auto& v : numbers)
            if (v->getState()==sortState::Paused)
                v->setState(sortState::InProgress);
    }
}

void window::onPauseClicked()
{
    if (state==sortState::InProgress)
    {
        state=sortState::Paused;
        for (auto& v : numbers)
            if (v->getState()==sortState::InProgress)
                v->setState(sortState::Paused);
    }
}

void window::onReloadClicked()
{
    state=sortState::NotMoving;
    for (auto& v : numbers)
        v->setState(sortState::NotMoving);

    wait_for_stoppedState();

    //reset number's order from before sorting has occured
    std::sort(numbers.begin(), numbers.end(), [](numberWidget* a, numberWidget* b){ return a->getNr() < b->getNr(); });
    draw_numbers();
}

void window::onSpeedChanged()
{
    QString str=ui->speedBox->currentText();
    movingWidget::setSpeed(str=="Normal" ? 1.0 : str.mid(7, 10).toFloat());
}

void window::onNumbersNumberChanged()
{
    if (ui->spinBox->value()>numbers.size())
    {
        //add new number/numbers
        for (int i=numbers.size() ; i<ui->spinBox->value() ; i++)
        {
            numbers.push_back(new numberWidget(this));
            numbers.back()->show();
        }
    }
    else
    {
        //remove number/numbers
        for (int i=numbers.size() ; i>ui->spinBox->value() ; i--)
        {
            numbers.back()->deleteLater();
            numbers.pop_back();
        }
    }
    draw_numbers();
}

void window::onEnableUI()
{
    ui->spinBox->setEnabled(true);
    ui->groupBox->setEnabled(true);
}
