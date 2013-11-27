#ifndef MYCP_H
#define MYCP_H

#include <QWidget>
#include "ui_form.h"

class MyCp : public QWidget, public Ui::Form
{
Q_OBJECT
public:
    explicit MyCp(QWidget *parent = 0);

signals:


public slots:
    void myslot();
private:

    //int Size;

};

#endif // MYCP_H
