// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software distributed under the License
// is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
// or implied.  See the License for the specific language governing permissions and limitations
// under the License.

#ifndef GAMEMAPCHANGER_H
#define GAMEMAPCHANGER_H

#include <vector>

#include "DataDefines.h"

struct PendingData {
    std::vector<MineralData> newMinerals;
    std::unordered_map<uint32_t, AnimalSpawnData> spawns;
    bool changedSpawns = false;
};

class GameMapChanger
{
public:
    struct Options
    {
        bool cureVillagers = false;
        bool removeFoW = false;
        bool removeBuildingSites = false;
        int difficulty = -1;
        bool doubleMinerals = false;
        bool finishFlattenAreas = false;
        QByteArray name;
        int pacifist = 1;
    };

    explicit GameMapChanger(const Options& options);

    bool copy(const QString& from, const QString& to, const PendingData& pendingData);

signals:

private:
    Options options_;

    bool handleAnimal(QByteArray& buf, const std::unordered_map<uint32_t, AnimalSpawnData>& spawns);
    void handleAnimalManager(QByteArray& buf, const std::unordered_map<uint32_t, AnimalSpawnData>& spawns);
    void handleMineralManager(QByteArray& buf, const std::vector<MineralData>& addMinerals);
    void handleGuids(QByteArray& buf, const std::set<uint32_t>& removed);
    void handleMetaData(QByteArray& buf);
    void handleVillager(QByteArray& buf);
    void handleFlattenAreas(QByteArray& buf);
};

#endif // GAMEMAPCHANGER_H
