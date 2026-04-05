// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software distributed under the License
// is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
// or implied.  See the License for the specific language governing permissions and limitations
// under the License.

#include "stdafx.h"
#include "GameMapChanger.h"
#include "StaticData.h"

namespace {
    template<class T>
    T copyArray(QDataStream& in, QDataStream& out)
    {
        T size;
        in >> size;
        QByteArray r;
        r.resize(size);
        in.readRawData(r.data(), size);
        out << size;
        out.writeRawData(r.data(), size);
        return size;
    }

    template<class T>
    T skipArray(QDataStream& in)
    {
        T size;
        in >> size;
        QByteArray r;
        r.resize(size);
        in.readRawData(r.data(), size);
        return size;
    }
}

GameMapChanger::GameMapChanger(const Options &options)
    : options_(options)
{
}

bool GameMapChanger::copy(const QString& from, const QString& to, const PendingData& pendingData)
{
    QFile toFile(to);
    QFile fromFile(from);
    if (toFile.fileName() == fromFile.fileName()) {
        return false;
    }
    if (!toFile.open(QIODevice::WriteOnly) || !fromFile.open(QIODevice::ReadOnly)) {
        return false;
    }

    QDataStream in(&fromFile);
    in.setByteOrder(QDataStream::LittleEndian);
    QDataStream out(&toFile);
    out.setByteOrder(QDataStream::LittleEndian);

    std::unordered_map<QByteArray, std::set<uint32_t>> removed;
    QByteArray lastRemoved;
    int32_t lastRemovedid = 0;
    int32_t removedCounter = -1;
    while(!in.atEnd()) {
        quint8 componentType;
        in >> componentType;
        QByteArray field = readArray<quint8>(in);
        uint fieldSize;
        in >> fieldSize;
        uint id = 0;
        in >> id;
        QByteArray buf(fieldSize - 4, Qt::Uninitialized);
        in.readRawData(buf.data(), fieldSize - 4);

        bool removeField = false;
        switch (parseBaseType(id)) {
        case BaseType::Guids: {
            auto i = removed.find(field.left(field.size() - 5));
            if (i != removed.end()) {
                handleGuids(buf, i->second);
            }
            break;
        }
        case BaseType::FoWSystem:
            if (options_.removeFoW) {
                for (int i = 0; i < 512; ++i) {
                    for (int j = 0; j < 512; ++j) {
                        buf[(1 + i + j * 512) * 4 + 1] = 0xff;
                        buf[(1 + i + j * 512) * 4 + 2] = 0xff;
                    }
                }
            }
            break;
        case BaseType::MetaData:
            if (!options_.name.isEmpty() || options_.pacifist != 1 || options_.difficulty != -1) {
                handleMetaData(buf);
            }
            break;
        case BaseType::BuildingBuildSite:
            removeField = options_.removeBuildingSites;
            break;
        case BaseType::FlattenAreaBuildsite:
            if (options_.finishFlattenAreas) {
                handleFlattenAreas(buf);
            }
            break;

        case BaseType::MineralManager:
            if (!pendingData.newMinerals.empty() || options_.doubleMinerals) {
                handleMineralManager(buf, pendingData.newMinerals);
            }
            break;
        case BaseType::Villager:
            if (options_.cureVillagers) {
                handleVillager(buf);
            }
            break;
        case BaseType::AnimalManager: {
            if (pendingData.changedSpawns) {
                handleAnimalManager(buf, pendingData.spawns);
            }
            break;
        }
        case BaseType::Bear:
        case BaseType::Boar:
        case BaseType::Wolf:
        case BaseType::Deer: {
            if (pendingData.changedSpawns) {
                removeField = handleAnimal(buf, pendingData.spawns);
            }
            break;
        }
        case BaseType::AgricultureManager:
        case BaseType::BatteringRam:
        case BaseType::CameraManager:
        case BaseType::ForageableResource:
        case BaseType::PotentialImmigrant:
        case BaseType::Raider:
        case BaseType::RaiderCamp:
        case BaseType::RelicExtraction:
        case BaseType::SalvagingSite:
        case BaseType::Shelter:
        case BaseType::TerrainManager:
        case BaseType::TimeManager:
        case BaseType::TownCenter:
        case BaseType::WolfDen:
        case BaseType::Unknown:
            break;
        }
        if (!removeField) {
            if (!lastRemoved.isEmpty()) {
                if (removedCounter != -1 && lastRemovedid == id) {
                    field = lastRemoved + QByteArray::number(removedCounter);
                    ++removedCounter;
                } else {
                    lastRemoved.clear();
                    lastRemovedid = 0;
                }
            }

            fieldSize = buf.size() + 4;
            out << componentType;
            out << quint8(field.size());
            out.writeRawData(field.data(), field.size());
            out << fieldSize;
            out << id;
            out.writeRawData(buf.data(), buf.size());
        } else {
            for (int i = 0; i < field.size(); ++i) {
                if (std::isdigit(field[i])) {
                    uint32_t index = field.sliced(i, field.size() - i).toInt();
                    if (removedCounter == -1) {
                        removedCounter = index;
                    }
                    lastRemovedid = id;
                    lastRemoved = field.left(i);
                    removed[lastRemoved].insert(index);
                    break;
                }
            }
        }
    }
    return true;

}

bool GameMapChanger::handleAnimal(QByteArray& buf, const std::unordered_map<uint32_t, AnimalSpawnData>& spawns)
{
    int32_t spawnArea = *reinterpret_cast<int32_t*>(buf.data() + buf.size() - sizeof(uint32_t) - 1);
    if (spawnArea > 0) {
        if (spawns.find(spawnArea) == spawns.end()) {
            return true;
        }
    }
    return false;
}

void GameMapChanger::handleAnimalManager(QByteArray& buf, const std::unordered_map<uint32_t, AnimalSpawnData>& spawns)
{
    QBuffer outBuf;
    outBuf.open(QIODeviceBase::WriteOnly);
    QDataStream fout(&outBuf);
    QDataStream fin(&buf, QIODeviceBase::ReadOnly);
    fout.setByteOrder(QDataStream::LittleEndian);
    fout.setFloatingPointPrecision(QDataStream::SinglePrecision);
    fin.setByteOrder(QDataStream::LittleEndian);
    fin.setFloatingPointPrecision(QDataStream::SinglePrecision);

    QByteArray tmp(2, Qt::Uninitialized);
    fin.readRawData(tmp.data(), 2);
    fout.writeRawData(tmp.data(), 2);

    quint32 herdCount;
    fin >> herdCount;
    fout << herdCount;

    for (uint i = 0; i < herdCount; ++i) {
        fin.readRawData(tmp.data(), 1);
        fout.writeRawData(tmp.data(), 1);
        quint32 animalsInHerd;
        fin >> animalsInHerd;
        fout << animalsInHerd;

        tmp.resize(animalsInHerd * 4);
        fin.readRawData(tmp.data(), tmp.size());
        fout.writeRawData(tmp.data(), tmp.size());

        copyArray<quint8>(fin, fout);
        tmp.resize(4);

        uchar homeBuilding;
        fin >> homeBuilding;
        fout << homeBuilding;
        if (homeBuilding == 1) {
            fin.readRawData(tmp.data(), tmp.size());
            fout.writeRawData(tmp.data(), tmp.size());
        }
        uchar relocatingBuildsite;
        fin >> relocatingBuildsite;
        fout << relocatingBuildsite;
        if (relocatingBuildsite == 1) {
            fin.readRawData(tmp.data(), tmp.size());
            fout.writeRawData(tmp.data(), tmp.size());
        }
        tmp.resize(38);
        fin.readRawData(tmp.data(), tmp.size());
        fout.writeRawData(tmp.data(), tmp.size());
        uchar activeGrazingArea;
        fin >> activeGrazingArea;
        fout << activeGrazingArea;
        if (activeGrazingArea == 1) {
            tmp.resize(4);
            fin.readRawData(tmp.data(), tmp.size());
            fout.writeRawData(tmp.data(), tmp.size());
        }
        tmp.resize(16);
        fin.readRawData(tmp.data(), tmp.size());
        fout.writeRawData(tmp.data(), tmp.size());
    }

    quint32 areaCount;
    fin >> areaCount;
    for (quint32 i = 0; i < areaCount; ++i) {
        fin.skipRawData(4);
        skipArray<uint8_t>(fin);
    }
    areaCount = spawns.size();
    fout << areaCount;
    for (const auto& i : spawns) {
        fout << i.first;
        fout <<  uint8_t(i.second.uuid.size());
        fout.writeRawData(i.second.uuid.data(), i.second.uuid.size());
    }

    quint32 spawnIntervalsByAnimalGroup;
    fin >> spawnIntervalsByAnimalGroup;
    fout << spawnIntervalsByAnimalGroup;

    for (uint i = 0; i < herdCount; ++i) {
        copyArray<quint8>(fin, fout);
        quint32 interval;
        fin >> interval;
        fout << interval;
    }

    fin.readRawData(tmp.data(), 1);
    fout.writeRawData(tmp.data(), 1);
    buf = outBuf.data();
}

void GameMapChanger::handleMineralManager(QByteArray& buf, const std::vector<MineralData>& addMinerals)
{
    QBuffer outBuf;
    outBuf.open(QIODeviceBase::WriteOnly);
    QDataStream fout(&outBuf);
    QDataStream fin(&buf, QIODeviceBase::ReadOnly);
    fout.setByteOrder(QDataStream::LittleEndian);
    fout.setFloatingPointPrecision(QDataStream::SinglePrecision);
    fin.setByteOrder(QDataStream::LittleEndian);
    fin.setFloatingPointPrecision(QDataStream::SinglePrecision);

    QByteArray tmp(1, Qt::Uninitialized);
    fin.readRawData(tmp.data(), 1);
    fout.writeRawData(tmp.data(), 1);

    auto hanleMineral = [&](std::set<MineralType> types, uint copyDataSize) -> uint {
        uint count;
        fin >> count;
        uint adding = 0;
        for (const auto& m : addMinerals) {
            if (types.count(m.type) != 0) {
                ++adding;
            }
        }
        fout << uint(count + adding);
        for (uint i = 0; i < count; ++i) {
            tmp.resize(copyDataSize);
            fin.readRawData(tmp.data(), tmp.size());
            fout.writeRawData(tmp.data(), tmp.size());
            uint amount;
            fin >> amount;
            if (options_.doubleMinerals) {
                amount *= 2;
            }
            fout << amount;
            uchar deep;
            fin >> deep;
            fout << deep;
        }
        return count;
    };

    MineralType dataTypes[3] = {MineralType::Clay, MineralType::Sand, MineralType::Unknown};
    for (const auto mineralType : dataTypes) {
        hanleMineral({mineralType}, 16);
        for (const auto& m : addMinerals) {
            if (m.type == mineralType) {
                fout << m.p;
                fout << m.r;
                fout << m.amount;
                fout << m.deep;
            }
        }
    }
    uint mineralIndex = hanleMineral({MineralType::Gold, MineralType::Coal, MineralType::Iron}, 24);
    for (const auto& m : addMinerals) {
        if (m.type == MineralType::Gold || m.type == MineralType::Coal || m.type == MineralType::Iron) {
            fout << ++mineralIndex;
            fout << mineralTypeId(m.type);
            fout << m.p;
            fout << m.r;
            fout << m.amount;
            fout << m.deep;
        }
    }
    quint32 count;
    fin >> count;
    tmp.resize(count * 32 + 1);
    fin.readRawData(tmp.data(), tmp.size());
    fout << count;
    fout.writeRawData(tmp.data(), tmp.size());

    quint32 mineralCount;
    fin >> mineralCount;
    tmp.resize(mineralCount * 20);
    fin.readRawData(tmp.data(), tmp.size());
    for (const auto& m : addMinerals) {
        if (m.type == MineralType::Gold || m.type == MineralType::Coal || m.type == MineralType::Iron) {
            ++mineralCount;
        }
    }
    fout << mineralCount;
    fout.writeRawData(tmp.data(), tmp.size());
    for (const auto& m : addMinerals) {
        if (m.type == MineralType::Gold || m.type == MineralType::Coal || m.type == MineralType::Iron) {
            fout << mineralTypeId(m.type);
            fout << m.p;
        }
    }
    tmp.resize(1);
    fin.readRawData(tmp.data(), 1);
    fout.writeRawData(tmp.data(), 1);

    buf = outBuf.data();
}

void GameMapChanger::handleGuids(QByteArray &buf, const std::set<uint32_t>& removed)
{
    QBuffer outBuf;
    outBuf.open(QIODeviceBase::WriteOnly);
    QDataStream fout(&outBuf);
    QDataStream fin(&buf, QIODeviceBase::ReadOnly);
    uint readed = 7;
    QByteArray tmp(3, Qt::Uninitialized);
    fin.readRawData(tmp.data(), tmp.size());
    fout.writeRawData(tmp.data(), tmp.size());
    uint8_t count;
    fin >> count;
    uint8_t newCount = count - removed.size();
    fout << newCount;
    tmp.resize(3);
    fin.readRawData(tmp.data(), tmp.size());
    fout.writeRawData(tmp.data(), tmp.size());

    int skip = 0;
    for (int i = 0; i < count; ++i) {
        if (removed.count(i) > 0) {
            readed += skipArray<quint8>(fin) + 1;
            ++skip;
            if (skip == removed.size()) {
                break;
            }
        } else {
            readed += copyArray<quint8>(fin, fout) + 1;
        }
    }
    fout.writeRawData(buf.data() + readed, buf.size() - readed);
    buf = outBuf.data();
}

void GameMapChanger::handleMetaData(QByteArray& buf)
{
    QBuffer outBuf;
    outBuf.open(QIODeviceBase::WriteOnly);
    QDataStream fout(&outBuf);
    QDataStream fin(&buf, QIODeviceBase::ReadOnly);
    fout.setByteOrder(QDataStream::LittleEndian);
    fin.setByteOrder(QDataStream::LittleEndian);

    uint readed = 5;
    QByteArray tmp(5, Qt::Uninitialized);
    fin.readRawData(tmp.data(), tmp.size());
    fout.writeRawData(tmp.data(), tmp.size());
    readed += copyArray<quint8>(fin, fout) + 1; // version
    readed += copyArray<quint8>(fin, fout) + 1; // seed

    // difficulties
    tmp.resize(4);
    fin.readRawData(tmp.data(), 4);
    if (options_.difficulty > 0) {
        tmp[0] = options_.difficulty - 1; // resources
        tmp[1] = options_.difficulty - 1; // disease
        tmp[2] = options_.difficulty; // wildlife
        tmp[3] = options_.difficulty; // raiders
    }
    fout.writeRawData(tmp.data(), 4);
    quint8 pacifist;
    fin >> pacifist;
    switch (options_.pacifist)
    {
    case 0:
        pacifist = 0;
        break;
    case 1:
        //keep value
        break;
    case 2:
        pacifist = 1;
        break;
    }
    fout << pacifist;

    // custom
    tmp.resize(20);
    fin.readRawData(tmp.data(), 20);
    fout.writeRawData(tmp.data(), 20);

    quint8 difficulty;
    fin >> difficulty;
    if (options_.difficulty >= 0) {
        difficulty = options_.difficulty;
    }
    fout << difficulty;

    readed += 25;

    if (!options_.name.isEmpty()) {
        quint8 lenName;
        fin >> lenName;
        tmp.resize(lenName);
        fin.readRawData(tmp.data(), tmp.size());
        fout << quint8(options_.name.size());
        fout.writeRawData(options_.name.data(), options_.name.size());
        readed += lenName + 1;
    }

    fout.writeRawData(buf.data() + readed, buf.size() - readed);
    buf = outBuf.data();
}

void GameMapChanger::handleFlattenAreas(QByteArray& buf)
{
    QBuffer outBuf;
    outBuf.open(QIODeviceBase::WriteOnly);
    QDataStream fout(&outBuf);
    QDataStream fin(&buf, QIODeviceBase::ReadOnly);
    fout.setByteOrder(QDataStream::LittleEndian);
    fin.setByteOrder(QDataStream::LittleEndian);

    uint readed = 122;
    QByteArray tmp(122, Qt::Uninitialized);
    fin.readRawData(tmp.data(), tmp.size());
    fout.writeRawData(tmp.data(), tmp.size());

    readed += copyArray<quint8>(fin, fout) + 1;

    tmp.resize(1);
    fin.readRawData(tmp.data(), 1);
    fout.writeRawData(tmp.data(), 1);

    uint count;
    fin >> count;
    fout << count;
    readed += 5;
    for (uint i = 0; i < count; ++i) {
        readed += copyArray<quint8>(fin, fout) + 1;
        tmp.resize(417);
        fin.readRawData(tmp.data(), tmp.size());
        fout.writeRawData(tmp.data(), tmp.size());
        uint amount;
        fin >> amount;
        amount = 1;
        fout << amount;
        readed += 421;
    }
    tmp.resize(20);
    fin.readRawData(tmp.data(), tmp.size());
    fout.writeRawData(tmp.data(), tmp.size());
    uint amount;
    fin >> amount;
    amount = 1;
    fout << amount;
    readed += 24;

    fout.writeRawData(buf.data() + readed, buf.size() - readed);
    buf = outBuf.data();
}

void GameMapChanger::handleVillager(QByteArray &buf)
{
    QBuffer outBuf;
    outBuf.open(QIODeviceBase::WriteOnly);
    QDataStream fout(&outBuf);
    QDataStream fin(&buf, QIODeviceBase::ReadOnly);
    fout.setByteOrder(QDataStream::LittleEndian);
    fin.setByteOrder(QDataStream::LittleEndian);

    uint readed = 5 + 41;
    QByteArray tmp(readed, Qt::Uninitialized);
    fin.readRawData(tmp.data(), tmp.size());
    fout.writeRawData(tmp.data(), tmp.size());
    readed += copyArray<quint8>(fin, fout) + 1; // Player

    tmp.resize(4);
    fin.readRawData(tmp.data(), tmp.size());
    fout.writeRawData(tmp.data(), tmp.size());
    uint8_t isWounded;
    fin >> isWounded;
    fout << isWounded;
    readed += 5;

    if (isWounded) {
        tmp.resize(4);
        fin.readRawData(tmp.data(), tmp.size());
        fout.writeRawData(tmp.data(), tmp.size());
        readed += 4;
    }
    tmp.resize(8 + 1 + 8);
    fin.readRawData(tmp.data(), tmp.size());
    fout.writeRawData(tmp.data(), tmp.size());
    readed += 8 + 1 + 8;

    readed += copyArray<quint8>(fin, fout) + 1; // name
    // VillagerHealth
    tmp.resize(12 + 52);
    fin.readRawData(tmp.data(), tmp.size());
    fout.writeRawData(tmp.data(), tmp.size());
    readed += 12 + 52;

    uint8_t lastFireWarmthDate;
    fin >> lastFireWarmthDate;
    fout << lastFireWarmthDate;
    ++readed;
    if (lastFireWarmthDate) {
        tmp.resize(12);
        fin.readRawData(tmp.data(), tmp.size());
        fout.writeRawData(tmp.data(), tmp.size());
        readed += 12;
    }
    // ConsumedFoodTracker
    //   ItemStorage[]
    uint32_t foodEatenByMonth;
    fin >> foodEatenByMonth;
    fout << foodEatenByMonth;
    readed += 4;
    for (uint i = 0; i < foodEatenByMonth; ++i) {
        uint8_t tmpByte;
        fin >> tmpByte;
        fout << tmpByte;

        uint32_t itemBundles;
        fin >> itemBundles;
        fout << itemBundles;
        readed += 5;
        for (uint i = 0; i < itemBundles; ++i) {
            readed += copyArray<quint8>(fin, fout) + 1; // Item
            tmp.resize(ItemFillerSize + 4);
            fin.readRawData(tmp.data(), tmp.size());
            fout.writeRawData(tmp.data(), tmp.size());
            readed += ItemFillerSize + 4;
        }
        tmp.resize(5);
        fin.readRawData(tmp.data(), tmp.size());
        fout.writeRawData(tmp.data(), tmp.size());
        readed += 5;
    }

    uint32_t eathenCount;
    fin >> eathenCount;
    fout << eathenCount;
    readed += 4;

    tmp.resize(eathenCount * 4 + 9);
    fin.readRawData(tmp.data(), tmp.size());
    fout.writeRawData(tmp.data(), tmp.size());
    readed += eathenCount * 4 + 9;

    uint32_t dietHistory;
    fin >> dietHistory;
    fout << dietHistory;
    readed += 4;

    tmp.resize(dietHistory * 4 + 71);
    fin.readRawData(tmp.data(), tmp.size());
    fout.writeRawData(tmp.data(), tmp.size());
    readed += dietHistory * 4 + 71;

    uint32_t drunkHistory;
    fin >> drunkHistory;
    fout << drunkHistory;
    readed += 4;

    tmp.resize(drunkHistory * 4);
    fin.readRawData(tmp.data(), tmp.size());
    fout.writeRawData(tmp.data(), tmp.size());
    readed += drunkHistory * 4;

    readed += copyArray<quint8>(fin, fout) + 1; // portrait

    // ImmuneSystemComponent
    uint8_t tmpByte;
    fin >> tmpByte;
    fout << tmpByte;

    uint32_t immunities;
    fin >> immunities;
    fout << immunities;
    readed += 5;

    for (uint i = 0; i < immunities; ++i) {
        readed += copyArray<quint8>(fin, fout) + 1; // Disease
    }

    uint32_t globalImmunities;
    fin >> globalImmunities;
    fout << globalImmunities;
    readed += 4;
    for (uint i = 0; i < globalImmunities; ++i) {
        // Disease
        readed += copyArray<quint8>(fin, fout) + 1; // Disease
        tmp.resize(12);
        fin.readRawData(tmp.data(), tmp.size());
        fout.writeRawData(tmp.data(), tmp.size());
        readed += 12;
    }

    uint32_t presentDiseases;
    fin >> presentDiseases;
    if (options_.cureVillagers) {
        fout << uint32_t(0);
    } else {
        fout << presentDiseases;
    }
    readed += 4;
    if (presentDiseases > 0) {

        tmp.resize(5);
        fin.readRawData(tmp.data(), tmp.size());
        if (!options_.cureVillagers) {
            fout.writeRawData(tmp.data(), tmp.size());
        }
        readed += 5;
        for (uint i = 0; i < presentDiseases; ++i) {
            // DiseaseComponent
            if (!options_.cureVillagers) {
                readed += copyArray<quint8>(fin, fout) + 1; // diseaseDef
                readed += copyArray<quint8>(fin, fout) + 1; // DiseaseComponent Guid
            } else {
                readed += skipArray<quint8>(fin) + 1; // diseaseDef
                readed += skipArray<quint8>(fin) + 1; // DiseaseComponent Guid
            }

            tmp.resize(24);
            fin.readRawData(tmp.data(), tmp.size());
            if (!options_.cureVillagers) {
                fout.writeRawData(tmp.data(), tmp.size());
            }
            readed += 24;
        }
    }

    fout.writeRawData(buf.data() + readed, buf.size() - readed);
    buf = outBuf.data();
}
