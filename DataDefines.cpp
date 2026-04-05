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
#include "DataDefines.h"

QColor itemColor(GameItem v)
{
    switch (v)
    {
    case GameItem::Herbs:
        return Qt::darkMagenta;
    case GameItem::Willow:
        return Qt::yellow;
    case GameItem::Roots:
        return Qt::darkYellow;
    case GameItem::Greens:
        return QColor(90, 200, 90);
    default:
        return QColor();
    }
}


QColor mineralColor(MineralType v)
{
    switch (v)
    {
    case MineralType::Iron:
        return QColor(140, 140, 140);
    case MineralType::Gold:
        return QColor(127, 127, 0);
    case MineralType::Coal:
        return QColor(80, 80, 80);
    case MineralType::Clay:
        return QColor(127, 30, 30);
    case MineralType::Sand:
        return QColor(255, 255, 170);
    case MineralType::Stone:
        return QColor(200, 200, 200);
    case MineralType::FertilityBonus:
        return QColor(173, 255, 47, 128);
    case MineralType::Unknown:
        break;
    }
    return QColor();
}

std::pair<QColor, QColor> animalSpawnColor(BaseType v)
{
    switch (v)
    {
    case BaseType::Bear:
        return std::make_pair(QColor(192, 64, 255), QColor(212, 192, 255, 128));
    case BaseType::Wolf:
        return std::make_pair(QColor(255, 64, 64), QColor(255, 212, 212, 128));
    case BaseType::Deer:
        return std::make_pair(QColor(0, 192, 0), QColor(212, 255, 212, 128));
    case BaseType::Boar:
        return std::make_pair(QColor(255, 216, 0), QColor(255, 233, 128, 128));
    default:
        return std::make_pair(QColor(), QColor());
    }
}

