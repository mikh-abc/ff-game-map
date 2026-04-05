// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software distributed under the License
// is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
// or implied.  See the License for the specific language governing permissions and limitations
// under the License.

#ifndef FILEDATAREADER_H
#define FILEDATAREADER_H

#include "DataDefines.h"
#include "DataReader.h"

class FileDataReader : public DataReader
{
public:
    void loadMap(const QString& path);

    bool loadSave(const QString& path);
    void closeSave();
    QString saveFileName() const;

    virtual std::vector<MineralData> minerals();
    virtual std::vector<ForageableData> forageables();
    virtual std::vector<RaiderData> raiders();
    virtual std::vector<BaseData> animals();
    virtual std::unordered_map<uint32_t, BaseData> sites();
    virtual std::unordered_map<uint32_t, AnimalSpawnData> animalsSpawns();
    virtual std::vector<AnimalData> deers();
    virtual GeneralSaveData generalSaveData();
    virtual AgricultureInfo::Data agricultureData();
    virtual TerrainData terrainData();
    virtual std::vector<VillagerData> villagers();
    virtual Point camera();

signals:

protected:
    bool seekDataType(QDataStream& in, BaseType baseType, uint index = 0);

    QString savePath_;
    QFile saveFile_;
    QHash<BaseType, QVector<qint64>> table_;
};

#endif // FILEDATAREADER_H
