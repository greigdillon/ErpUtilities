#ifndef CONTACTTHREADCONTROLLER_H
#define CONTACTTHREADCONTROLLER_H

#include "abstractthreadcontroller.h"

class ContactThreadController : public AbstractThreadController
{
    Q_OBJECT
public:
    explicit ContactThreadController(QObject *parent = nullptr);

signals:

public slots:

    // AbstractThreadController interface
protected:
    DatabaseToOperate database() const;
};

#endif // CONTACTTHREADCONTROLLER_H
