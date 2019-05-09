#ifndef ADDRESSMODEL_H
#define ADDRESSMODEL_H

#include "QSqlRelationalTableModel"
#include <QIdentityProxyModel>
#include <QtCore>
#include <QSqlRecord>
#include "abstractdatamodel.h"

class AddressModel : public AbstractDataModel
{
    Q_OBJECT

public:
    explicit AddressModel(QObject *parent = nullptr);

};

#endif // ADDRESSMODEL_H
