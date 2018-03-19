#include "changenumberdialog.h"
#include "ui_changenumberdialog.h"

changeNumberDialog::changeNumberDialog(QWidget *parent) :
    QDialog(parent, Qt::WindowCloseButtonHint | Qt::WindowTitleHint),
    ui(new Ui::changeNumberDialog)
{
    ui->setupUi(this);
}

changeNumberDialog::~changeNumberDialog()
{
    delete ui;
}

void changeNumberDialog::set_number(int a)
{
    ui->lineEdit->setText(QString::number(a));
}

void changeNumberDialog::on_pushButton_clicked()
{
    if (ui->lineEdit->text()!="")
    {
        emit numberEdited(ui->lineEdit->text().toInt());
        deleteLater();
    }
}
