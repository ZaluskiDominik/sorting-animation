#include "movingwidget.h"
#include <algorithm>
#include <ctime>
#include <thread>

std::mutex mutex;

const int movingWidget::moveDistance=2;
const int movingWidget::initMoveTime=40;
int movingWidget::moveTime=initMoveTime;

movingWidget::movingWidget(QWidget *parent)
    : QWidget(parent)
{
    state=sortState::NotMoving;
    QObject::connect(this, SIGNAL(moveTimeout()), this, SLOT(onMoveTimeout()));
}

void movingWidget::move(std::list<QPoint> _path)
{
    //set widget's moving path
    path=_path;
    timeoutContinue=true;
    setState(sortState::InProgress);

    //start move_thread() function in a new thread
    std::thread thr(move_thread, this);
    thr.detach();
}

void movingWidget::move_thread()
{
    int t1;

    //while widget don't stop moving
    while (isTimeoutContinuing())
    {
        //make a new move
        emit moveTimeout();

        //count time until firing off a new signal
        t1=clock();
        while (double(clock()-t1)/double(CLOCKS_PER_SEC)*1000.0<moveTime || getState()==sortState::Paused)
        {
            //if user wanted to stop sorting then end the thread
            if (!isTimeoutContinuing())
                break;
        }
    }
    //set flag that animation has ended
    set_state(sortState::NotMoving);
}

void movingWidget::onMoveTimeout()
{
    //if the widget is in the position of the first point from the path list
    if (x()==path.front().x() && y()==path.front().y())
    {
        //pop that point from the list
        path.pop_front();

        if (path.empty())
        {
            //end moving if that was the last point
            mutex.lock();
            timeoutContinue=false;
            mutex.unlock();
            return;
        }

        //if not then continue towards a new point
    }

    //current point
    QPoint p=path.front();

    int newX=x(), newY=y();
    //calculate a new widget's position
    if (p.x()!=x())
        newX=(p.x()>x()) ? x() + std::min(moveDistance, p.x() - x()) : x() + std::max(-moveDistance, p.x() - x());
    else
        newY=(p.y()>y()) ? y() + std::min(moveDistance, p.y() - y()) : y() + std::max(-moveDistance, p.y() - y());

    setGeometry(newX, newY, width(), height());
}
