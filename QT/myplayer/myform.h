#ifndef MYFORM_H
#define MYFORM_H

#include <QWidget>
#include "myplayer.h"

class MyForm : public QWidget
{
Q_OBJECT
public:
    explicit MyForm(QWidget *parent = 0);
    void mouseDoubleClickEvent(QMouseEvent * event);
    void mousePressEvent(QMouseEvent *event);
signals:

public slots:
    void mouseClick();

private:
    MyPlayer *parent_win;
    int flag;
    QTimer *timer;

};

#endif // MYFORM_H
