#ifndef CONTACTMODEL_H
#define CONTACTMODEL_H

#include "abstractdatamodel.h"

class ContactModel : public AbstractDataModel
{
    Q_OBJECT
public:
    explicit ContactModel(QObject *parent = nullptr);

};

#endif // CONTACTMODEL_H
