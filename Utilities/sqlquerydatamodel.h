#ifndef SQLQUERYDATAMODEL_H
#define SQLQUERYDATAMODEL_H

#include<QtCore>
#include<QtSql>
#include "relation.h"
#include "abstractrecord.h"


class SqlQueryDataModel : public QObject
{
    Q_OBJECT
public:
    explicit SqlQueryDataModel(QObject *parent = nullptr);
    static QString createUuid();
    QSqlError lastError;

    QString getDefaultSort() const;
    void setDefaultSort(const QString &value);

    QString getDefaultFilter() const;
    void setDefaultFilter(const QString &value);

signals:
    void jsonSignal(QJsonArray json, QJsonObject lastRecord = QJsonObject());

    void recordInserted(QJsonObject record);
    void recordChanged(QJsonObject record);
    void recordDeleted(QJsonObject record);
    void recordsFetched(QJsonArray json);

    // AbstractDataModel interface
public slots:
    void modelAsJson();
    void modelAsJson(QSqlQuery &query);
//    QJsonObject selectRow(int row) const override;
    QJsonObject record();
    bool insertRecord(const QJsonObject &record);
    bool setRecord(const QJsonObject &record);
    bool removeRecord(const QJsonObject &record);
    void sort(QString fieldName, Qt::SortOrder order);
    void filter(QString value);
    void setRelation(const QString row, const QString &tableName, const QString &indexColumn, const QString &displayColumn);
    void setRelation(QJsonObject relation);

    void setTable(QString tableName);
    QString getFilterString(QString value, bool isLastRecord = false) ;
    void setLastRecord(QJsonObject record);;

private:
    QSqlQuery readAllQuery;
    QSqlQuery insertQuery;
    QString tableName;
    QString selectAll();
    QString selectAllWithRelations();
    QList<Relation> relations;
    QStringList originalFields;
    QStringList orignalFieldsNoPrepend;
    void setFields();
    QSqlDatabase db;
    QHash<QString,int> fieldsMapWithTable;
    QJsonObject setSystemParameters(QJsonObject record);
    QJsonObject lastRecord;
    QSqlRecord tableRecord;
    QString defaultSort;
    QString defaultFilter;
};

#endif // SQLQUERYDATAMODEL_H
