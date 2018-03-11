#ifndef MOVINGWIDGET_H
#define MOVINGWIDGET_H

#include <QWidget>
#include <mutex>

extern std::mutex mutex;

enum class sortState{InProgress, Paused, NotMoving};

class movingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit movingWidget(QWidget *parent);

    //set speed of the moving animation(x1.5, x2.0 and so on)
    static void setSpeed(double speed)
    {
        mutex.lock();
        moveTime=initMoveTime / speed;
        mutex.unlock();
    }

    //end or pause moving
    void setState(sortState s)
    {
        mutex.lock();
        if (s==sortState::NotMoving && timeoutContinue==true)
            timeoutContinue=false;
        else
            state=s;
        mutex.unlock();
    }

    sortState getState()
    {
        mutex.lock();
        sortState res=state;
        mutex.unlock();
        return res;
    }

    //begin moving to given points
    void move(std::list<QPoint> _path);

private:
    //initial time between widget's moves
    static const int initMoveTime;
    //current time between widget's moves after applying sedSpeed() method
    static int moveTime;

    //distance the widget moves on each move
    static const int moveDistance;

    //whether moveTimeout() signal is continuing to be fired off
    bool timeoutContinue;

    //whether widget is moving or paused
    sortState state;

    //list of points that makes widget's path
    std::list<QPoint> path;

    //function in another thread that fire off moveTimeout() signals
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
