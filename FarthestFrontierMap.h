// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software distributed under the License
// is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
// or implied.  See the License for the specific language governing permissions and limitations
// under the License.

#ifndef FARTHESTFRONTIERMAP_H
#define FARTHESTFRONTIERMAP_H

#include <QObject>
#include "DataDefines.h"

class FarthestFrontierMap : public QObject
{
    Q_OBJECT
public:
    explicit FarthestFrontierMap(QObject* parent = nullptr);

    bool loadSave(const QString& path);
    bool copySave(const QString& path, bool removeFoW, bool removeBuildingSites);
    void closeSave();

    class SaveReader
    {
    public:
        explicit SaveReader(FarthestFrontierMap& map);
        ~SaveReader();

        Point camera();
        std::vector<MineralData> minerals();
        std::vector<ForageableData> forageables();
        std::vector<RaiderData> raiders();
        std::vector<BaseData> enemies();
        std::vector<BaseData> animals();
        std::vector<BaseData> houses();
        std::vector<AnimalData> deers();
        GeneralSaveData generalSaveData();
        AgricultureInfo::Data agricultureData();
    private:
        bool seekFieldSaveFile(BaseType baseType, uint index = 0);
        QHash<BaseType, QVector<qint64>>& table_;
        QFile saveFile_;
    };
    SaveReader reader();

    QPixmap landscape() const;
signals:

private:
    QString savePath_;
    QFile saveFile_;
    QHash<BaseType, QVector<qint64>> table_;
    QByteArray landscapeData_;
    QByteArray screenshotData_;

};

#endif // FARTHESTFRONTIERMAP_H
