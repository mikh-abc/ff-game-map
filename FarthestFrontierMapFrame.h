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

#include "DataDefines.h"

class GameMap;

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

    void on_actionOpenSav_triggered();
    void on_actionSaveSav_triggered();
    void on_actionCloseSav_triggered();
    void on_toolButtonAddSand_clicked();
    void on_toolButtonAddClay_clicked();
    void on_toolButtonAddCoal_clicked();
    void on_toolButtonAddIron_clicked();
    void on_toolButtonAddGold_clicked();

    void on_pushButtonAddOptionsCancel_clicked();
    void on_mapWidget_clicked(const QPointF& position);

    void on_pushButtonAddOptions_clicked();

private:
    void drawMapFromUi();
    void mapStateChanged(bool available);

    void startAddingMineral(MineralType type);

    std::vector<MineralData> pendingNewMinerals;

    Ui::FarthestFrontierMapFrame *ui;
    QSharedPointer<GameMap> map_;
    QString saveDirectory_;
    std::unordered_map<MineralType, QLabel*> mineralsLabels;
    std::unordered_map<GameItem, QLabel*> itemLabels;
};
#endif // FARTHESTFRONTIERMAPFRAME_H
