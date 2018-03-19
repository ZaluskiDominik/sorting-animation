#ifndef CHANGENUMBERDIALOG_H
#define CHANGENUMBERDIALOG_H

#include <QDialog>

namespace Ui {
class changeNumberDialog;
}

class changeNumberDialog : public QDialog
{
    Q_OBJECT

public:
    explicit changeNumberDialog(QWidget *parent = 0);
    ~changeNumberDialog();

    void set_number(int a);

private:
    Ui::changeNumberDialog *ui;

private slots:
    void on_pushButton_clicked();

signals:
    void numberEdited(int num);
};

#endif // CHANGENUMBERDIALOG_H
