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
#include "ParseData.h"

const std::unordered_map<uint, BaseType> BaseTypeById = {
    {  272625919, BaseType::FoWSystem              },
    {  274602495, BaseType::MineralManager         },
    {  482861567, BaseType::Deer                   },
    {  545559295, BaseType::Raider                 },
    {  788030719, BaseType::CameraManager          },
    {  912881919, BaseType::ForageableResource     },
    { 1358854911, BaseType::Bear                   },
    { 1357454847, BaseType::AnimalManager          },
    { 1676811007, BaseType::WolfDen                },
    { 1767231999, BaseType::TerrainManager         },
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

const std::unordered_map<QByteArray, BaseType> AnimalsSpawnTypeByUUID = {
    {  "7361af1e-8897-4099-b372-dfe646d10328", BaseType::Bear    }, // Bear_Hard
    {  "ac4a8fd7-b9cb-44e4-be78-105e11c07bd1", BaseType::Bear    }, // Bear_Normal
    {  "621e052e-99cb-483f-8cd8-6f0612e6d32c", BaseType::Bear    }, // Bear_VeryHard

    {  "ad9b2d3d-30fb-49a8-a688-724ec2dd3c1d", BaseType::Boar    }, // Boar_Easy
    {  "f0e7a583-81bc-4efe-8723-d5f3a602420d", BaseType::Boar    }, // Boar_Hard
    {  "9347bbe9-23eb-4f2a-90ab-f9620a2d8665", BaseType::Boar    }, // Boar_Normal
    {  "037681cb-0f6a-4431-bd1d-a62f629bad73", BaseType::Boar    }, // Boar_VeryHard

    {  "b4849983-c710-48e9-9d47-93f65369640b", BaseType::Deer    }, // Deer_Easy
    {  "6b4c3a15-9875-467e-83da-c9dc121fae87", BaseType::Deer    }, // Deer_Hard
    {  "1b89ffdd-78ca-4dc0-bbd4-cd81cda57fe1", BaseType::Deer    }, // Deer_Normal
    {  "9f9d0ef7-becd-4ddd-b219-2cc3898d930e", BaseType::Deer    }, // Deer_VeryHard

    {  "301d292d-b239-45fe-a021-83e1a08f1d69", BaseType::Wolf    }, // Wolf_Hard
    {  "2fb21f39-f10a-43f2-80f2-cde6e225ef6e", BaseType::Wolf    }, // Wolf_Hard_Roaming
    {  "43c882e6-a835-4a85-a028-186c8a53e855", BaseType::Wolf    }, // Wolf_Normal
    {  "ef151ac7-4e23-4687-95a5-bb9cd76eb28a", BaseType::Wolf    }, // Wolf_VeryHard
    {  "c8ec56cb-e421-4d59-90cd-513dfe95d1a2", BaseType::Wolf    }  // Wolf_VeryHard_Roaming
};

BaseType parseAnimalsSpawnType(const QByteArray &uuid) {
    auto i = AnimalsSpawnTypeByUUID.find(uuid);
    if (i == AnimalsSpawnTypeByUUID.end()) {
        return BaseType::Unknown;
    }
    return i->second;
}

const std::unordered_map<QByteArray, RaiderType> RaiderTypeByUUID = {
    {  "7b8e2ab8-2511-4b09-b2a3-7d020d200654", RaiderType::Thief           }, // RaiderUnit_Thief
    {  "f7eb7176-4b2d-450b-b991-aa4854de3b70", RaiderType::Brawler         }, // RaiderUnit_Brawler_Male
    {  "e2f3921a-a92c-4eef-9b80-1d4990d53d96", RaiderType::Brawler         }, // RaiderUnit_Brawler_Female
    {  "d46bd838-4635-4248-917b-a8bb8df8bbc2", RaiderType::Warrior         }, // RaiderUnit_Warrior_Male
    {  "8937422c-1758-4903-8760-8a559515dfa7", RaiderType::Warrior         }, // RaiderUnit_Warrior_Female
    {  "1c6eff9d-3d70-4758-8688-b0184883c41a", RaiderType::Shieldbearer    }, // RaiderUnit_Elite
    {  "2c5eff88-1435-43a7-9542-5cbbe7e223eb", RaiderType::Warmaster       }, // RaiderUnit_Champion

    {  "93fc8b22-7dcf-4103-a5b8-7fe0d86fd46b", RaiderType::Arbalest        }, // InvaderUnit_Arbalest
    {  "96238dda-f6f4-4c5e-b8fb-923332194bcd", RaiderType::Champion        }, // InvaderUnit_Champion
    {  "ba493164-2dc4-45f6-9560-5d35e7c4e4bf", RaiderType::Footman         }, // InvaderUnit_Footman
    {  "83a57f3d-37d5-45f0-9b20-ad55603a27ac", RaiderType::HeavyInfantry   }  // InvaderUnit_HeavyInfantry

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
    }
    return 0;
}


const std::unordered_map<QByteArray, GameItem> GameItemByText = {
    {  "ItemLogs", GameItem::Logs         },
    {  "ItemBerries", GameItem::Berries         },
    {  "ItemStone", GameItem::Stone         },
    {  "ItemPlanks", GameItem::Planks         },
    {  "ItemBread", GameItem::Bread         },
    {  "ItemClover", GameItem::Clover         },
    {  "ItemFirewood", GameItem::Firewood         },
    {  "ItemFlour", GameItem::Flour         },
    {  "ItemHide", GameItem::Hide         },
    {  "ItemHideCoat", GameItem::HideCoat         },
    {  "ItemMeat", GameItem::Meat         },
    {  "ItemRootVegetable", GameItem::RootVegetable         },
    {  "ItemBeans", GameItem::Beans         },
    {  "ItemGreens", GameItem::Greens         },
    {  "ItemGrain", GameItem::Grain         },
    {  "ItemMushroom", GameItem::Mushroom         },
    {  "ItemRoots", GameItem::Roots         },
    {  "ItemNuts", GameItem::Nuts         },
    {  "ItemFruit", GameItem::Fruit         },
    {  "ItemPreservedVeg", GameItem::PreservedVeg         },
    {  "ItemPreserves", GameItem::Preserves         },
    {  "ItemHerbs", GameItem::Herbs         },
    {  "ItemEggs", GameItem::Eggs         },
    {  "ItemIronOre", GameItem::IronOre         },
    {  "ItemIron", GameItem::Iron         },
    {  "ItemTool", GameItem::Tool         },
    {  "ItemHeavyTool", GameItem::HeavyTool         },
    {  "ItemWeapon", GameItem::Weapon         },
    {  "ItemSimpleWeapon", GameItem::SimpleWeapon         },
    {  "ItemHeavyWeapon", GameItem::HeavyWeapon         },
    {  "ItemShield", GameItem::Shield         },
    {  "ItemHauberk", GameItem::Hauberk         },
    {  "ItemPlatemail", GameItem::Platemail         },
    {  "ItemArrow", GameItem::Arrow         },
    {  "ItemBrick", GameItem::Brick         },
    {  "ItemBow", GameItem::Bow         },
    {  "ItemCrossbow", GameItem::Crossbow         },
    {  "ItemFish", GameItem::Fish         },
    {  "ItemShoes", GameItem::Shoes         },
    {  "ItemCarcass", GameItem::Carcass         },
    {  "ItemSmokedMeat", GameItem::SmokedMeat         },
    {  "ItemSmokedFish", GameItem::SmokedFish         },
    {  "ItemFlax", GameItem::Flax         },
    {  "ItemLinenClothes", GameItem::LinenClothes         },
    {  "ItemCoal", GameItem::Coal         },
    {  "ItemClay", GameItem::Clay         },
    {  "ItemWater", GameItem::Water         },
    {  "ItemGoldOre", GameItem::GoldOre         },
    {  "ItemGoldIngot", GameItem::GoldIngot         },
    {  "ItemPottery", GameItem::Pottery         },
    {  "ItemWheatBeer", GameItem::WheatBeer         },
    {  "ItemHoney", GameItem::Honey         },
    {  "ItemBasket", GameItem::Basket         },
    {  "ItemWillow", GameItem::Willow         },
    {  "ItemHealthyCarcass", GameItem::HealthyCarcass         },
    {  "ItemUnhealthyCarcass", GameItem::UnhealthyCarcass         },
    {  "ItemSicklyCarcass", GameItem::SicklyCarcass         },
    {  "ItemFurniture", GameItem::Furniture         },
    {  "ItemTallow", GameItem::Tallow         },
    {  "ItemWax", GameItem::Wax         },
    {  "ItemSoap", GameItem::Soap         },
    {  "ItemCandle", GameItem::Candle         },
    {  "ItemSpice", GameItem::Spice         },
    {  "ItemPoop", GameItem::Poop         },
    {  "ItemCompost", GameItem::Compost         },
    {  "ItemDeceasedVillager", GameItem::DeceasedVillager         },
    {  "ItemMilk", GameItem::Milk         },
    {  "ItemCheese", GameItem::Cheese         },
    {  "ItemCow", GameItem::Cow         },
    {  "ItemLivestockToMilk", GameItem::LivestockToMilk         },
    {  "ItemSand", GameItem::Sand         },
    {  "ItemGlass", GameItem::Glass         },
    {  "ItemBarrel", GameItem::Barrel         },
    {  "ItemMedicine", GameItem::Medicine         },
    {  "ItemAnimalTrap", GameItem::AnimalTrap         },
    {  "ItemSmallCarcass", GameItem::SmallCarcass         },
    {  "ItemBoarCarcass", GameItem::BoarCarcass         }
};

GameItem parseItem(const QByteArray& v)
{
    auto i = GameItemByText.find(v);
    if (i == GameItemByText.end()) {
        return GameItem::Unknown;
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
