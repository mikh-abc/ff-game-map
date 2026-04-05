// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software distributed under the License
// is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
// or implied.  See the License for the specific language governing permissions and limitations
// under the License.

#ifndef FARTHESTFRONTIERMAPFRAME_H
#define FARTHESTFRONTIERMAPFRAME_H

#include <QMainWindow>
#include <QScopedPointer>
#include <QLocalSocket>

#include "DataDefines.h"
#include "GameMapChanger.h"
#include "VillagerModel.h"

class MemoryDataReader;
class FileDataReader;
class DataReader;

QT_BEGIN_NAMESPACE
namespace Ui { class FarthestFrontierMapFrame; }
QT_END_NAMESPACE

class FarthestFrontierMapFrame : public QMainWindow
{
    Q_OBJECT

public:
    FarthestFrontierMapFrame(QWidget* parent = nullptr);
    ~FarthestFrontierMapFrame();

private slots:
    void checkBoxStateChanged();
    void readGameData();
    void connectedToClient();
    void errorGameData(QLocalSocket::LocalSocketError socketError);

    void on_actionOpenMap_triggered();
    void on_actionOpenSav_triggered();
    void on_actionOpenLastSav_triggered();
    void on_actionSaveSav_triggered();
    void on_actionCloseSav_triggered();
    void on_toolButtonAddSand_clicked();
    void on_toolButtonAddClay_clicked();
    void on_toolButtonAddCoal_clicked();
    void on_toolButtonAddIron_clicked();
    void on_toolButtonAddGold_clicked();
    void on_toolButtonAddStone_clicked();
    void on_toolButtonAddAnimalsSpawns_clicked();
    void on_toolButtonRemoveAnimalsSpawns_clicked();
    void on_pushButtonMineralsAdd_clicked();
    void on_pushButtonMineralsCancel_clicked();
    void on_pushButtonSpawnCancel_clicked();
    void on_mapWidget_clicked(const QPointF& position);
    void on_actionHeightmapExport_triggered();
    void on_actionHeightmapImport_triggered();
    void on_actionCloseConnection_triggered();
    void on_actionFetchDataFromClient_triggered();
    void on_actionChangeForagerRange_triggered();
    void on_actionVeterancyKillsForEveryone_triggered(bool checked);
    void on_actionMoveAnimalsSpawn_triggered();
    void on_actionChangeFlattenTerrainRatio_triggered();

private:
    void openSav(const QString& fileName);
    void drawMapFromUi(const QSharedPointer<DataReader> &data);
    void updateUiStateChanged(bool available);
    void updateHeights(const std::vector<std::vector<float>>& heightMap);
    void startAddingMineral(MineralType type, bool deepOnly = false);

    void sendFetchDataPacket(const std::vector<uint32_t>& tags);
    void sendChangeForagerRangePacket(float foragerRange);
    void sendVeterancyKillsForEveryonePacket(bool enable);
    void sendRemoveScaredAnimalsPacket(const std::vector<uint16_t>& spawnAreas);
    void sendChangeFlattenTerrainRatioPacket(int32_t ratio);

    PendingData pendingData_;

    Ui::FarthestFrontierMapFrame* ui;
    QButtonGroup* buttonAnimalSpawnGroup;
    QScopedPointer<VillagerModel> villagerModel_;

    QSharedPointer<FileDataReader> fileData_;
    QSharedPointer<MemoryDataReader> memoryData_;
    QString saveDirectory_;
    std::unordered_map<MineralType, QLabel*> mineralsLabels_;
    std::unordered_map<GameItem, QLabel*> itemLabels_;

    std::vector<std::vector<float>> cachedHeights_;
    GeneralSaveData cachedSaveData_;

    QLocalSocket gameSocket_;
    bool firstUpdate_;
};
#endif // FARTHESTFRONTIERMAPFRAME_H
