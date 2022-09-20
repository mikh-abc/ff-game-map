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

class FarthestFrontierMap;

QT_BEGIN_NAMESPACE
namespace Ui { class FarthestFrontierMapFrame; }
QT_END_NAMESPACE

class FarthestFrontierMapFrame : public QMainWindow
{
    Q_OBJECT

public:
    FarthestFrontierMapFrame(QWidget *parent = nullptr);
    ~FarthestFrontierMapFrame();

private slots:
    void on_actionOpenMap_triggered();
    void on_actionOpenSav_triggered();
    void on_actionSaveSav_triggered();
    void on_checkBoxEnemy_stateChanged(int);
    void on_checkBoxBuildings_stateChanged(int);
    void on_checkBoxForageables_stateChanged(int);
    void on_checkBoxMinerals_stateChanged(int);
    void on_checkBoxAnimals_stateChanged(int);
    void on_pushButtonCancel_clicked();
    void on_pushButtonSave_clicked();

private:
    void drawMap();
    void drawMap(bool minerals, bool forageables, bool animals, bool enemies, bool buildings);

    Ui::FarthestFrontierMapFrame *ui;
    QScopedPointer<FarthestFrontierMap> map_;
    QString savePath_;
};
#endif // FARTHESTFRONTIERMAPFRAME_H
