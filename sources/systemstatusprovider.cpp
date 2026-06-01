#include "systemstatusprovider.h"

#include <QAbstractSocket>
#include <QDateTime>
#include <QFile>
#include <QJsonValue>
#include <QNetworkAddressEntry>
#include <QNetworkInterface>
#include <QStringList>
#include <QSysInfo>
#include <QTextStream>
#include <QThread>

#ifdef Q_OS_WIN
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

#if defined(Q_OS_MACOS) || defined(Q_OS_MAC)
#include <mach/host_info.h>
#include <mach/mach.h>
#include <mach/mach_host.h>
#include <sys/sysctl.h>
#endif

SystemStatusProvider::SystemStatusProvider()
{
    refresh();
}

void SystemStatusProvider::refresh()
{
    QJsonObject root;
    root.insert("app", "RYCOM");
    root.insert("timestamp", QDateTime::currentDateTime().toString(Qt::ISODate));
    root.insert("host", buildHostObject());
    root.insert("cpu", buildCpuObject());
    root.insert("memory", buildMemoryObject());
    root.insert("gpu", buildGpuObject());
    m_status = root;
}

QJsonObject SystemStatusProvider::statusJson() const
{
    return m_status;
}

QString SystemStatusProvider::primaryLocalIpv4()
{
    QString firstAddress;

    const QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    for (const QNetworkInterface &iface : interfaces) {
        const QNetworkInterface::InterfaceFlags flags = iface.flags();
        if (!(flags & QNetworkInterface::IsUp) ||
            !(flags & QNetworkInterface::IsRunning) ||
            (flags & QNetworkInterface::IsLoopBack)) {
            continue;
        }

        const QList<QNetworkAddressEntry> entries = iface.addressEntries();
        for (const QNetworkAddressEntry &entry : entries) {
            const QHostAddress address = entry.ip();
            if (address.protocol() != QAbstractSocket::IPv4Protocol || address.isLoopback()) {
                continue;
            }

            const QString ip = address.toString();
            if (firstAddress.isEmpty()) {
                firstAddress = ip;
            }

            const QStringList parts = ip.split('.');
            if (parts.size() != 4) {
                continue;
            }

            const int a = parts.at(0).toInt();
            const int b = parts.at(1).toInt();
            const bool privateAddress =
                    a == 10 ||
                    (a == 172 && b >= 16 && b <= 31) ||
                    (a == 192 && b == 168);
            if (privateAddress) {
                return ip;
            }
        }
    }

    if (!firstAddress.isEmpty()) {
        return firstAddress;
    }
    return "127.0.0.1";
}

QString SystemStatusProvider::statusUrl(quint16 port)
{
    return QString("http://%1:%2/status").arg(primaryLocalIpv4()).arg(port);
}

SystemStatusProvider::CpuSample SystemStatusProvider::readCpuSample() const
{
    CpuSample sample;

#ifdef Q_OS_WIN
    FILETIME idleTime;
    FILETIME kernelTime;
    FILETIME userTime;
    if (!GetSystemTimes(&idleTime, &kernelTime, &userTime)) {
        return sample;
    }

    ULARGE_INTEGER idle;
    idle.LowPart = idleTime.dwLowDateTime;
    idle.HighPart = idleTime.dwHighDateTime;

    ULARGE_INTEGER kernel;
    kernel.LowPart = kernelTime.dwLowDateTime;
    kernel.HighPart = kernelTime.dwHighDateTime;

    ULARGE_INTEGER user;
    user.LowPart = userTime.dwLowDateTime;
    user.HighPart = userTime.dwHighDateTime;

    sample.idle = idle.QuadPart;
    sample.total = kernel.QuadPart + user.QuadPart;
    sample.valid = true;
#elif defined(Q_OS_LINUX)
    QFile file("/proc/stat");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return sample;
    }

    const QByteArray line = file.readLine();
    const QList<QByteArray> fields = line.simplified().split(' ');
    if (fields.size() < 8 || fields.at(0) != "cpu") {
        return sample;
    }

    quint64 values[10] = {0};
    const int count = qMin(fields.size() - 1, 10);
    for (int i = 0; i < count; ++i) {
        values[i] = fields.at(i + 1).toULongLong();
    }

    const quint64 idle = values[3] + values[4];
    const quint64 nonIdle = values[0] + values[1] + values[2] + values[5] + values[6] + values[7];
    sample.idle = idle;
    sample.total = idle + nonIdle;
    sample.valid = true;
#elif defined(Q_OS_MACOS) || defined(Q_OS_MAC)
    host_cpu_load_info_data_t cpuInfo;
    mach_msg_type_number_t count = HOST_CPU_LOAD_INFO_COUNT;
    if (host_statistics(mach_host_self(), HOST_CPU_LOAD_INFO,
                        reinterpret_cast<host_info_t>(&cpuInfo), &count) != KERN_SUCCESS) {
        return sample;
    }

    const quint64 user = cpuInfo.cpu_ticks[CPU_STATE_USER];
    const quint64 nice = cpuInfo.cpu_ticks[CPU_STATE_NICE];
    const quint64 system = cpuInfo.cpu_ticks[CPU_STATE_SYSTEM];
    const quint64 idle = cpuInfo.cpu_ticks[CPU_STATE_IDLE];

    sample.idle = idle;
    sample.total = user + nice + system + idle;
    sample.valid = true;
#endif

    return sample;
}

QJsonObject SystemStatusProvider::buildCpuObject()
{
    QJsonObject cpu;
    const CpuSample sample = readCpuSample();
    bool usageValid = false;
    double usage = 0.0;

    if (sample.valid && m_lastCpuSample.valid && sample.total > m_lastCpuSample.total) {
        const quint64 totalDelta = sample.total - m_lastCpuSample.total;
        const quint64 idleDelta = sample.idle - m_lastCpuSample.idle;
        if (totalDelta > 0 && idleDelta <= totalDelta) {
            usage = (static_cast<double>(totalDelta - idleDelta) * 100.0) /
                    static_cast<double>(totalDelta);
            usageValid = true;
        }
    }

    if (sample.valid) {
        m_lastCpuSample = sample;
    }

    cpu.insert("usagePercent", nullableNumber(usageValid, roundedPercent(usage)));
    cpu.insert("temperatureCelsius", QJsonValue::Null);
    cpu.insert("logicalProcessors", QThread::idealThreadCount());
    return cpu;
}

QJsonObject SystemStatusProvider::buildMemoryObject() const
{
    QJsonObject memory;
    quint64 totalBytes = 0;
    quint64 availableBytes = 0;
    bool valid = false;

#ifdef Q_OS_WIN
    MEMORYSTATUSEX status;
    ZeroMemory(&status, sizeof(status));
    status.dwLength = sizeof(status);
    if (GlobalMemoryStatusEx(&status)) {
        totalBytes = status.ullTotalPhys;
        availableBytes = status.ullAvailPhys;
        valid = true;
    }
#elif defined(Q_OS_LINUX)
    QFile file("/proc/meminfo");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        while (!file.atEnd()) {
            const QByteArray line = file.readLine();
            const QList<QByteArray> fields = line.simplified().split(' ');
            if (fields.size() < 2) {
                continue;
            }
            if (fields.at(0) == "MemTotal:") {
                totalBytes = fields.at(1).toULongLong() * 1024ULL;
            } else if (fields.at(0) == "MemAvailable:") {
                availableBytes = fields.at(1).toULongLong() * 1024ULL;
            }
        }
        valid = totalBytes > 0 && availableBytes > 0;
    }
#elif defined(Q_OS_MACOS) || defined(Q_OS_MAC)
    int64_t total = 0;
    size_t totalSize = sizeof(total);
    if (sysctlbyname("hw.memsize", &total, &totalSize, nullptr, 0) == 0) {
        vm_statistics64_data_t vmstat;
        mach_msg_type_number_t count = HOST_VM_INFO64_COUNT;
        if (host_statistics64(mach_host_self(), HOST_VM_INFO64,
                              reinterpret_cast<host_info64_t>(&vmstat), &count) == KERN_SUCCESS) {
            vm_size_t pageSize = 0;
            host_page_size(mach_host_self(), &pageSize);
            totalBytes = static_cast<quint64>(total);
            availableBytes = static_cast<quint64>(vmstat.free_count) * static_cast<quint64>(pageSize);
            valid = totalBytes > 0;
        }
    }
#endif

    if (!valid || totalBytes == 0 || availableBytes > totalBytes) {
        memory.insert("totalBytes", QJsonValue::Null);
        memory.insert("usedBytes", QJsonValue::Null);
        memory.insert("availableBytes", QJsonValue::Null);
        memory.insert("usagePercent", QJsonValue::Null);
        return memory;
    }

    const quint64 usedBytes = totalBytes - availableBytes;
    const double usage = static_cast<double>(usedBytes) * 100.0 / static_cast<double>(totalBytes);

    memory.insert("totalBytes", static_cast<double>(totalBytes));
    memory.insert("usedBytes", static_cast<double>(usedBytes));
    memory.insert("availableBytes", static_cast<double>(availableBytes));
    memory.insert("usagePercent", roundedPercent(usage));
    return memory;
}

QJsonObject SystemStatusProvider::buildHostObject() const
{
    QJsonObject host;
    host.insert("name", QSysInfo::machineHostName());
    host.insert("os", QSysInfo::prettyProductName());
    host.insert("kernelType", QSysInfo::kernelType());
    host.insert("kernelVersion", QSysInfo::kernelVersion());
    host.insert("localIp", primaryLocalIpv4());
    return host;
}

QJsonObject SystemStatusProvider::buildGpuObject() const
{
    QJsonObject gpu;
    gpu.insert("name", QJsonValue::Null);
    gpu.insert("usagePercent", QJsonValue::Null);
    gpu.insert("temperatureCelsius", QJsonValue::Null);
    gpu.insert("note", "GPU status is not available without a vendor sensor API");
    return gpu;
}

double SystemStatusProvider::roundedPercent(double value)
{
    if (value < 0.0) {
        value = 0.0;
    } else if (value > 100.0) {
        value = 100.0;
    }
    return static_cast<int>(value * 10.0 + 0.5) / 10.0;
}

QJsonValue SystemStatusProvider::nullableNumber(bool valid, double value)
{
    if (!valid) {
        return QJsonValue::Null;
    }
    return QJsonValue(value);
}
