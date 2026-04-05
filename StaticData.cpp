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
#include "StaticData.h"

const std::unordered_map<uint, BaseType> BaseTypeById = {
    {  116683775, BaseType::FlattenAreaBuildsite    },
    {  117926143, BaseType::SalvagingSite           },
    {  272625919, BaseType::FoWSystem               },
    {  274602495, BaseType::MineralManager          },
    {  482861567, BaseType::Deer                    },
    {  545559295, BaseType::Raider                  },
    {  788030719, BaseType::CameraManager           },
    {  912881919, BaseType::ForageableResource      },
    { 1358854911, BaseType::Bear                    },
    { 1357454847, BaseType::AnimalManager           },
    { 1676811007, BaseType::WolfDen                 },
    { 1767231999, BaseType::TerrainManager          },
    { 1882797823, BaseType::TimeManager             },
    { 1974594303, BaseType::RaiderCamp              },
    { 2094352639, BaseType::BatteringRam            },
    { 2116582911, BaseType::AgricultureManager      },
    { 2012388863, BaseType::RelicExtraction         },
    { 2410555647, BaseType::Villager                },
    { 2831428095, BaseType::Shelter                 },
    { 2921281535, BaseType::Wolf                    },
    { 3005595647, BaseType::Boar                    },
    { 3229217023, BaseType::PotentialImmigrant      },
    { 3556611327, BaseType::TownCenter              },
    { 3982492927, BaseType::MetaData                },
    { 3997403647, BaseType::BuildingBuildSite       },
    { 4058971987, BaseType::Guids                   }
};

BaseType parseBaseType(uint32_t id) {
    auto i = BaseTypeById.find(id);
    if (i == BaseTypeById.end()) {
        return BaseType::Unknown;
    }
    return i->second;
}

const std::vector<uint32_t>& getBaseTypes() {
    static std::vector<uint> result;
    if (result.empty()) {
        result.reserve(BaseTypeById.size());
        for (const auto& i : BaseTypeById) {
            result.push_back(i.first);
        }
    }
    return result;
}

constexpr char Bear_Hard[]     = "7361af1e-8897-4099-b372-dfe646d10328";
constexpr char Bear_Normal[]   = "ac4a8fd7-b9cb-44e4-be78-105e11c07bd1";
constexpr char Bear_VeryHard[] = "621e052e-99cb-483f-8cd8-6f0612e6d32c";

constexpr char Boar_Easy[]     = "ad9b2d3d-30fb-49a8-a688-724ec2dd3c1d";
constexpr char Boar_Hard[]     = "f0e7a583-81bc-4efe-8723-d5f3a602420d";
constexpr char Boar_Normal[]   = "9347bbe9-23eb-4f2a-90ab-f9620a2d8665";
constexpr char Boar_VeryHard[] = "037681cb-0f6a-4431-bd1d-a62f629bad73";

constexpr char Deer_Easy[]     = "b4849983-c710-48e9-9d47-93f65369640b";
constexpr char Deer_Hard[]     = "6b4c3a15-9875-467e-83da-c9dc121fae87";
constexpr char Deer_Normal[]   = "1b89ffdd-78ca-4dc0-bbd4-cd81cda57fe1";
constexpr char Deer_VeryHard[] = "9f9d0ef7-becd-4ddd-b219-2cc3898d930e";

constexpr char Wolf_Hard[]             = "301d292d-b239-45fe-a021-83e1a08f1d69"; // WolfDen
constexpr char Wolf_Hard_Roaming[]     = "2fb21f39-f10a-43f2-80f2-cde6e225ef6e";
constexpr char Wolf_Normal[]           = "43c882e6-a835-4a85-a028-186c8a53e855"; // WolfDen
constexpr char Wolf_VeryHard[]         = "ef151ac7-4e23-4687-95a5-bb9cd76eb28a"; // WolfDen
constexpr char Wolf_VeryHard_Roaming[] = "c8ec56cb-e421-4d59-90cd-513dfe95d1a2";

constexpr char Wolf_GameEffect_Normal[]   = "95de49e0-f4dd-4f77-967c-c6832732585a";
constexpr char Wolf_GameEffect_Hard[]     = "6740a8ab-3747-4287-ab82-dd2b0d999c7c";
constexpr char Wolf_GameEffect_VeryHard[] = "73212ffa-d2d9-4b27-b07c-4ef6b62edf86";


const std::unordered_map<QByteArray, BaseType> AnimalsSpawnTypeByUUID = {
    {  Bear_Hard, BaseType::Bear     },
    {  Bear_Normal, BaseType::Bear   },
    {  Bear_VeryHard, BaseType::Bear },

    {  Boar_Easy, BaseType::Boar     },
    {  Boar_Hard, BaseType::Boar     },
    {  Boar_Normal, BaseType::Boar   },
    {  Boar_VeryHard, BaseType::Boar },

    {  Deer_Easy, BaseType::Deer     },
    {  Deer_Hard, BaseType::Deer     },
    {  Deer_Normal, BaseType::Deer   },
    {  Deer_VeryHard, BaseType::Deer },

    {  Wolf_Hard, BaseType::Wolf                },
    {  Wolf_Hard_Roaming, BaseType::Wolf        },
    {  Wolf_Normal, BaseType::Wolf              },
    {  Wolf_VeryHard, BaseType::Wolf            },
    {  Wolf_VeryHard_Roaming, BaseType::Wolf    },
    {  Wolf_GameEffect_Normal, BaseType::Wolf   },
    {  Wolf_GameEffect_Hard, BaseType::Wolf     },
    {  Wolf_GameEffect_VeryHard, BaseType::Wolf }
};

const std::vector<std::unordered_map<BaseType, QByteArray>> AnimalsSpawnTypeToUUID = {
    { {BaseType::Deer, Deer_Easy}, {BaseType::Boar, Boar_Easy} },
    { {BaseType::Deer, Deer_Normal}, {BaseType::Boar, Boar_Normal}, {BaseType::Wolf, Wolf_Normal}, {BaseType:: Bear, Bear_Normal} },
    { {BaseType::Deer, Deer_Hard}, {BaseType::Boar, Boar_Hard}, {BaseType::Wolf, Wolf_Hard_Roaming}, {BaseType:: Bear, Bear_Hard} },
    { {BaseType::Deer, Deer_VeryHard}, {BaseType::Boar, Boar_VeryHard}, {BaseType::Wolf, Wolf_VeryHard_Roaming}, {BaseType:: Bear, Bear_VeryHard} }
};

BaseType parseAnimalsSpawnType(const QByteArray &uuid) {
    auto i = AnimalsSpawnTypeByUUID.find(uuid);
    if (i == AnimalsSpawnTypeByUUID.end()) {
        return BaseType::Unknown;
    }
    return i->second;
}

const QByteArray& convertAnimalsSpawnTypeToUUID(uint32_t difficult, BaseType baseType) {
    static const QByteArray empty;
    if (difficult >= AnimalsSpawnTypeToUUID.size())
        return empty;
    const auto& map = AnimalsSpawnTypeToUUID[difficult];
    auto i = map.find(baseType);
    if (i == map.end()) {
        return empty;
    }
    return i->second;
}

const std::unordered_map<QByteArray, RaiderType> RaiderTypeByUUID = {
    {  "7b8e2ab8-2511-4b09-b2a3-7d020d200654", RaiderType::Thief           }, // RaiderUnit_Thief
    {  "ec4c5a27-2ca4-4c3f-9043-df53ebb9ad23", RaiderType::Thief           }, // RaiderUnit_ThiefFood
    {  "f7eb7176-4b2d-450b-b991-aa4854de3b70", RaiderType::Brawler         }, // RaiderUnit_Brawler_Male
    {  "e2f3921a-a92c-4eef-9b80-1d4990d53d96", RaiderType::Brawler         }, // RaiderUnit_Brawler_Female
    {  "d46bd838-4635-4248-917b-a8bb8df8bbc2", RaiderType::Warrior         }, // RaiderUnit_Warrior_Male
    {  "8937422c-1758-4903-8760-8a559515dfa7", RaiderType::Warrior         }, // RaiderUnit_Warrior_Female
    {  "0e001414-c18d-41fc-a5ea-6af77c526177", RaiderType::Archer          }, // RaiderUnit_Archer
    {  "11e97f97-5051-496e-8992-280107d05806", RaiderType::Pikeman         }, // RaiderUnit_Pikeman_Male
    {  "bfdd1c15-504d-495d-b9e0-402799ea1cdb", RaiderType::Pikeman         }, // RaiderUnit_Pikeman_Female
    {  "1c6eff9d-3d70-4758-8688-b0184883c41a", RaiderType::Shieldbearer    }, // RaiderUnit_Elite
    {  "b6d2492e-bd13-4012-9481-b2cd8f66094c", RaiderType::Archer          }, // RaiderUnit_Archer_Elite
    {  "2c5eff88-1435-43a7-9542-5cbbe7e223eb", RaiderType::Warmaster       }, // RaiderUnit_Champion

    {  "c7337224-db26-4932-9e8c-6e1ba260a7fe", RaiderType::Brawler         }, // RaiderUnit_RaidCamp_Brawler_Male
    {  "cf0ad0d2-367b-45d8-972c-6ad7964a2513", RaiderType::Brawler         }, // RaiderUnit_RaidCamp_Brawler_Female
    {  "8ab74b04-b9f6-40ef-8f4d-0cd0d83e621a", RaiderType::Warrior         }, // RaiderUnit_RaidCamp_Warrior_Male
    {  "c957677c-020a-4a68-9766-fae00b8e2c85", RaiderType::Warrior         }, // RaiderUnit_RaidCamp_Warrior_Female
    {  "7740afb9-8056-4c86-b6a2-caeb1dbf8844", RaiderType::Archer          }, // RaiderUnit_RaidCamp_Archer
    {  "72a22f6c-ab8e-41d5-9f37-83dd829e7e1c", RaiderType::Shieldbearer    }, // RaiderUnit_RaidCamp_Elite
    {  "8b0fed13-a1af-45c6-b6c8-cb486d9b48a4", RaiderType::Warmaster       }, // RaiderUnit_RaidCamp_Champion
    {  "c7e22203-d420-4e22-a8c5-76b128fc5162", RaiderType::Cavalry         }, // RaiderUnit_RaidCamp_Horseman

    {  "93fc8b22-7dcf-4103-a5b8-7fe0d86fd46b", RaiderType::Arbalest        }, // InvaderUnit_Arbalest
    {  "ba493164-2dc4-45f6-9560-5d35e7c4e4bf", RaiderType::Footman         }, // InvaderUnit_Footman
    {  "83a57f3d-37d5-45f0-9b20-ad55603a27ac", RaiderType::HeavyInfantry   }, // InvaderUnit_HeavyInfantry
    {  "96238dda-f6f4-4c5e-b8fb-923332194bcd", RaiderType::Champion        }, // InvaderUnit_Champion
    {  "8c506a65-72d9-4150-ad05-478dbec8f7c5", RaiderType::Cavalry         }, // InvaderUnit_CavalryHeavy
    {  "cc055493-ecea-4142-ae3a-2ec4d6edc537", RaiderType::Pikeman         }, // InvaderUnit_Pikeman
};

RaiderType parseRaiderType(const QByteArray &uuid) {
    auto i = RaiderTypeByUUID.find(uuid);
    if (i == RaiderTypeByUUID.end()) {
        return RaiderType::Unknown;
    }
    return i->second;
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

uint mineralTypeId(MineralType type)
{
    switch (type)
    {
    case MineralType::Iron:
        return 0;
    case MineralType::Gold:
        return 1;
    case MineralType::Coal:
        return 2;
    case MineralType::Clay:
    case MineralType::Sand:
    case MineralType::Stone:
    case MineralType::Unknown:
        break;
    }
    return 0;
}


const std::unordered_map<QByteArray, GameItem> GameItemByText = {
    {  "ItemWork", GameItem::Work },
    {  "ItemLogs", GameItem::Logs },
    {  "ItemBerries", GameItem::Berries },
    {  "ItemStone", GameItem::Stone },
    {  "ItemPlanks", GameItem::Planks },
    {  "ItemBread", GameItem::Bread },
    {  "ItemClover", GameItem::Clover },
    {  "ItemFirewood", GameItem::Firewood },
    {  "ItemFlour", GameItem::Flour },
    {  "ItemHide", GameItem::Hide },
    {  "ItemHideCoat", GameItem::HideCoat },
    {  "ItemMeat", GameItem::Meat },
    {  "ItemRootVegetable", GameItem::RootVegetable },
    {  "ItemBeans", GameItem::Beans },
    {  "ItemGreens", GameItem::Greens },
    {  "ItemGrain", GameItem::Grain },
    {  "ItemHay", GameItem::Hay },
    {  "ItemMushroom", GameItem::Mushroom },
    {  "ItemRoots", GameItem::Roots },
    {  "ItemNuts", GameItem::Nuts },
    {  "ItemFruit", GameItem::Fruit },
    {  "ItemPreservedVeg", GameItem::PreservedVeg },
    {  "ItemPreserves", GameItem::Preserves },
    {  "ItemHerbs", GameItem::Herbs },
    {  "ItemEggs", GameItem::Eggs },
    {  "ItemIronOre", GameItem::IronOre },
    {  "ItemIron", GameItem::Iron },
    {  "ItemTool", GameItem::Tool },
    {  "ItemHeavyTool", GameItem::HeavyTool },
    {  "ItemWeapon", GameItem::Weapon },
    {  "ItemSimpleWeapon", GameItem::SimpleWeapon },
    {  "ItemHeavyWeapon", GameItem::HeavyWeapon },
    {  "ItemShield", GameItem::Shield },
    {  "ItemHauberk", GameItem::Hauberk },
    {  "ItemPlatemail", GameItem::Platemail },
    {  "ItemArrow", GameItem::Arrow },
    {  "ItemBooks", GameItem::Books },
    {  "ItemPaper", GameItem::Paper },
    {  "ItemBrick", GameItem::Brick },
    {  "ItemBow", GameItem::Bow },
    {  "ItemCrossbow", GameItem::Crossbow },
    {  "ItemFish", GameItem::Fish },
    {  "ItemShoes", GameItem::Shoes },
    {  "ItemCarcass", GameItem::Carcass },
    {  "ItemBoarCarcass", GameItem::BoarCarcass },
    {  "ItemSmokedMeat", GameItem::SmokedMeat },
    {  "ItemSmokedFish", GameItem::SmokedFish },
    {  "ItemFlax", GameItem::Flax },
    {  "ItemLinenClothes", GameItem::LinenClothes },
    {  "ItemCoal", GameItem::Coal },
    {  "ItemClay", GameItem::Clay },
    {  "ItemWater", GameItem::Water },
    {  "ItemGoldOre", GameItem::GoldOre },
    {  "ItemGoldIngot", GameItem::GoldIngot },
    {  "ItemPottery", GameItem::Pottery },
    {  "ItemWheatBeer", GameItem::WheatBeer },
    {  "ItemHoney", GameItem::Honey },
    {  "ItemBasket", GameItem::Basket },
    {  "ItemWillow", GameItem::Willow },
    {  "ItemHealthyCarcass", GameItem::HealthyCarcass },
    {  "ItemUnhealthyCarcass", GameItem::UnhealthyCarcass },
    {  "ItemSicklyCarcass", GameItem::SicklyCarcass },
    {  "ItemFurniture", GameItem::Furniture },
    {  "ItemTallow", GameItem::Tallow },
    {  "ItemWax", GameItem::Wax },
    {  "ItemSoap", GameItem::Soap },
    {  "ItemCandle", GameItem::Candle },
    {  "ItemSpice", GameItem::Spice },
    {  "ItemPoop", GameItem::Poop },
    {  "ItemCompost", GameItem::Compost },
    {  "ItemDeceasedVillager", GameItem::DeceasedVillager },
    {  "ItemMilk", GameItem::Milk },
    {  "ItemCheese", GameItem::Cheese },
    {  "ItemCow", GameItem::Cow },
    {  "ItemLivestockToMilk", GameItem::LivestockToMilk },
    {  "ItemSand", GameItem::Sand },
    {  "ItemGlass", GameItem::Glass },
    {  "ItemBarrel", GameItem::Barrel },
    {  "ItemMedicine", GameItem::Medicine },
    {  "ItemAnimalTrap", GameItem::AnimalTrap },
    {  "ItemSmallCarcass", GameItem::SmallCarcass },
    {  "ItemHorse", GameItem::Horse },
    {  "ItemGoat", GameItem::Goat },
    {  "ItemChicken", GameItem::Chicken },
    {  "ItemWolfCarcass", GameItem::WolfCarcass },
    {  "ItemPastry", GameItem::Pastry }
};

GameItem parseItem(const QByteArray& v)
{
    auto i = GameItemByText.find(v);
    if (i == GameItemByText.end()) {
        return GameItem::MAX;
    }
    return i->second;
}

QDataStream& operator>>(QDataStream& in, Point& rhs) {
    in >> rhs.x;
    in >> rhs.y;
    in >> rhs.z;
    return in;
}

QDataStream& operator<<(QDataStream& out, const Point& rhs) {
    out << rhs.x;
    out << rhs.y;
    out << rhs.z;
    return out;
}

const std::unordered_map<QByteArray, DiseaseType> DiseaseTypeByUUID = {
    { "13b5f5e9-b7ec-49cc-839a-b9d4a8f942f1", DiseaseType::CD_Typhoid },
    { "b694fb0f-0f91-428e-a088-f03fea19cd47", DiseaseType::PD_Pneumonia },
    { "224c1829-5cc3-4694-b29c-9e51a0b5b5fe", DiseaseType::CD_Smallpox },
    { "bdf3d942-3c30-4827-b1ed-19a696d2e39a", DiseaseType::CD_BubonicPlague },
    { "03a08e19-120b-420f-95ea-8e60bbeb735a", DiseaseType::DisabilityPermanent_Bedridden },
    { "b178725f-b9da-4a57-ace0-9b1e61bb8d55", DiseaseType::CD_Flu },
    { "d9d0bab4-7301-4de8-9137-d9b422f9e76e", DiseaseType::CD_Cholera },
    { "de7d1883-ed96-4b29-ac74-b12705f2ea9b", DiseaseType::CD_Worms },
    { "90cc8919-3a3c-42dc-aea1-95b636295777", DiseaseType::PD_BrokenLeg },
    { "5cc78c71-6176-4f2d-994e-52f44b470152", DiseaseType::PD_Gangrene },
    { "a6d93856-36df-452a-a5d0-2c53cbd83889", DiseaseType::PD_Dysentery },
    { "a0706382-51bb-4421-a032-6c3a8b1731f0", DiseaseType::PD_Frostbite },
    { "8250abbc-d69b-4202-b304-e26130b6829b", DiseaseType::PD_Scurvy },
    { "456b00d9-7766-4266-b529-076ad2f5a4aa", DiseaseType::DisabilityTemporary_Bedridden },
    { "1762ff2e-0e6e-483a-ba9a-78d415d26da4", DiseaseType::PD_FootWound },
    { "3e44918c-cd2d-42bb-bc19-8db453fd66e7", DiseaseType::PD_Tetanus },
    { "f7ffe997-d40d-4ef6-b55e-0b33451fe454", DiseaseType::PD_Dehydration },
    { "6693bcc7-254f-485e-ad2d-40792c86b50c", DiseaseType::PD_RateBiteFever },
    { "7597bce8-d5c4-4d94-bf69-c635fe070a1c", DiseaseType::PD_BrokenBone },
    { "0a78c330-fb12-4cb4-b70b-1cbea51121d7", DiseaseType::PD_Infection },
    { "40816427-166f-4ba9-9a0d-651193bbf4c3", DiseaseType::PD_EagleAttack },
    { "f2a614cb-9b72-49e9-a86f-fb13d2e23424", DiseaseType::PD_BowelObstruction },
    { "0e670dcc-79bb-40cb-b667-5409504e0f85", DiseaseType::PD_FoodPoisoning },
    { "f82261e0-cb5f-41b4-863d-16d4fcc3da11", DiseaseType::PD_Gout },
    { "c3c9fe11-0ee8-4c15-bbcf-2bc5f49feca3", DiseaseType::PD_BeeSting },
    { "73473b2c-4006-419d-9e66-2621db731e64", DiseaseType::PD_Rabies },
    { "efda6234-f43f-4a11-b08d-192d92fa031c", DiseaseType::PD_Wound_01 }
};

DiseaseType parseDisease(const QByteArray &v)
{
    auto i = DiseaseTypeByUUID.find(v);
    if (i == DiseaseTypeByUUID.end()) {
        return DiseaseType::Unknown;
    }
    return i->second;
}

QLatin1String mineralStr(MineralType v)
{
    switch (v)
    {
    case MineralType::Iron:
        return QLatin1String("Iron");
    case MineralType::Gold:
        return QLatin1String("Gold");
    case MineralType::Coal:
        return QLatin1String("Coal");
    case MineralType::Clay:
        return QLatin1String("Clay");
    case MineralType::Sand:
        return QLatin1String("Sand");
    case MineralType::Stone:
        return QLatin1String("Stone");
    case MineralType::Unknown:
        break;
    }
    return QLatin1String();
}

QLatin1String educationStr(uint32_t e)
{
    switch (e)
    {
    case 0:
        return QLatin1String("None");
    case 1:
        return QLatin1String("Basic");
    }
    return QLatin1String();
}

QLatin1String occupationStr(VillagerData::Occupation c)
{
    switch (c)
    {
    case VillagerData::Laborer:
        return QLatin1String("Laborer");
    case VillagerData::Hunter:
        return QLatin1String("Hunter");
    case VillagerData::Builder:
        return QLatin1String("Builder");
    case VillagerData::Woodcutter:
        return QLatin1String("Woodcutter");
    case VillagerData::Sawyer:
        return QLatin1String("Sawyer");
    case VillagerData::Farmer:
        return QLatin1String("Farmer");
    case VillagerData::Baker:
        return QLatin1String("Baker");
    case VillagerData::Tanner:
        return QLatin1String("Tanner");
    case VillagerData::Miller:
        return QLatin1String("Miller");
    case VillagerData::Guard:
        return QLatin1String("Guard");
    case VillagerData::Miner:
        return QLatin1String("Miner");
    case VillagerData::Foundryman:
        return QLatin1String("Foundryman");
    case VillagerData::Blacksmith:
        return QLatin1String("Blacksmith");
    case VillagerData::TransitionToSoldier:
        return QLatin1String("Soldier");
    case VillagerData::Fletcher:
        return QLatin1String("Fletcher");
    case VillagerData::Fisherman:
        return QLatin1String("Fisherman");
    case VillagerData::Cobbler:
        return QLatin1String("Cobbler");
    case VillagerData::Smoker:
        return QLatin1String("Smoker");
    case VillagerData::Weaver:
        return QLatin1String("Weaver");
    case VillagerData::CharcoalMaker:
        return QLatin1String("Charcoal Maker");
    case VillagerData::Potter:
        return QLatin1String("Potter");
    case VillagerData::Child:
        return QLatin1String("Child");
    case VillagerData::BoatBuilder:
        return QLatin1String("Boat Builder");
    case VillagerData::Forager:
        return QLatin1String("Forager");
    case VillagerData::Brewer:
        return QLatin1String("Brewer");
    case VillagerData::Wainwright:
        return QLatin1String("Wainwright");
    case VillagerData::Publican:
        return QLatin1String("Publican");
    case VillagerData::BasketMaker:
        return QLatin1String("Basket Maker");
    case VillagerData::WorkCampLaborer:
        return QLatin1String("Work Camp Laborer");
    case VillagerData::Deserter:
        return QLatin1String("Deserter");
    case VillagerData::Trader:
        return QLatin1String("Trader");
    case VillagerData::Herder:
        return QLatin1String("Herder");
    case VillagerData::Healer:
        return QLatin1String("Healer");
    case VillagerData::FurnitureMaker:
        return QLatin1String("Furniture Maker");
    case VillagerData::Disabled:
        return QLatin1String("Disabled");
    case VillagerData::SoapMaker:
        return QLatin1String("Soap Maker");
    case VillagerData::Chandler:
        return QLatin1String("Chandler");
    case VillagerData::NightsoilMan:
        return QLatin1String("Nightsoil Man");
    case VillagerData::Teacher:
        return QLatin1String("Teacher");
    case VillagerData::Glassmaker:
        return QLatin1String("Glassmaker");
    case VillagerData::Brickmaker:
        return QLatin1String("Brickmaker");
    case VillagerData::Cheesemaker:
        return QLatin1String("Cheesemaker");
    case VillagerData::Cooper:
        return QLatin1String("Cooper");
    case VillagerData::Apothecary:
        return QLatin1String("Apothecary");
    case VillagerData::Soldier:
        return QLatin1String("Soldier");
    case VillagerData::Grocer:
        return QLatin1String("Grocer");
    case VillagerData::Armourer:
        return QLatin1String("Armourer");
    case VillagerData::RatCatcher:
        return QLatin1String("Rat Catcher");
    case VillagerData::Arborist:
        return QLatin1String("Arborist");
    case VillagerData::Preservist:
        return QLatin1String("Preservist");
    case VillagerData::Papermaker:
        return QLatin1String("Papermaker");
    case VillagerData::BookBinder:
        return QLatin1String("Book Binder");
    case VillagerData::Librarian:
        return QLatin1String("Librarian");
    case VillagerData::Priest:
        return QLatin1String("Priest");
    case VillagerData::Guildmaster:
        return QLatin1String("Guildmaster");
    case VillagerData::Scholar:
        return QLatin1String("Scholar");
    case VillagerData::None:
        return QLatin1String("[None]");
        break;
    }
    return QLatin1String();
}

QLatin1String raiderStr(RaiderType e)
{
    switch (e)
    {

    case RaiderType::Thief:
        return QLatin1String("Thief");
    case RaiderType::Brawler:
        return QLatin1String("Brawler");
    case RaiderType::Warrior:
        return QLatin1String("Warrior");
    case RaiderType::Shieldbearer:
        return QLatin1String("Shieldbearer");
    case RaiderType::Warmaster:
        return QLatin1String("Warmaster");
    case RaiderType::Arbalest:
        return QLatin1String("Arbalest");
    case RaiderType::Champion:
        return QLatin1String("Champion");
    case RaiderType::Footman:
        return QLatin1String("Footman");
    case RaiderType::HeavyInfantry:
        return QLatin1String("HeavyInfantry");
    case RaiderType::BatteringRam:
        return QLatin1String("BatteringRam");
    case RaiderType::RaiderCamp:
        return QLatin1String("RaiderCamp");
    case RaiderType::Archer:
        return QLatin1String("Archer");
    case RaiderType::Cavalry:
        return QLatin1String("Cavalry");
    case RaiderType::Pikeman:
        return QLatin1String("Pikeman");
    case RaiderType::Unknown:
        break;
    }

    return QLatin1String();
}

