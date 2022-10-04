// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software distributed under the License
// is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
// or implied.  See the License for the specific language governing permissions and limitations
// under the License.

#ifndef DATADEFINES_H
#define DATADEFINES_H

struct Point
{
    float x;
    float y;
    float z;
};

enum class MineralType
{
    Iron,
    Gold,
    Coal,
    Clay,
    Sand,
    Unknown
};

struct MineralData
{
    Point p;
    float r;
    uint count;
    MineralType type;
};

enum class RaiderType
{
    Thief,
    Brawler,
    Warrior,
    Shieldbearer,
    BatteringRam,
    Unknown
};

struct AnimalData
{
    Point p;
    std::vector<Point> spawnPoints;
    std::vector<Point> wanderPoints;
    uint spawnArea;
    float hp;
};

struct RaiderData
{
    Point p;
    Point spawn;
    RaiderType type;
    float hp;
    float p1;
    float p2;
};

enum class GameItem
{
    Mushrooms,
    Roots,
    Nuts,
    Herbs,
    Eggs,
    Willow,
    Greens,
    Berries,
    Logs,
    Unknown
};

struct ForageableData
{
    Point p;
    uint count;
    GameItem type;
};

enum class BaseType
{
    AgricultureManager,
    BatteringRam,
    Bear,
    Boar,
    BuildingBuildSite,
    BuildingBuildSiteGuids,
    CameraManager,
    Deer,
    FoWSystem,
    ForageableResource,
    MineralManager,
    MetaData,
    Raider,
    Shelter,
    TownCenter,
    Wolf,
    WolfDen,
    Unknown
};

struct BaseData
{
    Point p;
    BaseType type;
};

struct GeneralSaveData
{
    QByteArray seed;
    QByteArray version;
    QByteArray name;
    quint32 villagers = 0;
    quint32 years = 0;
    quint8 v1 = 0;
    quint8 v2 = 0;
    quint32 v3 = 0;
    quint32 v4 = 0;
    quint32 timestamp = 0;
    quint32 hours = 0;
    quint32 mins = 0;
    quint8 wildlifeDifficulty = 0;
    quint8 raidersDifficulty = 0;
    quint8 pacifist = 0;
};

namespace AgricultureInfo
{
enum DataType
{
    EnvFertility = 0,
    Fertility,
    Honey,
    OriginalHoney,
    Fooder,
    OriginalFooder,
    Water,
    OriginalWater,
    ClaySand,
    Max
};

struct Data
{
    float worldWidth = 0;
    float worldHeight = 0;
    std::map<DataType, std::vector<std::vector<float>>> data;
};

}

#endif // DATADEFINES_H
