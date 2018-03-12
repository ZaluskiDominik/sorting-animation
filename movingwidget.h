#ifndef MOVINGWIDGET_H
#define MOVINGWIDGET_H

#include <QWidget>
#include <mutex>

extern std::mutex mutex;

//moving widget can be in 3 states
enum class sortState{InProgress, Paused, NotMoving};

class movingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit movingWidget(QWidget *parent);

    //set speed of the moving animation(x1.5, x2.0 and so on)
    static void set_speed(double speed)
    {
        mutex.lock();
        moveTime=initMoveTime / speed;
        mutex.unlock();
    }

    //stop, pause, reset moving
    void set_state(sortState s)
    {
        mutex.lock();
        if (s==sortState::NotMoving && timeoutContinue==true)
            timeoutContinue=false;  //set flag that thread should be ended, but don't change state yet
        else
            state=s;
        mutex.unlock();
    }

    //return current state of widget
    sortState get_state()
    {
        mutex.lock();
        sortState res=state;
        mutex.unlock();
        return res;
    }

    //begin moving to the given points
    void move(std::list<QPoint> _path);

private:
    //initial time between widget's moves
    static const int initMoveTime;
    //current time between widget's moves after applying sedSpeed() method
    static int moveTime;

    //distance which the widget moves on each move
    static const int moveDistance;

    //whether moveTimeout() signal is continuing to be fired off
    bool timeoutContinue;

    //current state of widget
    sortState state;

    //list of points that makes widget's path
    std::list<QPoint> path;

    //thread function that fire off moveTimeout() signals
    void move_thread();

    bool isTimeoutContinuing()
    {
        mutex.lock();
        bool res=timeoutContinue;
        mutex.unlock();
        return res;
    }

private slots:
    //each move is done there
    void onMoveTimeout();

signals:
    void moveTimeout();

};

#endif // MOVINGWIDGET_H
