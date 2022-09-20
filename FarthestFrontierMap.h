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
    void loadMap(const QString& path);
    bool loadSave(const QString& path);
    bool copySave(const QString& path, bool removeFoW);
    QPixmap landscape() const;

    Point size();
    Point start();
    std::vector<MineralData> minerals();
    std::vector<ForageableData> forageables();
    std::vector<RaiderData> raiders();
    std::vector<BaseData> enemies();
    std::vector<BaseData> animals();
    std::vector<BaseData> houses();
    GeneralSaveData generalSaveData();

    QByteArray saveName() const;
    QByteArray seed() const;
    QByteArray version() const;

signals:

private:
    bool seekFieldSaveFile(quint32 id, uint index = 0);

    QFile saveFile_;
    QHash<quint32, QVector<qint64>> table_;
    QByteArray landscapeData_;
    QByteArray screenshotData_;

};

#endif // FARTHESTFRONTIERMAP_H
