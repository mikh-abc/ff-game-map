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

const uint32_t ModTag = 1;
constexpr float AnimalSpawnAreaSize = 64;
constexpr float CellSize = 5;

enum class BaseType
{
    AgricultureManager,
    AnimalManager,
    BatteringRam,
    Bear,
    Boar,
    BuildingBuildSite,
    CameraManager,
    Deer,
    FlattenAreaBuildsite,
    FoWSystem,
    ForageableResource,
    Guids,
    MineralManager,
    MetaData,
    PotentialImmigrant,
    Raider,
    RaiderCamp,
    RelicExtraction,
    SalvagingSite,
    Shelter,
    TerrainManager,
    TimeManager,
    TownCenter,
    Villager,
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
    Stone,
    FertilityBonus,
    Unknown
};

struct MineralData
{
    Point p;
    float r;
    uint32_t amount;
    uchar deep;
    MineralType type;
};

enum class DiseaseType
{
    Unknown,
    CD_Typhoid,
    PD_Pneumonia,
    CD_Smallpox,
    CD_BubonicPlague,
    DisabilityPermanent_Bedridden,
    CD_Flu,
    CD_Cholera,
    CD_Worms,
    PD_BrokenLeg,
    PD_Gangrene,
    PD_Dysentery,
    PD_Frostbite,
    PD_Scurvy,
    DisabilityTemporary_Bedridden,
    PD_FootWound,
    PD_Tetanus,
    PD_Dehydration,
    PD_RateBiteFever,
    PD_BrokenBone,
    PD_Infection,
    PD_EagleAttack,
    PD_BowelObstruction,
    PD_FoodPoisoning,
    PD_Gout,
    PD_BeeSting,
    PD_Rabies,
    PD_Wound_01
};

enum class RaiderType
{
    Thief,
    Brawler,
    Archer,
    Warrior,
    Shieldbearer,
    Warmaster,
    Arbalest,
    Champion,
    Footman,
    HeavyInfantry,
    Cavalry,
    Pikeman,
    BatteringRam,
    RaiderCamp,
    Unknown
};

struct AnimalData
{
    Point p;
    std::vector<Point> spawnPoints;
    std::vector<Point> wanderPoints;
    uint32_t spawnArea;
    float hp;
};

struct AnimalSpawnData
{
    BaseType type;
    QByteArray uuid;
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

struct VillagerData
{
    enum Occupation
    {
        Laborer = 0,
        Hunter = 1,
        Builder = 2,
        Woodcutter = 3,
        Sawyer = 4,
        Farmer = 5,
        Baker = 6,
        Tanner = 7,
        Miller = 8,
        Guard = 9,
        Miner = 10,
        Foundryman = 11,
        Blacksmith = 12,
        TransitionToSoldier = 13,
        Fletcher = 14,
        Fisherman = 15,
        Cobbler = 16,
        Smoker = 17,
        Weaver = 18,
        CharcoalMaker = 19,
        Potter = 20,
        Child = 21,
        BoatBuilder = 22,
        Forager = 23,
        Brewer = 24,
        Wainwright = 25,
        Publican = 26,
        BasketMaker = 27,
        WorkCampLaborer = 28,
        Deserter = 29,
        Trader = 30,
        Herder = 31,
        Healer = 33,
        FurnitureMaker = 34,
        Disabled = 35,
        SoapMaker = 36,
        Chandler = 37,
        NightsoilMan = 38,
        Teacher = 39,
        Glassmaker = 40,
        Brickmaker = 41,
        Cheesemaker = 42,
        Cooper = 43,
        Apothecary = 44,
        Soldier = 45,
        Grocer = 46,
        Armourer = 47,
        RatCatcher = 48,
        Arborist = 49,
        Preservist = 50,
        Papermaker = 51,
        BookBinder = 52,
        Librarian = 53,
        Priest = 54,
        Guildmaster = 55,
        Scholar = 57,
        None = 58
    };

    Point p;
    float hp;
    bool male;
    Occupation occupation;
    QByteArray name;
    uint32_t education;
    uint32_t year;
    uint32_t month;
    uint32_t homeResidence;
    uint32_t placeOfWork;
    float veterancyKills;

    std::vector<DiseaseType> diseases;
};

enum class GameItem
{
    Unassigned,
    Work,
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
    BoarCarcass,
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
    Books,
    Paper,
    Horse,
    WolfCarcass,
    Hay,
    Goat,
    Chicken,
    Pastry,
    MAX
};

struct GameItemCount
{
    GameItem item;
    uint32_t count;
};

struct ForageableData
{
    Point p;
    std::vector<GameItemCount> available;
    std::vector<GameItemCount> replenish;
    [[deprecated]] GameItem type;
    [[deprecated]] uint32_t amount;
};

struct BaseData
{
    uint32_t id;
    Point p;
    BaseType type;
};

struct ModData
{
    int32_t monthsPassed = 0;
    int32_t monthsToNextRaid = 0;
    int32_t monthsUntilRaidAttacksCanResume = 0;
    std::vector<std::pair<int32_t, int32_t>> previousRaidDates;
};

struct GeneralSaveData
{
    QByteArray seed;
    QByteArray version;
    QByteArray name;
    uint32_t villagers = 0;
    uint32_t currentYear = 0;
    uint32_t currentMonth = 0;
    uint32_t startYear = 0;
    uint32_t startMonth = 0;
    uint8_t resourcesDifficulty = 0;
    uint8_t diseaseDifficulty = 0;
    uint8_t wildlifeDifficulty = 0;
    uint8_t raidersDifficulty = 0;
    uint8_t difficultyLevel = 0;
    uint32_t timestamp = 0;
    uint32_t hours = 0;
    uint32_t mins = 0;
    uint8_t pacifist = 0;
    bool hasRaidInfo = false;
    uint32_t startingMonthInclusive = 0;
    uint32_t endingMonthInclusive = 0;
    uint32_t numRaidsPerRaidCountLimit = 0;
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
    GoatFodder,
    GoatOriginalFodder,
    Water,
    OriginalWater,
    ShorelineInfluenced,
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

struct TerrainData
{
    uint32_t regrownTreeCount = 0;
    uint32_t treeGrouthCount = 0;
    uint32_t choppedTreeCount = 0;
    uint32_t objectsCount = 0;
    std::vector<std::vector<float>> heights;
};

QColor itemColor(GameItem v);
QColor mineralColor(MineralType v);
std::pair<QColor, QColor> animalSpawnColor(BaseType v);

#endif // DATADEFINES_H
