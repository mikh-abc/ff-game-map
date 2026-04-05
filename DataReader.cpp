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
#include "DataReader.h"
#include "StaticData.h"

namespace {

void readItemStorage(QDataStream& in, std::vector<GameItemCount>& r)
{
    in.skipRawData(1); // 00
    // List<ItemBundle>
    uint itemBundles;
    in >> itemBundles;
    for (uint i = 0; i < itemBundles; ++i) {
        GameItemCount itemCount;
        auto itemStr = readArray<quint8>(in);
        in.skipRawData(ItemFillerSize);
        itemCount.item = parseItem(itemStr);
        in >> itemCount.count;
        r.push_back(itemCount);
    }
    in.skipRawData(5);
}
}

BaseData DataReader::readAnimal(QDataStream &in)
{
    BaseData r;
    in >> r.id;
    in.skipRawData(1);
    in >> r.p;
    return r;
}

BaseData DataReader::readResource(QDataStream &in)
{
    BaseData r;
    in >> r.id;

    quint8 flag;
    in >> flag;
    if (flag) {
        in.skipRawData(41); // parent Transform
    }
    in.skipRawData(1);
    in >> r.p;
    return r;
}

std::vector<MineralData> DataReader::readMinerals(QDataStream& in)
{
    std::vector<MineralData> r;
    MineralType dataTypes[3] = {MineralType::Clay, MineralType::Sand, MineralType::Stone};
    for (MineralType mineralType : dataTypes) {
        quint32 count;
        in >> count;
        for (uint i = 0; i < count; ++i) {
            MineralData d;
            d.type = mineralType;
            in >> d.p;
            in >> d.r;
            in >> d.amount;
            in >> d.deep;
            r.emplace_back(d);
        }
    }
    quint32 mineralCount;
    in >> mineralCount;
    for (uint i = 0; i < mineralCount; ++i) {
        quint32 id;
        in >> id;
        quint32 mineral;
        in >> mineral;
        MineralData d;
        d.type = parseMineralType(mineral);
        in >> d.p;
        in >> d.r;
        in >> d.amount;
        in >> d.deep;
        r.emplace_back(d);
    }
    quint32 fertilityBonusSites;
    in >> fertilityBonusSites;
    for (uint i = 0; i < fertilityBonusSites; ++i) {
        MineralData d;
        d.type = MineralType::FertilityBonus;
        in >> d.p;
        Point p2;
        in >> p2;
        in >> d.r;
        float bonus;
        in >> bonus;
        d.amount = bonus;
        d.deep = false;
        r.emplace_back(d);
    }
    return r;
}

ForageableData DataReader::readForageable(QDataStream &in)
{
    in.skipRawData(4);
    quint8 flag;
    in >> flag;
    if (flag) {
        in.skipRawData(4);
    }
    in.skipRawData(1);
    // Resource
    ForageableData d;
    in >> d.p;
    in.skipRawData(28);
    readArray<quint8>(in); //"BerriesResource" "ForagingResource" "BushResource"
    readItemStorage(in, d.available);
    in.skipRawData(33);
    // itemToReplenishRate
    uint replenishItemsSize;
    in >> replenishItemsSize;
    for (uint i = 0; i < replenishItemsSize; ++i) {
        GameItemCount itemCount;
        auto itemStr = readArray<quint8>(in);
        itemCount.item = parseItem(itemStr);
        in >> itemCount.count;
        d.replenish.push_back(itemCount);
    }

    //TODO remove legacy
    if (!d.replenish.empty()) {
        const GameItemCount& f = d.replenish[0];
        d.type = f.item;
        d.amount = f.count;
    }
    return d;
}

RaiderData DataReader::readRaiderCamp(QDataStream &in)
{
    in.skipRawData(4);
    quint8 flag;
    in >> flag;
    if (flag) {
        in.skipRawData(4);
    }
    in.skipRawData(1);
    // Resource
    RaiderData d;
    d.type = RaiderType::RaiderCamp;
    in >> d.p;
    return d;
}

RaiderData DataReader::readRaider(QDataStream& in)
{
    in.skipRawData(5);
    RaiderData d;
    in >> d.p;
    in.skipRawData(28);
    readArray<quint8>(in); //"Raider"
    // DamageableComponent
    float lifePercentage;
    in >> lifePercentage;
    quint8 isWounded;
    in >> isWounded;
    if (isWounded) {
        in.skipRawData(4); // woundedTime
    }
    in.skipRawData(8); // lifeModifier damageDealtModifier
    in >> d.spawn;
    in.skipRawData(1);
    uint lootedItemsSize;
    in >> lootedItemsSize;
    QHash<QByteArray, uint> items;
    for (uint i = 0; i < lootedItemsSize; ++i) {
        auto txt = readArray<quint8>(in); // "Item..."
        in.skipRawData(ItemFillerSize);
        uint count;
        in >> count;
        if (count) {
            items[txt.right(txt.size() - 4)] = count;
        }
    }
    in >> d.p1;
    in.skipRawData(2);
    uint ownItemsSize;
    in >> ownItemsSize;
    for (uint i = 0; i < ownItemsSize; ++i) {
        auto txt = readArray<quint8>(in); // "Item..."
        in.skipRawData(ItemFillerSize);
        uint count;
        in >> count;
        if (count) {
            items[txt.right(txt.size() - 4)] = count;
        }
    }
    in.skipRawData(5);
    in >> d.p2;
    auto u = readArray<quint8>(in);
    d.type = parseRaiderType(u);
    return d;
}

RaiderData DataReader::readBatteringRam(QDataStream& in)
{
    in.skipRawData(5);
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
    return d;
}

AnimalData DataReader::readDeer(QDataStream& in)
{
    in.skipRawData(5);
    AnimalData d;
    in >> d.p;
    in.skipRawData(28);
    readArray<quint8>(in); //"LandAnimalResource"
    float lifePercentage;
    in >> lifePercentage;
    quint8 isWounded;
    in >> isWounded;
    if (isWounded) {
        in.skipRawData(4); // woundedTime
    }
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
    return d;
}

std::unordered_map<uint32_t, AnimalSpawnData> DataReader::readAnimalsSpawns(QDataStream& in)
{
    std::unordered_map<uint32_t, AnimalSpawnData> r;
    in.skipRawData(1);
    uint herdCount;
    in >> herdCount;
    for (uint i = 0; i < herdCount; ++i) {
        in.skipRawData(1);
        uint animalsInHerd;
        in >> animalsInHerd;
        in.skipRawData(animalsInHerd * 4);
        readArray<quint8>(in); //guid animal
        uchar homeBuilding;
        in >> homeBuilding;
        if (homeBuilding == 1) {
            in.skipRawData(4);
        }
        uchar relocatingBuildsite;
        in >> relocatingBuildsite;
        if (relocatingBuildsite == 1) {
            in.skipRawData(4);
        }
        in.skipRawData(38);
        uchar activeGrazingArea;
        in >> activeGrazingArea;
        if (activeGrazingArea == 1) {
            in.skipRawData(4);
        }
        in.skipRawData(16);
    }
    quint32 areaCount;
    in >> areaCount;
    for (uint i = 0; i < areaCount; ++i) {
        AnimalSpawnData d;
        uint32_t spawnArea;
        in >> spawnArea;
        d.uuid = readArray<quint8>(in);
        d.type = parseAnimalsSpawnType(d.uuid);
        r.insert({spawnArea, d});
    }
    return r;
}

GeneralSaveData DataReader::readMetaData(QDataStream& in)
{
    GeneralSaveData r;
    in.skipRawData(4); // latestSaveVersion
    r.version = readArray<quint8>(in);
    r.seed = readArray<quint8>(in);
    in >> r.resourcesDifficulty;
    in >> r.diseaseDifficulty;
    in >> r.wildlifeDifficulty;
    in >> r.raidersDifficulty;
    in >> r.pacifist;
    in.skipRawData(20); // guidedStart сustom
    in >> r.difficultyLevel;
    r.name = readArray<quint8>(in);
    in >> r.villagers;
    int year;
    in >> year;
    in >> r.startYear;
    r.currentYear = (r.startYear + year - 1);
    in >> r.timestamp;
    in >> r.hours;
    in >> r.mins;
    return r;
}

GeneralSaveData DataReader::readModMetaData(QDataStream& in)
{
    GeneralSaveData r;
    in.skipRawData(4); // latestSaveVersion
    r.version = readArray<quint8>(in);
    r.seed = readArray<quint8>(in);
    in >> r.resourcesDifficulty;
    in >> r.diseaseDifficulty;
    in >> r.wildlifeDifficulty;
    in >> r.raidersDifficulty;
    in >> r.difficultyLevel;
    in >> r.pacifist;
    r.name = readArray<quint8>(in);
    in >> r.villagers;
    uint32_t currentDate;
    in >> currentDate;
    uint32_t startDate;
    in >> startDate;

    //int day = packed_date & 0x1F;
    r.currentYear = (currentDate >> 9);
    r.currentMonth = (currentDate >> 5) & 0xF;
    r.startYear = (startDate >> 9);
    r.startMonth = (startDate >> 5) & 0xF;

    r.hasRaidInfo = true;
    in >> r.startingMonthInclusive;
    in >> r.endingMonthInclusive;
    in >> r.numRaidsPerRaidCountLimit;
    return r;
}

AgricultureInfo::Data DataReader::readAgricultureData(QDataStream& in)
{
    AgricultureInfo::Data r;
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

TerrainData DataReader::readTerrainData(QDataStream& in)
{
    TerrainData r;
    // Terrain2Manager
    in >> r.regrownTreeCount;
    /*int prototypeIndexFromTree
     int x
     int z
     Matrix4x4 tree.matrix
     Color tree.color
     float tree.scaleModifier*/
    in.skipRawData(r.regrownTreeCount * 96);

    in >> r.treeGrouthCount;
    /*
     * int prototypeIndexFromTree
       int x
       int z
       float num
       float growthTime
       Matrix4x4 tree.matrix
       Color tree2.color
       float tree2.scaleModifier
     */
    in.skipRawData(r.treeGrouthCount * 104);

    in >> r.choppedTreeCount;
    /*int x
      int z
      float time */
    in.skipRawData(r.choppedTreeCount * 12);

    in >> r.objectsCount;
    /* int x
       int z
       int prototypeIndexFromTree
       Matrix4x4 tree.matrix
       Color tree.color
       float tree.scaleModifier
       bool tree.blockRendering
       bool tree.culledByRenderBounds
       Bounds tree.renderBounds */
    in.skipRawData(r.objectsCount * 122);
    uint countStumps;
    in >> countStumps;
    for (uint i = 0; i < countStumps; ++i) {
        /*
            Vector3 position
            Quaternion rotation
            Vector3 localScale
            string text
            float stumpAge
         */
        in.skipRawData(40);
        readArray<quint8>(in);
        in.skipRawData(4);
    }

    uint mapSize;
    in >> mapSize;
    uint total;
    in >> total;
    r.heights.resize(mapSize);
    for (uint i = 0; i < mapSize; ++i) {
        r.heights[i].resize(mapSize);
        for (uint j = 0; j < mapSize; ++j) {
            in >> r.heights[i][j];
        }
    }
    return r;
}

VillagerData DataReader::readVillager(QDataStream& in)
{
    in.skipRawData(5);
    VillagerData d;
    // Transform
    in >> d.p;
    in.skipRawData(28);
    readArray<quint8>(in); //"Player"
    // CombatComponent
    float lifePercentage;
    in >> lifePercentage;
    quint8 isWounded;
    in >> isWounded;
    if (isWounded) {
        in.skipRawData(4); // woundedTime
    }
    in.skipRawData(8); // lifeModifier damageDealtModifier
    quint8 male;
    in >> male;
    d.male = male;
    // joinReason
    in.skipRawData(4);
    in >> d.occupation;
    d.name = readArray<quint8>(in);
    // VillagerHealth
    //   CEDateTime
    float day;
    in >> day;
    in >> d.month;
    in >> d.year;
    in.skipRawData(52); // exposure diet happiness happinessShoes happinessClothes happinessLuxury happinessFood happinessHealth happinessGrief endurance dietHealthRegen exposureLevelexposureRate
    quint8 lastFireWarmthDate;
    in >> lastFireWarmthDate;
    if (lastFireWarmthDate) {
        in.skipRawData(12); //CEDateTime lastFireWarmthDate
    }
    // ConsumedFoodTracker
    //   ItemStorage[]
    uint foodEatenByMonth;
    in >> foodEatenByMonth;
    for (uint i = 0; i < foodEatenByMonth; ++i) {
        std::vector<GameItemCount> is;
        readItemStorage(in, is);
    }
    uint eathenCount;
    in >> eathenCount;
    in.skipRawData(eathenCount * 4 + 9);
    uint dietHistory;
    in >> dietHistory;
    in.skipRawData(dietHistory * 4 + 71);
    uint drunkHistory;
    in >> drunkHistory;
    in.skipRawData(drunkHistory * 4);
    auto portrait = readArray<quint8>(in); // "Male..."
    // ImmuneSystemComponent
    in.skipRawData(1); // immunities
    uint immunities;
    in >> immunities;
    for (uint i = 0; i < immunities; ++i) {
        // Disease
        readArray<quint8>(in);
    }
    uint globalImmunities;
    in >> globalImmunities;
    for (uint i = 0; i < globalImmunities; ++i) {
        // Disease
        readArray<quint8>(in);
        in.skipRawData(12); // CEDateTime
    }
    uint presentDiseases;
    in >> presentDiseases;
    if (presentDiseases > 0) {
        in.skipRawData(5);
        for (uint i = 0; i < presentDiseases; ++i) {
            // DiseaseComponent
            DiseaseType disease = parseDisease(readArray<quint8>(in)); // diseaseDef
            readArray<quint8>(in); // DiseaseComponent Guid
            //   durationTimer Remaining
            float durationDays;
            in >> durationDays;
            in.skipRawData(8); // month year
            //   cureChanceTimer Remaining
            float cureDays;
            in >> cureDays;
            in.skipRawData(8); // month year
            d.diseases.push_back(disease);
        }
    }
    // EducationComponent
    in >> d.education;
    in.skipRawData(4); // accumulatedSessions
    if (d.occupation == VillagerData::Soldier) {
        in.skipRawData(4); // stance
        uint8_t waitingHorse;
        in >> waitingHorse;
        if (waitingHorse) {
            in.skipRawData(4);
        }
        readArray<quint8>(in); // soldierTypeData
        in.skipRawData(1); // focusedOnCombatFlag
        uint8_t commandedToPoint;
        in >> commandedToPoint;
        if (commandedToPoint) {
            in.skipRawData(12);
        }
    }
    in.skipRawData(1); // isMounted
    std::vector<GameItemCount> temporaryInventory;
    readItemStorage(in, temporaryInventory);
    std::vector<GameItemCount> permanentInventory;
    readItemStorage(in, permanentInventory);
    std::vector<GameItemCount> foodInventory;
    readItemStorage(in, foodInventory);
    uint8_t homeResidence;
    in >> homeResidence;
    if (homeResidence) {
        in >> d.homeResidence;
    } else {
        d.homeResidence = 0;
    }
    uint8_t placeOfWork;
    in >> placeOfWork;
    if (placeOfWork) {
        in >> d.placeOfWork;
    } else {
        d.placeOfWork = 0;
    }
    uint8_t structureInsideOf;
    in >> structureInsideOf;
    if (structureInsideOf) {
        in.skipRawData(4);
    }
    in.skipRawData(8); // timeSpentTravelling timeSpentStill
    uint32_t timeInStateCategoriesDict;
    in >> timeInStateCategoriesDict;
    in.skipRawData(timeInStateCategoriesDict * 8);
    in >> d.veterancyKills;
    return d;
}

Point DataReader::readCamera(QDataStream& in)
{
    Point r;
    in >> r.x;
    in >> r.y;
    in >> r.z;
    return r;
}
