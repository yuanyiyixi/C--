#ifndef MYFORM_H
#define MYFORM_H

#include "ui_form.h"
#include <QWidget>

class MyWidget : public QWidget, public Ui::Form
{
    Q_OBJECT

public:
    MyWidget(QWidget *parent = 0);
public slots:
    void myslot();


private slots:
    void on_btn_out_clicked();
private:
    Ui::Form *ui;
};

#endif // MYFORM_H
