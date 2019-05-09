#ifndef ADDRESSTHREADCONTROLLER_H
#define ADDRESSTHREADCONTROLLER_H

#include "abstractthreadcontroller.h"

class AddressThreadController : public AbstractThreadController
{
    Q_OBJECT
public:
    explicit AddressThreadController(QObject *parent = nullptr);

    // AbstractThreadController interface
protected:
    DatabaseToOperate database() const;
};

#endif // ADDRESSTHREADCONTROLLER_H
