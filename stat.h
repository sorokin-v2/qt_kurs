#ifndef STAT_H
#define STAT_H

#include <QDialog>
#include <QDate>
#include <QtConcurrent>
#include <QChart>
#include <QChartView>
#include <QGridLayout>
#include <QBarSet>
#include <QBarSeries>
#include <QBarCategoryAxis>
#include <QValueAxis>
#include <QLineSeries>

#include "database.h"

namespace Ui {
class Stat;
}

class Stat : public QDialog
{
    Q_OBJECT

public:
    explicit Stat(QWidget *parent = nullptr);   
    ~Stat();

    void SetData(DataBase*, const QString&, const QString&, const QDate);

public slots:

    void ReceiveStatusYearWorkloadRequest(const QSqlError, QSqlQuery*);
    void ReceiveStatusMonthWorkloadRequest(const QSqlError, QSqlQuery *);

private slots:
    void on_tw_statTab_currentChanged(int index);

    void on_cb_monthList_currentIndexChanged(int index);

private:

    void RequestMonthWorkload();

    Ui::Stat *ui;
    DataBase* dataBase {nullptr};
    QDate statDate = {QDate::currentDate()};
    QString airportName {""};
    QString airportCode {""};

    QChart* yearChart {nullptr};
    QChart* monthChart {nullptr};

    QChartView* yearChartView {nullptr};
    QChartView* monthChartView {nullptr};

    QGridLayout* yearLayout {nullptr};
    QGridLayout* monthLayout {nullptr};

    QLineSeries* lineSeries {nullptr};


};

#endif // STAT_H
