#include "window.h"
#include "ui_window.h"
#include <thread>

window::window(QWidget *parent) :
    QDialog(parent, Qt::WindowTitleHint | Qt::WindowCloseButtonHint | Qt::WindowMinimizeButtonHint),
    ui(new Ui::window)
{
    ui->setupUi(this);
    setWindowTitle("Sorting animation");

    set_state(sortState::NotMoving);

    numbers.resize(ui->spinBox->value());
    //create numberWidgets and draw them
    for (auto& v : numbers)
    {
        v=new numberWidget(this);
        QObject::connect(v, SIGNAL(numberDeleted()), this, SLOT(onNumberDeleted()));
    }
    draw_numbers();

    create_interactive_buttons();

    create_pointerWidgets();

    //connect all slots
    QObject::connect(ui->speedBox, SIGNAL(currentTextChanged(QString)), this, SLOT(onSpeedChanged()));
    QObject::connect(ui->spinBox, SIGNAL(valueChanged(int)), this, SLOT(onNumbersNumberChanged()));
    QObject::connect(this, SIGNAL(sortThreadEnded()), this, SLOT(onSortThreadEnded()));

    QObject::connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(onQuit()));
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

void window::draw_numbers()
{
    const int w=80, h=90, space=20;
    for (unsigned int i=0 ; i<numbers.size() ; i++)
        numbers[i]->setGeometry(width()/2 -  (( (space + w) * numbers.size() - space )/2) + ((w + space)*i), height()/2 - (h/2), w, h);
}

//***********************************************************************************************
//PointerWidgets methods

void window::create_pointerWidgets()
{
    iPtr=new pointerWidget(this, "i");
    jPtr=new pointerWidget(this, "j");
    iPtr->setGeometry(100, 100, numbers[0]->width(), numbers[0]->height());
    jPtr->setGeometry(100, 200, numbers[0]->width(), numbers[0]->height());
    set_pointers_visible(false);
}

void window::set_pointers_visible(bool yesNo)
{
    iPtr->setVisible(yesNo);
    jPtr->setVisible(yesNo);
}

void window::change_pointers_pos(int x1, int x2)
{
    iPtr->change_pos(numbers[x1]->x(), numbers[x1]->y() + (1.2 * numbers[x1]->height()));
    jPtr->change_pos(numbers[x2]->x(), numbers[x2]->y() - (1.2 * numbers[x2]->height()));
    while (iPtr->position_changed()==false || jPtr->position_changed()==false);
}

void window::move_pointer_widget(pointerWidget *p, int x)
{
    if (get_state()==sortState::NotMoving)
        return;

    p->move(std::list<QPoint>( {QPoint(numbers[x]->x(), p->y())} ));
    while(p->get_state()!=sortState::NotMoving);
}

void window::set_ptr_state_if(sortState state, sortState thisState)
{
    if (iPtr->get_state()==thisState)
        iPtr->set_state(state);
    if (jPtr->get_state()==thisState)
        jPtr->set_state(state);
}

//***********************************************************************************************

void window::start_animation()
{
    set_state(sortState::InProgress);

    if (ui->quickRadio->isChecked())
        quick_sort(0, numbers.size()-1);
    else if (ui->bubbleRadio->isChecked())
        bubble_sort();
    else if (ui->selectionRadio->isCheckable())
        selection_sort();

    set_state(sortState::NotMoving);
    emit sortThreadEnded();
}

void window::swap_animation(numberWidget &a, numberWidget &b)
{
    if (get_state()==sortState::NotMoving)
        return;

    //save position of first widget
    const int ax=a.x(), ay=a.y();
    //save both widgets' color
    const QColor aColor=a.get_color(), bColor=b.get_color();

    //change those both widgets' color
    a.set_color(Qt::green);
    b.set_color(Qt::green);

    //start moving both widgets
    a.move( std::list<QPoint>({ QPoint(ax, ay + (1.2 * a.height())), QPoint(b.x(), ay + (1.2 * a.height())), QPoint(b.x(), b.y()) }) );
    b.move( std::list<QPoint>({ QPoint(b.x(), b.y() - (1.2 * b.height())), QPoint(ax, b.y() - (1.2 * b.height())), QPoint(ax, ay) }) );

    //wait till they swap, then stop moving
    while (a.get_state()!=sortState::NotMoving || b.get_state()!=sortState::NotMoving);

    //reset theirs' color
    a.set_color(aColor);
    b.set_color(bColor);
}

void window::wait_for_stoppedState()
{
    unsigned int i;
    while (true)
    {
        for (i=0 ; i<numbers.size() && numbers[i]->get_state()==sortState::NotMoving; i++);
        //if all widgets are not moving then break the loop
        if (i==numbers.size() && iPtr->get_state()==sortState::NotMoving && jPtr->get_state()==sortState::NotMoving && iPtr->position_changed() && jPtr->position_changed())
            break;
    }
}

void window::enable_UI(bool yesNo)
{
    ui->spinBox->setEnabled(yesNo);
    ui->groupBox->setEnabled(yesNo);
}

void window::mswait(int time)
{
    int t=clock();
    while(double(clock()-t)/double(CLOCKS_PER_SEC)*1000<time || get_state()==sortState::Paused)
    {
        //if animation was ended by user then break the loop
        if (get_state()==sortState::NotMoving)
            break;
    }
}

//*******************************************************************
//Sorting methods

void window::quick_sort(int l, int r)
{
    if (l>=r || get_state()==sortState::NotMoving)
        return;

    //set pointerWidgets on each end of sorted part of an array
    change_pointers_pos(l, r);
    mswait(500);

    //choose the middle number as a pivot
    numberWidget& piv=*numbers[(l+r)/2];
    numbers[(l+r)/2]->set_color(Qt::red);
    mswait(500);

    int i=l-1, j=r+1;
    while (i<j && get_state()!=sortState::NotMoving)
    {
        while (numbers[++i]->get_number() < piv.get_number());
        //move i pointer
        move_pointer_widget(iPtr, i);
        mswait(500);

        while (numbers[--j]->get_number() > piv.get_number());
        //move j pointer
        move_pointer_widget(jPtr, j);
        mswait(500);

        if (i<j)
        {
            //start moving widgets
            swap_animation(*numbers[i], *numbers[j]);
            if (get_state()!=sortState::NotMoving)
                std::swap(numbers[i], numbers[j]);
        }
    }
    piv.set_color(Qt::blue);

    quick_sort(l, j);
    quick_sort(j+1, r);
}

void window::bubble_sort()
{
    change_pointers_pos(0, numbers.size()-1);
    for (int i=numbers.size()-1 ; i ; i--)
    {
        mswait(500);
        for (int j=0 ; j<i ; j++)
        {
            if (numbers[j]->get_number() > numbers[j+1]->get_number())
            {
                swap_animation(*numbers[j], *numbers[j+1]);
                if (get_state()!=sortState::NotMoving)
                    std::swap(numbers[j], numbers[j+1]);
            }
            move_pointer_widget(iPtr, j+1);
            mswait(500);
        }
        change_pointers_pos(0, i);
        move_pointer_widget(jPtr, i-1);
    }
}

void window::selection_sort()
{
    int i, j, max;
    change_pointers_pos(0, numbers.size()-1);
    for (j=numbers.size()-1 ; j && get_state()!=sortState::NotMoving ; j--)
    {
        max=0;
        numbers[0]->set_color(Qt::red);

        for (i=1 ; i<=j ; i++)
        {
            move_pointer_widget(iPtr, i);
            if (numbers[i]->get_number() > numbers[max]->get_number())
            {
                numbers[max]->set_color(Qt::blue);
                max=i;
                numbers[i]->set_color(Qt::red);
            }
            mswait(500);
        }

        numbers[max]->set_color(Qt::blue);
        if (max!=j)
        {
            swap_animation(*numbers[max], *numbers[j]);
            if (get_state()!=sortState::NotMoving)
                std::swap(numbers[max], numbers[j]);
        }

        change_pointers_pos(0, j);
        move_pointer_widget(jPtr, j-1);
    }
}

//**********************************************************************

void window::onPlayClicked()
{
    if (get_state()==sortState::NotMoving)
    {
        //sorting not started
        set_pointers_visible(true);
        //disable an option for changing the number of numberWidgets
        enable_UI(false);

        //start sorting
        std::thread thread(start_animation, this);
        thread.detach();
    }
    else if (get_state()==sortState::Paused)
    {
        //sorting paused --> restart it
        set_state(sortState::InProgress);
        for (auto& v : numbers)
            if (v->get_state()==sortState::Paused)
                v->set_state(sortState::InProgress);
        set_ptr_state_if(sortState::InProgress, sortState::Paused);
    }
}

void window::onPauseClicked()
{
    //if sorting is in progress then pause it
    if (get_state()==sortState::InProgress)
    {
        set_state(sortState::Paused);
        for (auto& v : numbers)
            if (v->get_state()==sortState::InProgress)
                v->set_state(sortState::Paused);
        set_ptr_state_if(sortState::Paused, sortState::InProgress);
    }
}

void window::onReloadClicked()
{
    //stop sorting
    set_state(sortState::NotMoving);
    for (auto& v : numbers)
        v->set_state(sortState::NotMoving);

    iPtr->set_state(sortState::NotMoving);
    jPtr->set_state(sortState::NotMoving);

    wait_for_stoppedState();

    //reset number's order from before sorting has occured
    std::sort(numbers.begin(), numbers.end(), [](numberWidget* a, numberWidget* b){ return a->get_nr() < b->get_nr(); });
    draw_numbers();
}

void window::onSpeedChanged()
{
    QString str=ui->speedBox->currentText();
    movingWidget::set_speed(str=="Normal" ? 1.0 : str.mid(7, 10).toFloat());
}

void window::onNumbersNumberChanged()
{
    if (ui->spinBox->value() > numbers.size())
    {
        //add new number/numbers
        for (int i=numbers.size() ; i<ui->spinBox->value() ; i++)
        {
            numbers.push_back(new numberWidget(this));
            numbers.back()->show();
            QObject::connect(numbers.back(), SIGNAL(numberDeleted()), this, SLOT(onNumberDeleted()));
        }
    }
    else if (ui->spinBox->value() < numbers.size())
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

void window::onSortThreadEnded()
{
    enable_UI(true);
    set_pointers_visible(false);
}

void window::onNumberDeleted()
{
    if (get_state()==sortState::NotMoving && numbers.size() > ui->spinBox->minimum())
    {
        auto num=qobject_cast<numberWidget*>(sender());
        int i;

        for (i=0 ; numbers[i]!=num ; i++);
        for ( ; i<numbers.size()-1 ; i++)
            numbers[i]=numbers[i+1];

        num->deleteLater();
        numbers.pop_back();
        ui->spinBox->setValue(numbers.size());
        //spinBox's signal will redraw numbers
    }
}

void window::onQuit()
{
    onReloadClicked();
}
