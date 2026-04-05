// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software distributed under the License
// is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
// or implied.  See the License for the specific language governing permissions and limitations
// under the License.

#ifndef DATAREADER_H
#define DATAREADER_H

#include "DataDefines.h"

class GameData;

class DataReader
{
public:
    virtual Point camera() = 0;
    virtual std::vector<MineralData> minerals() = 0;
    virtual std::vector<ForageableData> forageables() = 0;
    virtual std::vector<RaiderData> raiders() = 0;
    virtual std::vector<BaseData> animals() = 0;
    virtual std::unordered_map<uint32_t, BaseData> sites() = 0;
    virtual std::unordered_map<uint32_t, AnimalSpawnData> animalsSpawns() = 0;
    virtual std::vector<AnimalData> deers() = 0;
    virtual GeneralSaveData generalSaveData() = 0;
    virtual AgricultureInfo::Data agricultureData() = 0;
    virtual TerrainData terrainData() = 0;
    virtual std::vector<VillagerData> villagers() = 0;

protected:
    BaseData readAnimal(QDataStream &in);
    BaseData readResource(QDataStream& in);
    std::vector<MineralData> readMinerals(QDataStream& in);
    Point readCamera(QDataStream& in);
    ForageableData readForageable(QDataStream& in);
    RaiderData readRaiderCamp(QDataStream &in);
    RaiderData readRaider(QDataStream& in);
    RaiderData readBatteringRam(QDataStream& in);
    BaseData readBuilding(QDataStream& in);
    std::unordered_map<uint32_t, AnimalSpawnData> readAnimalsSpawns(QDataStream& in);
    AnimalData readDeer(QDataStream& in);
    GeneralSaveData readMetaData(QDataStream& in);
    GeneralSaveData readModMetaData(QDataStream& in);
    AgricultureInfo::Data readAgricultureData(QDataStream& in);
    TerrainData readTerrainData(QDataStream& in);
    VillagerData readVillager(QDataStream& in);
};

#endif // DATAREADER_H
