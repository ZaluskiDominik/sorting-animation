#include "window.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    std::srand(time(0));
    QApplication a(argc, argv);
    window w;
    w.show();

    return a.exec();
}
