#ifndef MYPIC_H
#define MYPIC_H

#include <QWidget>
#include <QPixmap>
#include "ui_form.h"

class MyPic : public QWidget, public Ui::Form
{
Q_OBJECT
public:
    explicit MyPic(QWidget *parent = 0);

signals:

public slots:
    void myslot();
private:
     QStringList filenames;
     QPixmap *pics;

};

#endif // MYPIC_H
