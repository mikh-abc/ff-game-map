// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software distributed under the License
// is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
// or implied.  See the License for the specific language governing permissions and limitations
// under the License.

#ifndef STATICDATA_H
#define STATICDATA_H

#include <QByteArray>
#include "DataDefines.h"

constexpr uint ItemFillerSize = 417;

const std::vector<uint32_t>& getBaseTypes();
BaseType parseBaseType(uint32_t id);
BaseType parseAnimalsSpawnType(const QByteArray& uuid);
const QByteArray& convertAnimalsSpawnTypeToUUID(uint32_t difficult, BaseType baseType);
RaiderType parseRaiderType(const QByteArray& uuid);
MineralType parseMineralType(quint32 v);
uint mineralTypeId(MineralType type);
GameItem parseItem(const QByteArray& v);
DiseaseType parseDisease(const QByteArray& v);
QLatin1String mineralStr(MineralType v);
QLatin1String occupationStr(VillagerData::Occupation c);
QLatin1String educationStr(uint32_t e);
QLatin1String raiderStr(RaiderType e);

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

#endif // STATICDATA_H
