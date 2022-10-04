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
#include "FarthestFrontierMap.h"

#include <QDebug>

namespace {

const std::unordered_map<uint, BaseType> BaseTypeById = {
    {  272625919, BaseType::FoWSystem              },
    {  274602495, BaseType::MineralManager         },
    {  482861567, BaseType::Deer                   },
    {  545559295, BaseType::Raider                 },
    {  788030719, BaseType::CameraManager          },
    {  912881919, BaseType::ForageableResource     },
    { 1358854911, BaseType::Bear                   },
    { 1676811007, BaseType::WolfDen                },
    { 2094352639, BaseType::BatteringRam           },
    { 2116582911, BaseType::AgricultureManager     },
    { 2831428095, BaseType::Shelter                },
    { 2921281535, BaseType::Wolf                   },
    { 3005595647, BaseType::Boar                   },
    { 3556611327, BaseType::TownCenter             },
    { 3982492927, BaseType::MetaData               },
    { 3997403647, BaseType::BuildingBuildSite      },
    { 4058971987, BaseType::BuildingBuildSiteGuids }
    };

BaseType parseBaseType(uint id) {
    auto i = BaseTypeById.find(id);
    if (i == BaseTypeById.end()) {
        return BaseType::Unknown;
    }
    return i->second;
}

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

MineralType parseMineralType(quint32 v)
{
    switch (v)
    {
    case 0:
        return MineralType::Iron;
    case 1:
        return MineralType::Gold;
    case 2:
        return MineralType::Coal;
    }
    return MineralType::Unknown;
}

GameItem parseItem(const QByteArray& v)
{
    if (v == "ItemMushrooms")
        return GameItem::Mushrooms;
    else if (v == "ItemRoots")
        return GameItem::Roots;
    else if (v == "ItemNuts")
        return GameItem::Nuts;
    else if (v == "ItemHerbs")
        return GameItem::Herbs;
    else if (v == "ItemEggs")
        return GameItem::Eggs;
    else if (v == "ItemWillow")
        return GameItem::Willow;
    else if (v == "ItemGreens")
        return GameItem::Greens;
    else if (v == "ItemBerries")
        return GameItem::Berries;
    else if (v == "ItemLogs")
        return GameItem::Logs;
    return GameItem::Unknown;
}

QDataStream& operator>>(QDataStream& in, Point& rhs) {
   in >> rhs.x;
   in >> rhs.y;
   in >> rhs.z;
   return in;
}

}

FarthestFrontierMap::FarthestFrontierMap(QObject* parent)
    : QObject{parent}
{

}

QPixmap FarthestFrontierMap::landscape() const
{
    QPixmap pixmap;
    pixmap.loadFromData(landscapeData_, "PNG");
    return pixmap;
}

bool FarthestFrontierMap::loadSave(const QString& path)
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

bool FarthestFrontierMap::copySave(const QString& path, bool removeFoW, bool removeBuildingSites)
{
    QFile newFile_(path);
    if (!newFile_.open(QIODevice::WriteOnly)) {
        return false;
    }
    saveFile_.seek(0);

    QDataStream in(&saveFile_);
    in.setByteOrder(QDataStream::LittleEndian);
    QDataStream out(&newFile_);
    out.setByteOrder(QDataStream::LittleEndian);

    while(!in.atEnd()) {
        quint8 componentType;
        in >> componentType;
        QByteArray field = readArray<quint8>(in);
        quint32 fieldSize;
        in >> fieldSize;
        quint32 id = 0;
        in >> id;
        QByteArray buf(fieldSize - 4, Qt::Uninitialized);
        in.readRawData(buf.data(), fieldSize - 4);

        bool removeField = false;
        switch (parseBaseType(id)) {
        case BaseType::FoWSystem: {
            if (removeFoW) {
                for (int i = 0; i < 512; ++i) {
                    for (int j = 0; j < 512; ++j) {
                        buf[(1 + i + j * 512) * 4 + 1] = 0xff;
                        buf[(1 + i + j * 512) * 4 + 2] = 0xff;
                    }
                }
            }
            break;
        }
        case BaseType::BuildingBuildSite:
            removeField = removeBuildingSites;
            break;
        }
        if (!removeField) {
            out << componentType;
            out << quint8(field.size());
            out.writeRawData(field.data(), field.size());
            out << fieldSize;
            out << id;
            out.writeRawData(buf.data(), buf.size());
        }
    }
    return true;
}

void FarthestFrontierMap::closeSave()
{
    saveFile_.close();
    savePath_.clear();
}

FarthestFrontierMap::SaveReader FarthestFrontierMap::reader()
{
    return SaveReader(*this);
}

FarthestFrontierMap::SaveReader::SaveReader(FarthestFrontierMap &map)
    : table_(map.table_)
    , saveFile_(map.savePath_)
{
    saveFile_.open(QIODevice::ReadOnly);
}

FarthestFrontierMap::SaveReader::~SaveReader()
{
    saveFile_.close();
}

std::vector<MineralData> FarthestFrontierMap::SaveReader::minerals()
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
        in >> d.count;
        r.emplace_back(d);
    }
    quint32 sandCount;
    in >> sandCount;
    for (int i = 0; i < sandCount; ++i) {
        MineralData d;
        d.type = MineralType::Sand;
        in >> d.p;
        in >> d.r;
        in >> d.count;
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
        in >> d.count;
        r.emplace_back(d);
    }
    return r;
}

std::vector<ForageableData> FarthestFrontierMap::SaveReader::forageables()
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
            in >> d.count;
            d.p = p;
            d.type = parseItem(item);
            r.emplace_back(d);
        }
    }
    return r;
}

std::vector<RaiderData> FarthestFrontierMap::SaveReader::raiders()
{
    QDataStream in(&saveFile_);
    in.setByteOrder(QDataStream::LittleEndian);
    in.setFloatingPointPrecision(QDataStream::SinglePrecision);

    int index = 0;
    std::vector<RaiderData> r;
    QHash<QByteArray, uint> uuids;
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
        QHash<QByteArray, uint> items;
        for (uint i = 0; i < lootedItemsSize; ++i) {
            auto txt = readArray<quint8>(in); // "Item..."
            in.skipRawData(417);
            uint count;
            in >> count;
            if (count) {
                items[txt.right(txt.size() - 4)] = count;
            }
            //qDebug() << txt << count;
        }
        in >> d.p1; // = 250 //carry
        in.skipRawData(1); // 00
        uint ownItemsSize;
        in >> ownItemsSize;
        for (uint i = 0; i < ownItemsSize; ++i) {
            auto txt = readArray<quint8>(in); // "Item..."
            in.skipRawData(417);
            uint count;
            in >> count;
            if (count) {
                items[txt.right(txt.size() - 4)] = count;
            }
            //qDebug() << txt << count;
        }
        in.skipRawData(4); // 0
        in >> d.p2; // = 250 //carry?
        auto u = readArray<quint8>(in);
        uuids[u]++;
        if (items.size() > 0)
        {
            qDebug() << u << items;
        }
        r.emplace_back(d);
    }
    index = 0;
    while (seekFieldSaveFile(BaseType::BatteringRam, index++)) {
        in.skipRawData(6);
        RaiderData d;
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

    qDebug() << uuids;
    return r;
}

std::vector<BaseData> FarthestFrontierMap::SaveReader::enemies()
{
    QDataStream in(&saveFile_);
    in.setByteOrder(QDataStream::LittleEndian);
    in.setFloatingPointPrecision(QDataStream::SinglePrecision);

    std::vector<BaseData> r;
    int index = 0;
    while (seekFieldSaveFile(BaseType::Raider, index++)) {
        in.skipRawData(6);
        BaseData d;
        d.type = BaseType::Raider;
        in >> d.p;
        in.skipRawData(28);
        readArray<quint8>(in); //"Raider"
        float hp;
        in >> hp;
        r.emplace_back(d);
    }
    return r;
}

std::vector<BaseData> FarthestFrontierMap::SaveReader::animals()
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

std::vector<AnimalData> FarthestFrontierMap::SaveReader::deers()
{
    QDataStream in(&saveFile_);
    in.setByteOrder(QDataStream::LittleEndian);
    in.setFloatingPointPrecision(QDataStream::SinglePrecision);

    int index = 0;
    std::vector<AnimalData> r;
    //QHash<QByteArray, uint> uuids;
    while (seekFieldSaveFile(BaseType::Deer, index++)) {
        in.skipRawData(6);
        AnimalData d;
        in >> d.p;
        in.skipRawData(28);
        readArray<quint8>(in); //"LandAnimalResource"
        float hp;
        in >> hp;
        in.skipRawData(1); // 00
        auto u = readArray<quint8>(in);
        in.skipRawData(2); // 00 01
        uint sizeSpawnPoints;
        in >> sizeSpawnPoints;
        for (uint i = 0; i < sizeSpawnPoints; ++i) {
            Point p;
            in >> p;
            d.spawnPoints.emplace_back(std::move(p));
        }
        in.skipRawData(1); // 01
        uint sizeWanderPoints;
        in >> sizeWanderPoints;
        for (uint i = 0; i < sizeWanderPoints; ++i) {
            Point p;
            in >> p;
            d.wanderPoints.emplace_back(std::move(p));
        }
        in.skipRawData(1); // 00
        in >> d.spawnArea;
        r.emplace_back(std::move(d));
    }
    return r;
}

std::vector<BaseData> FarthestFrontierMap::SaveReader::houses()
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

GeneralSaveData FarthestFrontierMap::SaveReader::generalSaveData()
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

AgricultureInfo::Data FarthestFrontierMap::SaveReader::agricultureData()
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

bool FarthestFrontierMap::SaveReader::seekFieldSaveFile(BaseType baseType, uint index)
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

Point FarthestFrontierMap::SaveReader::camera()
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
