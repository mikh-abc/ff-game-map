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
#include "FileDataReader.h"
#include "StaticData.h"

bool FileDataReader::loadSave(const QString& path)
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
        QByteArray field = readArray<quint8>(in);
        quint32 fieldSize;
        in >> fieldSize;
        quint32 id = 0;
        in >> id;
        table_[parseBaseType(id)].push_back(saveFile_.pos() + 1);
        in.skipRawData(fieldSize - 4);
    }
    return true;
}

std::vector<MineralData> FileDataReader::minerals()
{
    QDataStream in(&saveFile_);
    if (!seekDataType(in, BaseType::MineralManager))
        return {};
    return readMinerals(in);
}

std::vector<ForageableData> FileDataReader::forageables()
{
    QDataStream in(&saveFile_);
    int index = 0;
    std::vector<ForageableData> r;
    while (seekDataType(in, BaseType::ForageableResource, index++)) {
        r.push_back(readForageable(in));
    }
    return r;
}

std::vector<RaiderData> FileDataReader::raiders()
{
    QDataStream in(&saveFile_);
    std::vector<RaiderData> r;
    int index = 0;
    while (seekDataType(in, BaseType::RaiderCamp, index++)) {
        r.push_back(readRaiderCamp(in));
    }
    index = 0;
    while (seekDataType(in, BaseType::Raider, index++)) {
        r.push_back(readRaider(in));
    }
    index = 0;
    while (seekDataType(in, BaseType::BatteringRam, index++)) {
        r.push_back(readBatteringRam(in));
    }

    return r;
}

std::vector<BaseData> FileDataReader::animals()
{
    QDataStream in(&saveFile_);
    const BaseType list[] = { BaseType::Deer, BaseType::Bear, BaseType::Boar, BaseType::Wolf, BaseType::WolfDen };
    std::vector<BaseData> r;
    for (BaseType i : list) {
        int index = 0;
        while (seekDataType(in, i, index++)) {
            BaseData rd = readAnimal(in);
            rd.type = i;
            r.push_back(rd);
        }
    }
    return r;
}

std::unordered_map<uint32_t, BaseData> FileDataReader::sites()
{
    QDataStream in(&saveFile_);
    const BaseType list[] = { BaseType::TownCenter, BaseType::Shelter, BaseType::RelicExtraction, BaseType::SalvagingSite };
    std::unordered_map<uint32_t, BaseData> r;
    for (BaseType i : list) {
        int index = 0;
        while (seekDataType(in, i, index++)) {
            BaseData rd = readResource(in);
            rd.type = i;
            r.insert(std::pair<uint32_t, BaseData>(rd.id, rd));
        }
    }
    return r;
}

std::unordered_map<uint32_t, AnimalSpawnData> FileDataReader::animalsSpawns()
{
    QDataStream in(&saveFile_);
    if (!seekDataType(in, BaseType::AnimalManager))
        return {};
    return readAnimalsSpawns(in);
}

std::vector<AnimalData> FileDataReader::deers()
{
    QDataStream in(&saveFile_);
    int index = 0;
    std::vector<AnimalData> r;
    while (seekDataType(in, BaseType::Deer, index++)) {
        r.push_back(readDeer(in));
    }
    return r;
}

GeneralSaveData FileDataReader::generalSaveData()
{
    QDataStream in(&saveFile_);
    if (!seekDataType(in, BaseType::MetaData))
        return {};
    return readMetaData(in);
}

AgricultureInfo::Data FileDataReader::agricultureData()
{
    QDataStream in(&saveFile_);
    if (!seekDataType(in, BaseType::AgricultureManager))
        return {};
    return readAgricultureData(in);
}

TerrainData FileDataReader::terrainData()
{
    QDataStream in(&saveFile_);
    if (!seekDataType(in, BaseType::TerrainManager))
        return {};
    return readTerrainData(in);
}

std::vector<VillagerData> FileDataReader::villagers()
{
    QDataStream in(&saveFile_);
    int index = 0;
    std::vector<VillagerData> r;
    while (seekDataType(in, BaseType::Villager, index++)) {
        r.push_back(readVillager(in));
    }
    return r;
}

Point FileDataReader::camera()
{
    QDataStream in(&saveFile_);
    Point r;
    memset(&r, 0, sizeof(Point));
    if (!seekDataType(in, BaseType::CameraManager))
        return r;
    return readCamera(in);

}

void FileDataReader::closeSave()
{
    saveFile_.close();
    savePath_.clear();
}

QString FileDataReader::saveFileName() const
{
    return savePath_;
}

bool FileDataReader::seekDataType(QDataStream& in, BaseType baseType, uint index)
{
    in.setByteOrder(QDataStream::LittleEndian);
    in.setFloatingPointPrecision(QDataStream::SinglePrecision);
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
