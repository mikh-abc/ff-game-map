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
#include "GameMap.h"
#include "ParseData.h"

QPixmap GameMap::landscape() const
{
    QPixmap pixmap;
    pixmap.loadFromData(landscapeData_, "PNG");
    return pixmap;
}

bool GameMap::loadSave(const QString& path)
{
    saveFile_.close();
    saveFile_.setFileName(path);
    if (!saveFile_.open(QIODevice::ReadOnly)) {
        return false;
    }
    savePath_ = path;
    table_.clear();
    QDataStream in(&saveFile_);
    in.setByteOrder(QDataStream::LittleEndian);

    while(!in.atEnd()) {
        quint8 componentType;
        in >> componentType;
        readArray<quint8>(in);
        quint32 fieldSize;
        in >> fieldSize;
        quint32 id = 0;
        in >> id;
        table_[parseBaseType(id)].push_back(saveFile_.pos());
        in.skipRawData(fieldSize - 4);
    }
    return true;
}

void GameMap::closeSave()
{
    saveFile_.close();
    savePath_.clear();
}

QString GameMap::saveFileName() const
{
    return savePath_;
}

GameMap::SaveReader GameMap::reader()
{
    return SaveReader(*this);
}

std::vector<MineralData> GameMap::SaveReader::minerals()
{
    QDataStream in(&saveFile_);
    in.setByteOrder(QDataStream::LittleEndian);
    in.setFloatingPointPrecision(QDataStream::SinglePrecision);

    if (!seekFieldSaveFile(BaseType::MineralManager)) {
        return std::vector<MineralData>();
    }

    std::vector<MineralData> r;

    in.skipRawData(1);
    quint32 clayCount;
    in >> clayCount;
    for (int i = 0; i < clayCount; ++i) {
        MineralData d;
        d.type = MineralType::Clay;
        in >> d.p;
        in >> d.r;
        in >> d.amount;
        r.emplace_back(d);
    }
    quint32 sandCount;
    in >> sandCount;
    for (int i = 0; i < sandCount; ++i) {
        MineralData d;
        d.type = MineralType::Sand;
        in >> d.p;
        in >> d.r;
        in >> d.amount;
        r.emplace_back(d);
    }
    quint32 mineralCount;
    in >> mineralCount;
    for (int i = 0; i < mineralCount; ++i) {
        quint32 id;
        in >> id;
        quint32 mineral;
        in >> mineral;
        MineralData d;
        d.type = parseMineralType(mineral);
        in >> d.p;
        in >> d.r;
        in >> d.amount;
        r.emplace_back(d);
    }
    return r;
}

std::vector<ForageableData> GameMap::SaveReader::forageables()
{
    QDataStream in(&saveFile_);
    in.setByteOrder(QDataStream::LittleEndian);
    in.setFloatingPointPrecision(QDataStream::SinglePrecision);

    int index = 0;
    std::vector<ForageableData> r;
    while (seekFieldSaveFile(BaseType::ForageableResource, index++)) {
        in.skipRawData(5);
        quint8 tmp1;
        in >> tmp1;
        in.skipRawData(1 + tmp1 * 4);
        Point p;
        in >> p;
        in.skipRawData(28);
        readArray<quint8>(in); //"BerriesResource" "ForagingResource" "BushResource"
        in.skipRawData(1);
        uint availableItemsSize;
        in >> availableItemsSize;
        for (uint i = 0; i < availableItemsSize; ++i) {
            readArray<quint8>(in);
            in.skipRawData(417);
            uint count;
            in >> count;
        }
        in.skipRawData(31);
        uint yieldsItemsSize;
        in >> yieldsItemsSize;
        for (uint i = 0; i < yieldsItemsSize; ++i) {
            ForageableData d;
            auto item = readArray<quint8>(in);
            in >> d.amount;
            d.p = p;
            d.type = parseItem(item);
            r.emplace_back(d);
        }
    }
    return r;
}

std::vector<RaiderData> GameMap::SaveReader::raiders()
{
    QDataStream in(&saveFile_);
    in.setByteOrder(QDataStream::LittleEndian);
    in.setFloatingPointPrecision(QDataStream::SinglePrecision);

    int index = 0;
    std::vector<RaiderData> r;
    while (seekFieldSaveFile(BaseType::Raider, index++)) {
        in.skipRawData(6);
        RaiderData d;
        in >> d.p;
        in.skipRawData(28);
        readArray<quint8>(in); //"Raider"
        float hp;
        in >> hp;
        in.skipRawData(1); // 00
        in >> d.spawn;
        in.skipRawData(1); // 00
        uint lootedItemsSize;
        in >> lootedItemsSize;
        for (uint i = 0; i < lootedItemsSize; ++i) {
            readArray<quint8>(in); // "Item..."
            in.skipRawData(417);
            uint count;
            in >> count;
        }
        in >> d.p1; // = 250 //carry
        in.skipRawData(1); // 00
        uint ownItemsSize;
        in >> ownItemsSize;
        for (uint i = 0; i < ownItemsSize; ++i) {
            readArray<quint8>(in); // "Item..."
            in.skipRawData(417);
            uint count;
            in >> count;
        }
        in.skipRawData(4); // 0
        in >> d.p2; // = 250 //carry?
        auto u = readArray<quint8>(in);
        d.type = parseRaiderType(u);
        r.emplace_back(d);
    }
    index = 0;
    while (seekFieldSaveFile(BaseType::BatteringRam, index++)) {
        in.skipRawData(6);
        RaiderData d;
        d.type = RaiderType::BatteringRam;
        in >> d.p;
        in.skipRawData(28);
        readArray<quint8>(in); //"BatteringRam"
        float hp;
        in >> hp;
        in.skipRawData(1); // 00
        in >> d.spawn;
        in.skipRawData(1); // 00
        r.emplace_back(d);
    }
    return r;
}

std::vector<BaseData> GameMap::SaveReader::animals()
{
    QDataStream in(&saveFile_);
    in.setByteOrder(QDataStream::LittleEndian);
    in.setFloatingPointPrecision(QDataStream::SinglePrecision);

    const BaseType list[] = { BaseType::Deer, BaseType::Bear, BaseType::Boar, BaseType::Wolf, BaseType::WolfDen };
    std::vector<BaseData> r;
    for (BaseType i : list) {
        int index = 0;
        while (seekFieldSaveFile(i, index++)) {
            in.skipRawData(6);
            BaseData d;
            d.type = i;
            in >> d.p;
            r.emplace_back(d);
        }
    }
    return r;
}

std::vector<BaseData> GameMap::SaveReader::houses()
{
    QDataStream in(&saveFile_);
    in.setByteOrder(QDataStream::LittleEndian);
    in.setFloatingPointPrecision(QDataStream::SinglePrecision);

    int index = 0;
    std::vector<BaseData> r;
    while (seekFieldSaveFile(BaseType::TownCenter, index++)) {
        in.skipRawData(7);
        BaseData d;
        d.type = BaseType::TownCenter;
        in >> d.p;
        r.emplace_back(d);
    }
    index = 0;
    while (seekFieldSaveFile(BaseType::Shelter, index++)) {
        in.skipRawData(7);
        BaseData d;
        d.type = BaseType::Shelter;
        in >> d.p;
        r.emplace_back(d);
    }
    return r;
}

std::vector<AnimalSpawnData> GameMap::SaveReader::animalsSpawns()
{
    QDataStream in(&saveFile_);
    in.setByteOrder(QDataStream::LittleEndian);
    in.setFloatingPointPrecision(QDataStream::SinglePrecision);

    std::vector<AnimalSpawnData> r;
    if (!seekFieldSaveFile(BaseType::AnimalManager)) {
        return r;
    }

    in.skipRawData(2);
    uint herdCount;
    in >> herdCount;
    for (int i = 0; i < herdCount; ++i) {
        in.skipRawData(1);
        uint workerCount;
        in >> workerCount;
        in.skipRawData(workerCount * 4);
        readArray<quint8>(in);

        in.skipRawData(15);
        uchar center;
        in >> center;
        if (center == 1) {
            in.skipRawData(4);
        }
        in.skipRawData(53);
    }
    quint32 areaCount;
    in >> areaCount;
    QHash<QByteArray, uint> uuids;
    for (int i = 0; i < areaCount; ++i) {
        AnimalSpawnData d;
        in >> d.spawnArea;
        auto uuid = readArray<quint8>(in);
        d.type = parseAnimalsSpawnType(uuid);
        uuids[uuid]++;
        r.emplace_back(d);
    }
    qDebug() << uuids;
    return r;
}

GeneralSaveData GameMap::SaveReader::generalSaveData()
{
    QDataStream in(&saveFile_);
    in.setByteOrder(QDataStream::LittleEndian);
    in.setFloatingPointPrecision(QDataStream::SinglePrecision);

    GeneralSaveData r;
    if (!seekFieldSaveFile(BaseType::MetaData)) {
        return r;
    }
    in.skipRawData(5);
    r.version = readArray<quint8>(in);
    r.seed = readArray<quint8>(in);
    in >> r.v1;
    in >> r.v2;
    in >> r.wildlifeDifficulty;
    in >> r.raidersDifficulty;
    in >> r.pacifist;
    r.name = readArray<quint8>(in);
    in >> r.villagers;
    in >> r.years;
    in >> r.v3;
    in >> r.v4;
    in >> r.timestamp;
    in >> r.hours;
    in >> r.mins;
    readArray<quint32>(in); //png
    return r;
}

AgricultureInfo::Data GameMap::SaveReader::agricultureData()
{
    QDataStream in(&saveFile_);
    in.setByteOrder(QDataStream::LittleEndian);
    in.setFloatingPointPrecision(QDataStream::SinglePrecision);

    AgricultureInfo::Data r;
    if (!seekFieldSaveFile(BaseType::AgricultureManager)) {
        return r;
    }
    in.skipRawData(6);
    in >> r.worldWidth;
    in >> r.worldHeight;
    uint width;
    uint height;
    in >> width;
    in >> height;
    for (uint t = 0; t < AgricultureInfo::Max; ++t) {
        r.data[static_cast<AgricultureInfo::DataType>(t)].resize(width);
    }
    for (uint i = 0; i < width; ++i) {
        for (uint t = 0; t < AgricultureInfo::Max; ++t) {
            r.data[static_cast<AgricultureInfo::DataType>(t)][i].resize(height);
        }
        for (uint j = 0; j < height; ++j) {
            for (uint t = 0; t < AgricultureInfo::Max; ++t) {
                float v;
                in >> v;
                r.data[static_cast<AgricultureInfo::DataType>(t)][i][j] = v;
            }
        }
    }
    return r;
}

std::vector<std::vector<float>> GameMap::SaveReader::heightMap()
{
    QDataStream in(&saveFile_);
    in.setByteOrder(QDataStream::LittleEndian);
    in.setFloatingPointPrecision(QDataStream::SinglePrecision);

    std::vector<std::vector<float>> r;
    if (!seekFieldSaveFile(BaseType::TerrainManager)) {
        return r;
    }
    auto start = saveFile_.pos();
    in.skipRawData(1);
    uint regrownTreeCount;
    in >> regrownTreeCount;
    in.skipRawData(regrownTreeCount * 96);
    uint treeGrouthCount;
    in >> treeGrouthCount;
    in.skipRawData(treeGrouthCount * 100);
    uint choppedTreeCount;
    in >> choppedTreeCount;
    in.skipRawData(choppedTreeCount * 12);
    uint objectsCount;
    in >> objectsCount;
    in.skipRawData(objectsCount * 122);
    qDebug() << saveFile_.pos() - start;
    uint count1;
    in >> count1;
    for (int i = 0; i < count1; ++i) {
        in.skipRawData(24);
        uint count11;
        in >> count11;
        in.skipRawData(count11 * 16);
        in.skipRawData(2);
    }
    qDebug() << saveFile_.pos() - start;
    uint count2;
    in >> count2;
    in.skipRawData(count2 * 81);

    uint mapSize;
    in >> mapSize;
    uint total;
    in >> total;
    r.resize(mapSize);
    for (uint i = 0; i < mapSize; ++i) {
        r[i].resize(mapSize);
        for (uint j = 0; j < mapSize; ++j) {
            in >> r[i][j];
        }
    }
    return r;
}

bool GameMap::SaveReader::seekFieldSaveFile(BaseType baseType, uint index)
{
    auto i = table_.find(baseType);
    if (i == table_.end()) {
        return false;
    }
    const auto& v = i.value();
    if (v.size() <= index) {
        return false;
    }
    saveFile_.seek(v[index]);
    return true;
}

GameMap::SaveReader::SaveReader(GameMap &map)
    : table_(map.table_)
    , saveFile_(map.savePath_)
{
    saveFile_.open(QIODevice::ReadOnly);
}

GameMap::SaveReader::~SaveReader()
{
    saveFile_.close();
}

Point GameMap::SaveReader::camera()
{
    QDataStream in(&saveFile_);
    in.setByteOrder(QDataStream::LittleEndian);
    in.setFloatingPointPrecision(QDataStream::SinglePrecision);

    Point r;
    memset(&r, 0, sizeof(Point));
    if (!seekFieldSaveFile(BaseType::CameraManager)) {
        return r;
    }
    in.skipRawData(1);
    in >> r.x;
    in >> r.y;
    in >> r.z;
    return r;
}
