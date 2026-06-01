#ifndef SYSTEMSTATUSPROVIDER_H
#define SYSTEMSTATUSPROVIDER_H

#include <QJsonObject>
#include <QJsonValue>
#include <QString>

class SystemStatusProvider
{
public:
    SystemStatusProvider();

    void refresh();
    QJsonObject statusJson() const;

    static QString primaryLocalIpv4();
    static QString statusUrl(quint16 port);

private:
    struct CpuSample {
        quint64 idle = 0;
        quint64 total = 0;
        bool valid = false;
    };

    CpuSample readCpuSample() const;
    QJsonObject buildCpuObject();
    QJsonObject buildMemoryObject() const;
    QJsonObject buildHostObject() const;
    QJsonObject buildGpuObject() const;

    static double roundedPercent(double value);
    static QJsonValue nullableNumber(bool valid, double value);

    CpuSample m_lastCpuSample;
    QJsonObject m_status;
};

#endif // SYSTEMSTATUSPROVIDER_H
