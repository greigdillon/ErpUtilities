#include "addressmodel.h"
#include "QSqlRecord"
#include "QSqlRelationalTableModel"
#include "QDebug"
#include <QtCore>
#include <QSqlError>
#include <QSqlField>
AddressModel::AddressModel(QObject *parent) : AbstractDataModel(parent)
{
    innerModel->setTable ("addr");
    innerModel->select ();
}
