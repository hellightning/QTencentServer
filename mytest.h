/**
 * @file mytest.h
 * @brief 单元测试类
 */
#ifndef MYTEST_H
#define MYTEST_H

#include <QObject>
#include <QtTest/QtTest>

class mytest : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief mytest
     * @param 单元测试类构造函数
     */
    explicit mytest(QObject *parent = nullptr);

private slots:
    /**
     * @brief UiTest 测试Ui
     */
    void UiTest();

signals:

};

#endif // MYTEST_H
