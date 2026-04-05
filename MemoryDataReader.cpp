// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software distributed under the License
// is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
// or implied.  See the License for the specific language governing permissions and limitations
// under the License.

#include "MemoryDataReader.h"
#include "StaticData.h"


std::vector<MineralData> MemoryDataReader::minerals()
{
    QByteArray& buf = seekDataType(BaseType::MineralManager);
    if (buf.isEmpty())
        return {};
    QDataStream in(&buf, QIODevice::ReadOnly);
    in.setByteOrder(QDataStream::LittleEndian);
    in.setFloatingPointPrecision(QDataStream::SinglePrecision);
    return readMinerals(in);
}

std::vector<ForageableData> MemoryDataReader::forageables()
{
    std::vector<ForageableData> r;
    for (int index = 0; ; ++index) {
        QByteArray& buf = seekDataType(BaseType::ForageableResource, index);
        if (buf.isEmpty())
            break;
        QDataStream in(&buf, QIODevice::ReadOnly);
        in.setByteOrder(QDataStream::LittleEndian);
        in.setFloatingPointPrecision(QDataStream::SinglePrecision);
        r.push_back(readForageable(in));
    }
    return r;
}

std::vector<RaiderData> MemoryDataReader::raiders()
{
    std::vector<RaiderData> r;
    for (int index = 0; ; ++index) {
        QByteArray& buf = seekDataType(BaseType::RaiderCamp, index);
        if (buf.isEmpty())
            break;
        QDataStream in(&buf, QIODevice::ReadOnly);
        in.setByteOrder(QDataStream::LittleEndian);
        in.setFloatingPointPrecision(QDataStream::SinglePrecision);
        r.push_back(readRaiderCamp(in));
    }
    for (int index = 0; ; ++index) {
        QByteArray& buf = seekDataType(BaseType::Raider, index);
        if (buf.isEmpty())
            break;
        QDataStream in(&buf, QIODevice::ReadOnly);
        in.setByteOrder(QDataStream::LittleEndian);
        in.setFloatingPointPrecision(QDataStream::SinglePrecision);
        r.push_back(readRaider(in));
    }
    for (int index = 0; ; ++index) {
        QByteArray& buf = seekDataType(BaseType::BatteringRam, index);
        if (buf.isEmpty())
            break;
        QDataStream in(&buf, QIODevice::ReadOnly);
        in.setByteOrder(QDataStream::LittleEndian);
        in.setFloatingPointPrecision(QDataStream::SinglePrecision);
        r.push_back(readRaider(in));
    }
    return r;
}

std::vector<BaseData> MemoryDataReader::animals()
{
    const BaseType list[] = { BaseType::Deer, BaseType::Bear, BaseType::Boar, BaseType::Wolf, BaseType::WolfDen };
    std::vector<BaseData> r;
    for (BaseType i : list) {
        for (int index = 0; ; ++index) {
            QByteArray& buf = seekDataType(i, index);
            if (buf.isEmpty())
                break;
            QDataStream in(&buf, QIODevice::ReadOnly);
            in.setByteOrder(QDataStream::LittleEndian);
            in.setFloatingPointPrecision(QDataStream::SinglePrecision);
            BaseData d;
            d.type = i;
            in.skipRawData(5);
            in >> d.p;
            r.push_back(d);
        }
    }
    return r;
}

std::unordered_map<uint32_t, BaseData> MemoryDataReader::sites()
{
    const BaseType list[] = { BaseType::TownCenter, BaseType::Shelter, BaseType::RelicExtraction, BaseType::SalvagingSite };
    std::unordered_map<uint32_t, BaseData> r;
    for (BaseType i : list) {
        for (int index = 0; ; ++index) {
            QByteArray& buf = seekDataType(i, index);
            if (buf.isEmpty())
                break;
            QDataStream in(&buf, QIODevice::ReadOnly);
            in.setByteOrder(QDataStream::LittleEndian);
            in.setFloatingPointPrecision(QDataStream::SinglePrecision);
            BaseData rd = readResource(in);
            rd.type = i;
            r.insert(std::pair<uint32_t, BaseData>(rd.id, rd));
        }
    }
    return r;
}

std::unordered_map<uint32_t, AnimalSpawnData> MemoryDataReader::animalsSpawns()
{
    QByteArray& buf = seekDataType(BaseType::AnimalManager);
    if (buf.isEmpty())
        return {};
    QDataStream in(&buf, QIODevice::ReadOnly);
    in.setByteOrder(QDataStream::LittleEndian);
    in.setFloatingPointPrecision(QDataStream::SinglePrecision);
    return readAnimalsSpawns(in);
}

std::vector<AnimalData> MemoryDataReader::deers()
{
    std::vector<AnimalData> r;
    for (int index = 0; ; ++index) {
        QByteArray& buf = seekDataType(BaseType::Deer, index);
        if (buf.isEmpty())
            break;
        QDataStream in(&buf, QIODevice::ReadOnly);
        in.setByteOrder(QDataStream::LittleEndian);
        in.setFloatingPointPrecision(QDataStream::SinglePrecision);
        r.push_back(readDeer(in));
    }

    return r;
}

GeneralSaveData MemoryDataReader::generalSaveData()
{
    QByteArray& buf = seekDataType(BaseType::MetaData);
    if (buf.isEmpty())
        return {};
    QDataStream in(&buf, QIODevice::ReadOnly);
    in.setByteOrder(QDataStream::LittleEndian);
    in.setFloatingPointPrecision(QDataStream::SinglePrecision);
    return readModMetaData(in);
}

AgricultureInfo::Data MemoryDataReader::agricultureData()
{
    QByteArray& buf = seekDataType(BaseType::AgricultureManager);
    if (buf.isEmpty())
        return {};
    QDataStream in(&buf, QIODevice::ReadOnly);
    in.setByteOrder(QDataStream::LittleEndian);
    in.setFloatingPointPrecision(QDataStream::SinglePrecision);
    return readAgricultureData(in);
}

TerrainData MemoryDataReader::terrainData()
{
    QByteArray& buf = seekDataType(BaseType::TerrainManager);
    if (buf.isEmpty())
        return {};
    QDataStream in(&buf, QIODevice::ReadOnly);
    in.setByteOrder(QDataStream::LittleEndian);
    in.setFloatingPointPrecision(QDataStream::SinglePrecision);
    return readTerrainData(in);
}

std::vector<VillagerData> MemoryDataReader::villagers()
{
    std::vector<VillagerData> r;
    for (int index = 0; ; ++index) {
        QByteArray& buf = seekDataType(BaseType::Villager, index);
        if (buf.isEmpty())
            break;
        QDataStream in(&buf, QIODevice::ReadOnly);
        in.setByteOrder(QDataStream::LittleEndian);
        in.setFloatingPointPrecision(QDataStream::SinglePrecision);
        r.push_back(readVillager(in));
    }
    return r;
}

Point MemoryDataReader::camera()
{
    Point r;
    memset(&r, 0, sizeof(Point));
    QByteArray& buf = seekDataType(BaseType::CameraManager);
    if (buf.isEmpty())
        return r;
    QDataStream in(&buf, QIODevice::ReadOnly);
    in.setByteOrder(QDataStream::LittleEndian);
    in.setFloatingPointPrecision(QDataStream::SinglePrecision);
    return readCamera(in);

}

ModData MemoryDataReader::modData()
{
    ModData r;
    if (modData_.isEmpty())
        return r;
    QDataStream in(&modData_, QIODevice::ReadOnly);
    in.setByteOrder(QDataStream::LittleEndian);
    in.setFloatingPointPrecision(QDataStream::SinglePrecision);
    in >> r.monthsPassed;
    in >> r.monthsToNextRaid;
    in >> r.monthsUntilRaidAttacksCanResume;
    uint8_t dateSize;
    in >> dateSize;
    for (uint i = 0; i < dateSize; ++i) {
        std::pair<uint32_t, uint32_t> date;
        in >> date.first;
        in >> date.second;
        r.previousRaidDates.emplace_back(std::move(date));
    }
    return r;
}

bool MemoryDataReader::appendData(const QByteArray& buf)
{
    socketBuffer_.append(buf);
    if (socketBuffer_.size() < sizeof(uint) * 3) {
        return false;
    }
    uint size = *reinterpret_cast<uint*>(socketBuffer_.data());
    int total = 0;
    while (socketBuffer_.size() - total >= size + sizeof(uint) * 3) {
        uint tag = *reinterpret_cast<uint*>(socketBuffer_.data() + total + sizeof(uint));
        uint index = *reinterpret_cast<uint*>(socketBuffer_.data() + total + 2 * sizeof(uint));
        bool next = (index & 0x80000000) == 0x80000000;
        index = index & 0x7FFFFFFF;
        auto b = socketBuffer_.mid(total + sizeof(uint) * 3, size);
        total += sizeof(uint) * 3 + size;
        size = *reinterpret_cast<uint*>(socketBuffer_.data() + total);
        if (tag == ModTag) {
            modData_ = std::move(b);
        } else {
            update(parseBaseType(tag), index, std::move(b));
        }
        if (!next) {
            waiting_.remove(tag);
        }
    }
    if (total == 0) {
        return false;
    }
    if (total == socketBuffer_.size()) {
        socketBuffer_.clear();
    } else {
        socketBuffer_ = socketBuffer_.mid(total, socketBuffer_.size() - total);
    }
    if (waiting_.empty()) {
        return true;
    }
    return false;
}

void MemoryDataReader::addWaitingData(const std::vector<uint>& tags, uint32_t modTag)
{
    waiting_.clear();
    waiting_.insert(modTag);
    modData_.clear();
    for (uint tag : tags) {
        waiting_.insert(tag);
        auto i = table_.find(parseBaseType(tag));
        if (i != table_.end()) {
            table_.erase(i);
        }
    }
}

void MemoryDataReader::update(BaseType baseType, uint index, QByteArray&& buf)
{
    auto i = table_.find(baseType);
    if (i == table_.end()) {
        i = table_.insert(baseType, QVector<QByteArray>());
    }
    auto& v = i.value();
    if (v.size() <= index) {
        v.insert(index, std::move(buf));
        return;
    }
    v.resize(index + 1);
    v.insert(index, std::move(buf));
}

QByteArray& MemoryDataReader::seekDataType(BaseType baseType, uint index)
{
    static QByteArray dummy;
    auto i = table_.find(baseType);
    if (i == table_.end()) {
        return dummy;
    }
    auto& v = i.value();
    if (v.size() <= index) {
        return dummy;
    }
    return v[index];
}
