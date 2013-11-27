#ifndef MYTIME_H
#define MYTIME_H

#include <QWidget>
#include <QLCDNumber>

class MyTime : public QWidget
{
Q_OBJECT
public:
    explicit MyTime(QWidget *parent = 0);
private:
    QLCDNumber *lcd;

signals:

public slots:
    void showTime();

};

#endif // MYTIME_H
