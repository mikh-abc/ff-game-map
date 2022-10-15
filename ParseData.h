#ifndef PARSEDATA_H
#define PARSEDATA_H

#include <QByteArray>
#include "DataDefines.h"

BaseType parseBaseType(uint id);
BaseType parseAnimalsSpawnType(const QByteArray& uuid);
RaiderType parseRaiderType(const QByteArray& uuid);
MineralType parseMineralType(quint32 v);
uint mineralTypeId(MineralType type);
GameItem parseItem(const QByteArray& v);

template<class T>
QByteArray readArray(QDataStream& in)
{
    T size;
    in >> size;
    QByteArray r;
    r.resize(size);
    in.readRawData(r.data(), size);
    return r;
}

QDataStream& operator>>(QDataStream& in, Point& rhs);
QDataStream& operator<<(QDataStream& out, const Point& rhs);

#endif // PARSEDATA_H
