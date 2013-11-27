#ifndef MYWIDGET_H
#define MYWIDGET_H

#include <QWidget>
#include "ui_form.h"

class MyWidget : public QWidget , public Ui::Form
{
Q_OBJECT
public:
    explicit MyWidget(QWidget *parent = 0);

signals:

public slots:
    void do_login();

};

#endif // MYWIDGET_H
