#ifndef MYTEST_H
#define MYTEST_H

#include <QObject>
#include <QtTest/QtTest>

class mytest : public QObject
{
    Q_OBJECT
public:
    explicit mytest(QObject *parent = nullptr);

private slots:
    void UiTest();

signals:

};

#endif // MYTEST_H
