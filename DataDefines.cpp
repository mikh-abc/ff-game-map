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
    }
    return QColor();
}


QColor mineralColor(MineralType v)
{
    switch (v)
    {
    case MineralType::Iron:
        return Qt::gray;
    case MineralType::Gold:
        return Qt::darkYellow;
    case MineralType::Coal:
        return QColor(64, 64, 64);
    case MineralType::Clay:
        return Qt::darkRed;
    case MineralType::Sand:
        return QColor(255, 255, 170);
    }
    return QColor();
}

