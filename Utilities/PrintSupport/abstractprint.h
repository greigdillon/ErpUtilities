#ifndef ABSTRACTPRINT_H
#define ABSTRACTPRINT_H

#include "QtCore"

class AbstractPrint : public QThread
{
    Q_OBJECT

public:
    explicit AbstractPrint(QObject *parent = nullptr);
    ~AbstractPrint();

signals:

public slots:
    void openPrintDialog();

private:
    QMutex mutex;
    QWaitCondition condition;
    bool abort;
    bool restart;

    // QThread interface
protected:
    void run() override;
};

#endif // ABSTRACTPRINT_H
