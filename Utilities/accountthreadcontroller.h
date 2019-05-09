#ifndef ACCOUNTTHREADCONTROLLER_H
#define ACCOUNTTHREADCONTROLLER_H

#include "abstractthreadcontroller.h"

class AccountThreadController : public AbstractThreadController
{
    Q_OBJECT
public:
    explicit AccountThreadController(QObject *parent = nullptr);

signals:

public slots:

    // AbstractThreadController interface
protected:
    DatabaseToOperate database() const;
};

#endif // ACCOUNTTHREADCONTROLLER_H
