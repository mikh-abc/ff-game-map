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

namespace {
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

QDataStream& operator>>(QDataStream& in, Point &rhs) {
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

void FarthestFrontierMap::loadMap(const QString& path)
{
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly)) {
        return;
    }
    QDataStream in(&f);
    in.setByteOrder(QDataStream::LittleEndian);
    in.setFloatingPointPrecision(QDataStream::SinglePrecision);
    in.skipRawData(0x80B0);
    landscapeData_ = readArray<quint32>(in);
    in.skipRawData(0x157b5ab);
    quint32 count;
    in >> count;
    in.skipRawData(count * 4);
    in.skipRawData(0x90146);

    quint32 objectsCount;
    in >> objectsCount;

    qDebug() << objectsCount;
    for (quint32 i = 0; i < objectsCount; ++i) {
        quint32 id;
        in >> id;
        in.skipRawData(8);
        float x;
        in >> x;
        float y;
        in >> y;
        if (y > 2)
            qDebug() << id << x << y;
    }
}

bool FarthestFrontierMap::loadSave(const QString& path)
{
    saveFile_.close();
    saveFile_.setFileName(path);
    if (!saveFile_.open(QIODevice::ReadOnly)) {
        return false;
    }
    table_.clear();
    QDataStream in(&saveFile_);
    in.setByteOrder(QDataStream::LittleEndian);

    while(!in.atEnd()) {
        quint8 componentType;
        in >> componentType;
        QByteArray field = readArray<quint8>(in);
        quint32 fieldSize;
        in >> fieldSize;
        quint32 id = 0;
        in >> id;
        table_[id].push_back(saveFile_.pos());
        in.skipRawData(fieldSize - 4);
    }
    return true;
}

bool FarthestFrontierMap::copySave(const QString &path, bool removeFoW)
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
        out << componentType;
        QByteArray field = readArray<quint8>(in);
        out << quint8(field.size());
        out.writeRawData(field.data(), field.size());
        quint32 fieldSize;
        in >> fieldSize;
        out << fieldSize;
        quint32 id = 0;
        in >> id;
        out << id;

        QByteArray buf(fieldSize - 4, Qt::Uninitialized);
        in.readRawData(buf.data(), fieldSize - 4);
        if (removeFoW && id == 272625919) { // fowSystem
            for (int i = 0; i < 512; ++i) {
                for (int j = 0; j < 512; ++j) {
                    buf[(1 + i + j * 512) * 4 + 1] = 0xff;
                    buf[(1 + i + j * 512) * 4 + 2] = 0xff;
                }
            }
        }
        out.writeRawData(buf.data(), buf.size());
    }
    return true;
}

std::vector<MineralData> FarthestFrontierMap::minerals()
{
    QDataStream in(&saveFile_);
    in.setByteOrder(QDataStream::LittleEndian);
    in.setFloatingPointPrecision(QDataStream::SinglePrecision);

    if (!seekFieldSaveFile(274602495)) { // mineralManager
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

std::vector<ForageableData> FarthestFrontierMap::forageables()
{
    QDataStream in(&saveFile_);
    in.setByteOrder(QDataStream::LittleEndian);
    in.setFloatingPointPrecision(QDataStream::SinglePrecision);

    int index = 0;
    std::vector<ForageableData> r;
    while (seekFieldSaveFile(912881919, index++)) { // forageableResource
        in.skipRawData(5);
        quint8 tmp1;
        in >> tmp1;
        in.skipRawData(1 + tmp1 * 4);
        ForageableData d;
        d.type = GameItem::Unknown;
        in >> d.p;
        in.skipRawData(28);
        readArray<quint8>(in); //"BerriesResource" "ForagingResource" "BushResource"
        in.skipRawData(1);
        quint32 tmp4;
        in >> tmp4;
        if (tmp4 == 0) {
            in.skipRawData(35);
        } else {
            readArray<quint8>(in);
            in.skipRawData(456);
        }
        auto item = readArray<quint8>(in);
        d.type = parseItem(item);
        in >> d.count;
        r.emplace_back(d);
    }
    return r;
}

std::vector<RaiderData> FarthestFrontierMap::raiders()
{
    QDataStream in(&saveFile_);
    in.setByteOrder(QDataStream::LittleEndian);
    in.setFloatingPointPrecision(QDataStream::SinglePrecision);

    int index = 0;
    std::vector<RaiderData> r;
    QHash<QByteArray, uint> uuids;
    while (seekFieldSaveFile(545559295, index++)) { // raider
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
    }
    index = 0;
    while (seekFieldSaveFile(2094352639, index++)) { // batteringRam
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
    }

    qDebug() << uuids;
    return r;
}
/*
 * ("7b8e2ab8-2511-4b09-b2a3-7d020d200654", 17) thief
 * ("f7eb7176-4b2d-450b-b991-aa4854de3b70", 10) brawler
 * ("e2f3921a-a92c-4eef-9b80-1d4990d53d96", 16) brawler female
 * ("d46bd838-4635-4248-917b-a8bb8df8bbc2", 12) warrior
 * ("8937422c-1758-4903-8760-8a559515dfa7",  4) warrior female
 * ("1c6eff9d-3d70-4758-8688-b0184883c41a",  2) shieldbearer
  */

std::vector<BaseData> FarthestFrontierMap::enemies()
{
    QDataStream in(&saveFile_);
    in.setByteOrder(QDataStream::LittleEndian);
    in.setFloatingPointPrecision(QDataStream::SinglePrecision);

    std::vector<BaseData> r;
    int index = 0;
    while (seekFieldSaveFile(545559295, index++)) { // raider
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

std::vector<BaseData> FarthestFrontierMap::animals()
{
    QDataStream in(&saveFile_);
    in.setByteOrder(QDataStream::LittleEndian);
    in.setFloatingPointPrecision(QDataStream::SinglePrecision);

    const std::vector<std::pair<uint, BaseType>> ids =
        {{ 482861567, BaseType::Deer},
         {3005595647, BaseType::Boar},
         {2921281535, BaseType::Wolf},
         {1358854911, BaseType::Bear}
        };
    std::vector<BaseData> r;
    for (const auto& i : ids) {
        int index = 0;
        while (seekFieldSaveFile(i.first, index++)) {
            in.skipRawData(6);
            BaseData d;
            d.type = i.second;
            in >> d.p;
            r.emplace_back(d);
        }
    }
    return r;
}

std::vector<BaseData> FarthestFrontierMap::houses()
{
    QDataStream in(&saveFile_);
    in.setByteOrder(QDataStream::LittleEndian);
    in.setFloatingPointPrecision(QDataStream::SinglePrecision);

    int index = 0;
    std::vector<BaseData> r;
    while (seekFieldSaveFile(3556611327, index++)) { // townCenter
        in.skipRawData(7);
        BaseData d;
        d.type = BaseType::TownCenter;
        in >> d.p;
        r.emplace_back(d);
    }
    index = 0;
    while (seekFieldSaveFile(2831428095, index++)) { // shelter
        in.skipRawData(7);
        BaseData d;
        d.type = BaseType::Shelter;
        in >> d.p;
        r.emplace_back(d);
    }
    return r;
}

GeneralSaveData FarthestFrontierMap::generalSaveData()
{
    QDataStream in(&saveFile_);
    in.setByteOrder(QDataStream::LittleEndian);
    in.setFloatingPointPrecision(QDataStream::SinglePrecision);

    GeneralSaveData r;
    if (!seekFieldSaveFile(3982492927)) { // metaData
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

QByteArray FarthestFrontierMap::saveName() const
{
    return QByteArray();
}

QByteArray FarthestFrontierMap::seed() const
{
    return QByteArray();
}

QByteArray FarthestFrontierMap::version() const
{
    return QByteArray();
}

bool FarthestFrontierMap::seekFieldSaveFile(quint32 id, uint index)
{
    auto i = table_.find(id);
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

QPixmap FarthestFrontierMap::landscape() const
{
    QPixmap pixmap;
    pixmap.loadFromData(landscapeData_, "PNG");
    return pixmap;
}

Point FarthestFrontierMap::size()
{
    QDataStream in(&saveFile_);
    in.setByteOrder(QDataStream::LittleEndian);
    in.setFloatingPointPrecision(QDataStream::SinglePrecision);

    //1767231999 terrainManager
    Point r;
    memset(&r, 0, sizeof(Point));
    if (!seekFieldSaveFile(2116582911)) { // agricultureManager
        return r;
    }
    in.skipRawData(6);
    in >> r.x;
    in >> r.y;
    return r;
}

Point FarthestFrontierMap::start()
{
    QDataStream in(&saveFile_);
    in.setByteOrder(QDataStream::LittleEndian);
    in.setFloatingPointPrecision(QDataStream::SinglePrecision);

    Point r;
    memset(&r, 0, sizeof(Point));
    if (!seekFieldSaveFile(788030719)) { // cameraManager
        return r;
    }
    in.skipRawData(1);
    in >> r.x;
    in >> r.y;
    in >> r.z;
    return r;
}
