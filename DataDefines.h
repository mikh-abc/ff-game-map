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

enum class BaseType
{
    AgricultureManager,
    AnimalManager,
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
    TerrainManager,
    TownCenter,
    Wolf,
    WolfDen,
    Unknown
};

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
    uint amount;
    MineralType type;
};

enum class RaiderType
{
    Thief,
    Brawler,
    Warrior,
    Shieldbearer,
    Warmaster,
    Arbalest,
    Champion,
    Footman,
    HeavyInfantry,
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

struct AnimalSpawnData
{
    uint spawnArea;
    BaseType type;
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
    Logs,
    Berries,
    Stone,
    Planks,
    Bread,
    Clover,
    Firewood,
    Flour,
    Hide,
    HideCoat,
    Meat,
    RootVegetable,
    Beans,
    Greens,
    Grain,
    Mushroom,
    Roots,
    Nuts,
    Fruit,
    PreservedVeg,
    Preserves,
    Herbs,
    Eggs,
    IronOre,
    Iron,
    Tool,
    HeavyTool,
    Weapon,
    SimpleWeapon,
    HeavyWeapon,
    Shield,
    Hauberk,
    Platemail,
    Arrow,
    Brick,
    Bow,
    Crossbow,
    Fish,
    Shoes,
    Carcass,
    SmokedMeat,
    SmokedFish,
    Flax,
    LinenClothes,
    Coal,
    Clay,
    Water,
    GoldOre,
    GoldIngot,
    Pottery,
    WheatBeer,
    Honey,
    Basket,
    Willow,
    HealthyCarcass,
    UnhealthyCarcass,
    SicklyCarcass,
    Furniture,
    Tallow,
    Wax,
    Soap,
    Candle,
    Spice,
    Poop,
    Compost,
    DeceasedVillager,
    Milk,
    Cheese,
    Cow,
    LivestockToMilk,
    Sand,
    Glass,
    Barrel,
    Medicine,
    AnimalTrap,
    SmallCarcass,
    BoarCarcass,
    Unknown
};

struct ForageableData
{
    Point p;
    uint amount;
    GameItem type;
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
    Fodder,
    OriginalFodder,
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

QColor itemColor(GameItem v);
QColor mineralColor(MineralType v);

#endif // DATADEFINES_H
