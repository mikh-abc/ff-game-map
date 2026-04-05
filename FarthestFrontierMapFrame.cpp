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
#include "FileDataReader.h"
#include "MemoryDataReader.h"
#include "StaticData.h"
#include "FarthestFrontierMapFrame.h"
#include "ui_FarthestFrontierMapFrame.h"
#include "MapWidget.h"
#include "SaveDialog.h"
#include "GameMapChanger.h"

namespace {

const QLatin1String SelectlocationStr("Select location on map");

QString metaDataStr(const GeneralSaveData& saveData, const TerrainData& td)
{
    static const QString BaseInfoStr("name: %1\nseed: %2\nversion: %3\nvillagers: %4\nyears: %5 (%6)\n");
    static const QString DifficultyInfoStr("---Difficulty---\nresources: %1\ndisease: %2\nwildlife: %3\nraiders: %4\npacifist: %5\n");
    static const QString TerrainInfoStr("---Terrain---\nregrown trees: %1\ntrees grouth: %2\nchopped trees: %3\nobjects: %4\n");
    QString res = BaseInfoStr.arg(QString::fromUtf8(saveData.name)).arg(saveData.seed).arg(saveData.version).arg(saveData.villagers).arg(saveData.currentYear - saveData.startYear + 1).arg(saveData.currentYear);
    res.append(DifficultyInfoStr.arg(saveData.resourcesDifficulty).arg(saveData.diseaseDifficulty).arg(saveData.wildlifeDifficulty).arg(saveData.raidersDifficulty).arg(saveData.pacifist));
    res.append(TerrainInfoStr.arg(td.regrownTreeCount).arg(td.treeGrouthCount).arg(td.choppedTreeCount).arg(td.objectsCount));
    return res;
}


void addPixmap(QColor c, QLabel* label)
{
    QPixmap icon(20, 20);
    icon.fill(c);
    label->setPixmap(icon);
}

template<class L, class T>
void updateStats(const std::unordered_map<L, QLabel*>& labels, const std::vector<T>& list)
{
    for (const auto& l : labels) {
        l.second->setText("");
    }
    std::unordered_map<L, std::pair<uint, uint>> numbers;
    for (const auto& i : list) {
        std::pair<uint, uint>& ni = numbers[i.type];
        uint amount = i.amount;
        if (amount < 99999) {
            ni.first += amount;
        }
        ++ni.second;
    }

    QLocale loc(QLocale::English);
    for (const auto& ne : numbers) {
        auto label = labels.find(ne.first);
        if (label == labels.end()) {
            continue;
        }
        label->second->setText(QString("x%1 total: %2").arg(ne.second.second).arg(loc.toString(ne.second.first)));
    }
}

float getHeightInterpolated(const std::vector<std::vector<float>>& heights, float mapx, float mapz)
{
    constexpr float cellSize = 5;

    int x1 = mapx / cellSize;
    int z1 = mapz / cellSize;
    if (x1 < 0) {
        x1 = 0;
    }

    if (z1 < 0) {
        z1 = 0;
    }
    int size = heights.size();

    if (x1 >= size) {
        x1 = size - 1;
    }

    if (z1 >= size) {
        z1 = size - 1;
    }

    int x2 = x1 + 1 < size ? x1 + 1 : x1;
    int z2 = z1 + 1 < size ? z1 + 1 : z1;
    float sx = (mapx - x1 * cellSize) / cellSize;
    float sz = (mapz - z1 * cellSize) / cellSize;
    if (sx + sz < 1.f)
    {
        float height = heights[x1][z1];
        float height2 = heights[x2][z1];
        float height3 = heights[x1][z2];
        return height + (height2 - height) * sx + (height3 - height) * sz;
    }

    float height4 = heights[x2][z1];
    float height5 = heights[x1][z2];
    float height6 = heights[x2][z2];
    return height6 + (height5 - height6) * (1.f - sx) + (height4 - height6) * (1.f - sz);
}

}

FarthestFrontierMapFrame::FarthestFrontierMapFrame(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::FarthestFrontierMapFrame)
    , buttonAnimalSpawnGroup(new QButtonGroup(this))
{
    ui->setupUi(this);
    static const QMap<QString, QString> WidgetsText = {{"mapViewWidget", "Map"}, {"villagesViewWidget", "Villages"}};

    for (int i = 0; i < ui->centralStackedWidget->count(); ++i) {
        QWidget* w = ui->centralStackedWidget->widget(i);
        QAction* action = new QAction(WidgetsText[w->objectName()], this);
        connect(action, &QAction::triggered, this, [this, i]() {
            ui->centralStackedWidget->setCurrentIndex(i);
        });
        ui->menuView->addAction(action);
    }
    connect(ui->checkBoxAnimals, &QCheckBox::stateChanged, this, &FarthestFrontierMapFrame::checkBoxStateChanged);
    connect(ui->checkBoxAnimalsSpawns, &QCheckBox::stateChanged, this, &FarthestFrontierMapFrame::checkBoxStateChanged);
    connect(ui->checkBoxBuildings, &QCheckBox::stateChanged, this, &FarthestFrontierMapFrame::checkBoxStateChanged);
    connect(ui->checkBoxEnemies, &QCheckBox::stateChanged, this, &FarthestFrontierMapFrame::checkBoxStateChanged);
    connect(ui->groupBoxMinerals, &QGroupBox::toggled, this, &FarthestFrontierMapFrame::checkBoxStateChanged);
    connect(ui->checkBoxClay, &QCheckBox::stateChanged, this, &FarthestFrontierMapFrame::checkBoxStateChanged);
    connect(ui->checkBoxSand, &QCheckBox::stateChanged, this, &FarthestFrontierMapFrame::checkBoxStateChanged);
    connect(ui->checkBoxGold, &QCheckBox::stateChanged, this, &FarthestFrontierMapFrame::checkBoxStateChanged);
    connect(ui->checkBoxIron, &QCheckBox::stateChanged, this, &FarthestFrontierMapFrame::checkBoxStateChanged);
    connect(ui->checkBoxCoal, &QCheckBox::stateChanged, this, &FarthestFrontierMapFrame::checkBoxStateChanged);
    connect(ui->checkBoxStone, &QCheckBox::stateChanged, this, &FarthestFrontierMapFrame::checkBoxStateChanged);
    connect(ui->groupBoxForageables, &QGroupBox::toggled, this, &FarthestFrontierMapFrame::checkBoxStateChanged);
    connect(ui->checkBoxGreens, &QCheckBox::stateChanged, this, &FarthestFrontierMapFrame::checkBoxStateChanged);
    connect(ui->checkBoxHerbs, &QCheckBox::stateChanged, this, &FarthestFrontierMapFrame::checkBoxStateChanged);
    connect(ui->checkBoxRoots, &QCheckBox::stateChanged, this, &FarthestFrontierMapFrame::checkBoxStateChanged);
    connect(ui->checkBoxWillow, &QCheckBox::stateChanged, this, &FarthestFrontierMapFrame::checkBoxStateChanged);
    connect(ui->checkBoxHeightMap, &QCheckBox::stateChanged, this, &FarthestFrontierMapFrame::checkBoxStateChanged);
    connect(ui->spinBoxHeightMapMin, &QDoubleSpinBox::valueChanged, this, &FarthestFrontierMapFrame::checkBoxStateChanged);
    connect(ui->spinBoxHeightMapMax, &QDoubleSpinBox::valueChanged, this, &FarthestFrontierMapFrame::checkBoxStateChanged);
    connect(ui->checkBoxFertility, &QCheckBox::stateChanged, this, &FarthestFrontierMapFrame::checkBoxStateChanged);
    connect(ui->checkBoxFodder, &QCheckBox::stateChanged, this, &FarthestFrontierMapFrame::checkBoxStateChanged);
    connect(ui->checkBoxWater, &QCheckBox::stateChanged, this, &FarthestFrontierMapFrame::checkBoxStateChanged);
    connect(ui->sliderFertility, &QSlider::valueChanged, this, &FarthestFrontierMapFrame::checkBoxStateChanged);
    connect(ui->sliderFodder, &QSlider::valueChanged, this, &FarthestFrontierMapFrame::checkBoxStateChanged);
    connect(ui->sliderWater, &QSlider::valueChanged, this, &FarthestFrontierMapFrame::checkBoxStateChanged);

    QObject::connect(ui->sliderFertility, &QSlider::sliderMoved, [&](int value) {
         QToolTip::showText(QCursor::pos(), QString("%1").arg(value), nullptr);
       });
    QObject::connect(ui->sliderFodder, &QSlider::sliderMoved, [&](int value) {
         QToolTip::showText(QCursor::pos(), QString("%1").arg(value), nullptr);
       });
    QObject::connect(ui->sliderWater, &QSlider::sliderMoved, [&](int value) {
         QToolTip::showText(QCursor::pos(), QString("%1").arg(value), nullptr);
       });

    mineralsLabels_[MineralType::Clay] = ui->labelStatsClay;
    mineralsLabels_[MineralType::Sand] = ui->labelStatsSand;
    mineralsLabels_[MineralType::Coal] = ui->labelStatsCoal;
    mineralsLabels_[MineralType::Iron] = ui->labelStatsIron;
    mineralsLabels_[MineralType::Gold] = ui->labelStatsGold;

    itemLabels_[GameItem::Greens] = ui->labelStatsGreens;
    itemLabels_[GameItem::Herbs] = ui->labelStatsHerbs;
    itemLabels_[GameItem::Roots] = ui->labelStatsRoots;
    itemLabels_[GameItem::Willow] = ui->labelStatsWillow;

    addPixmap(mineralColor(MineralType::Clay), ui->labelIconClay);
    addPixmap(mineralColor(MineralType::Sand), ui->labelIconSand);
    addPixmap(mineralColor(MineralType::Coal), ui->labelIconCoal);
    addPixmap(mineralColor(MineralType::Iron), ui->labelIconIron);
    addPixmap(mineralColor(MineralType::Gold), ui->labelIconGold);
    addPixmap(mineralColor(MineralType::Stone), ui->labelIconStone);

    addPixmap(itemColor(GameItem::Greens), ui->labelIconGreens);
    addPixmap(itemColor(GameItem::Herbs), ui->labelIconHerbs);
    addPixmap(itemColor(GameItem::Roots), ui->labelIconRoots);
    addPixmap(itemColor(GameItem::Willow), ui->labelIconWillow);

    auto list = QStandardPaths::standardLocations(QStandardPaths::HomeLocation);
    if (list.size() > 0) {
        QDir dir = QDir(list[0]);
        dir.cd("Saved Games");
        dir.cd("Farthest Frontier");
        dir.cd("Save");
        saveDirectory_ = dir.path();
    }

    connect(&gameSocket_, &QLocalSocket::connected, this, &FarthestFrontierMapFrame::connectedToClient);
    connect(&gameSocket_, &QLocalSocket::readyRead, this, &FarthestFrontierMapFrame::readGameData);
    connect(&gameSocket_, &QLocalSocket::errorOccurred, this, &FarthestFrontierMapFrame::errorGameData);

    ui->viewVillages->sortByColumn(0, Qt::AscendingOrder);

    buttonAnimalSpawnGroup->addButton(ui->toolButtonAnimalSpawnsMoveUp, 0);
    buttonAnimalSpawnGroup->addButton(ui->toolButtonAnimalSpawnsMoveRight, 1);
    buttonAnimalSpawnGroup->addButton(ui->toolButtonAnimalSpawnsMoveDown, 2);
    buttonAnimalSpawnGroup->addButton(ui->toolButtonAnimalSpawnsMoveLeft, 3);
}

FarthestFrontierMapFrame::~FarthestFrontierMapFrame()
{
    delete ui;
}


void FarthestFrontierMapFrame::on_actionOpenMap_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, windowTitle(), saveDirectory_, "Farthest Frontier Maps (*.map)");
    if (fileName.isEmpty())
        return;
}


void FarthestFrontierMapFrame::on_actionOpenSav_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, windowTitle(), saveDirectory_, "Farthest Frontier Saves (*.sav)");
    openSav(fileName);
}

void FarthestFrontierMapFrame::on_actionOpenLastSav_triggered()
{
    QDirIterator it(saveDirectory_, QStringList() << "*.sav", QDir::Files, QDirIterator::Subdirectories);
    QString last;
    QDateTime lastDate = QDateTime::fromSecsSinceEpoch(0);
    while (it.hasNext()) {
        QString filename = it.next();
        QFileInfo info(filename);
        if (info.lastModified() > lastDate) {
            lastDate = info.lastModified();
            last = filename;
        }
    }
    openSav(last);
}

void FarthestFrontierMapFrame::openSav(const QString& fileName)
{
    if (fileName.isEmpty())
        return;
    fileData_.reset(new FileDataReader());
    if (!fileData_->loadSave(fileName)) {
        QMessageBox::critical(this, windowTitle(), "Can't open file");
        return;
    }

    updateUiStateChanged(true);

    cachedSaveData_ = fileData_->generalSaveData();
    if (cachedSaveData_.version.compare("v1.1.0") < 0) {
        QMessageBox::critical(this, windowTitle(), QString("Incompatible version: %1").arg(cachedSaveData_.version));
        return;
    }
    ui->viewVillages->setModel(nullptr);
    villagerModel_.reset(new VillagerModel(fileData_->villagers(), cachedSaveData_.startYear));
    ui->viewVillages->setModel(villagerModel_.get());
    pendingData_.spawns = fileData_->animalsSpawns();
    TerrainData td = fileData_->terrainData();
    updateHeights(td.heights);
    updateStats(mineralsLabels_, fileData_->minerals());
    updateStats(itemLabels_, fileData_->forageables());
    ui->textEdit->setPlainText(metaDataStr(cachedSaveData_, td));
    drawMapFromUi(fileData_);
}

void FarthestFrontierMapFrame::checkBoxStateChanged()
{
    if (!fileData_.isNull()) {
        drawMapFromUi(fileData_);
    }
    else if (!memoryData_.isNull()) {
        drawMapFromUi(memoryData_);
    }
}

void FarthestFrontierMapFrame::drawMapFromUi(const QSharedPointer<DataReader>& data)
{
    MapWidget::DrawOptions opt;
    if (ui->groupBoxMinerals->isChecked()) {
        opt.clay = ui->checkBoxClay->isChecked();
        opt.sand = ui->checkBoxSand->isChecked();
        opt.coal = ui->checkBoxCoal->isChecked();
        opt.iron = ui->checkBoxIron->isChecked();
        opt.gold = ui->checkBoxGold->isChecked();
        opt.stone = ui->checkBoxStone->isChecked();
    }
    if (ui->groupBoxForageables->isChecked()) {
        opt.greens = ui->checkBoxGreens->isChecked();
        opt.herbs = ui->checkBoxHerbs->isChecked();
        opt.roots = ui->checkBoxRoots->isChecked();
        opt.willow = ui->checkBoxWillow->isChecked();
    }
    if (ui->checkBoxHeightMap->isChecked()) {
        opt.heightMap = ui->checkBoxHeightMap->isChecked();
        opt.heightMin = ui->spinBoxHeightMapMin->value();
        opt.heightMax = ui->spinBoxHeightMapMax->value();
    }
    if (ui->checkBoxFertility->isChecked()) {
        opt.fertility = ui->sliderFertility->value();
    }
    if (ui->checkBoxFodder->isChecked()) {
        opt.fodder = ui->sliderFodder->value();
    }
    if (ui->checkBoxWater->isChecked()) {
        opt.water = ui->sliderWater->value();
    }
    opt.enemies = ui->checkBoxEnemies->isChecked();
    opt.animals = ui->checkBoxAnimals->isChecked();
    opt.animalsSpawns = ui->checkBoxAnimalsSpawns->isChecked();
    opt.buildings = ui->checkBoxBuildings->isChecked();
    ui->mapWidget->update(opt, data);
}

void FarthestFrontierMapFrame::updateUiStateChanged(bool available)
{
    ui->actionSaveSav->setEnabled(available);
    ui->actionCloseSav->setEnabled(available);
    ui->toolButtonAddClay->setEnabled(available);
    ui->toolButtonAddSand->setEnabled(available);
    ui->toolButtonAddIron->setEnabled(available);
    ui->toolButtonAddCoal->setEnabled(available);
    ui->toolButtonAddGold->setEnabled(available);
    ui->toolButtonAddStone->setEnabled(available);
    ui->toolButtonAddAnimalsSpawns->setEnabled(available);
    ui->toolButtonRemoveAnimalsSpawns->setEnabled(available);
    pendingData_.newMinerals.clear();
    ui->mapWidget->resetHighlight();
    ui->stackedWidgetInfoOptions->setCurrentWidget(ui->pageInfoViewOptions);
}

void FarthestFrontierMapFrame::startAddingMineral(MineralType type, bool deepOnly)
{
    ui->mapWidget->startHighlightMouse(MapWidget::HighlightType::Circle);
    ui->labelAddOptionsTop->setText(mineralStr(type));
    ui->labelAddOptionsLocation->setText(SelectlocationStr);
    auto& e = pendingData_.newMinerals.emplace_back(MineralData());
    e.type = type;
    if (deepOnly) {
        ui->spinBoxAddOptionsAmount->setValue(99999);
        ui->spinBoxAddOptionsAmount->setDisabled(true);
        ui->checkBoxAddOptionsDeep->setCheckState(Qt::Checked);
        ui->checkBoxAddOptionsDeep->setDisabled(true);
    } else {
        ui->spinBoxAddOptionsAmount->setValue(5000);
        ui->spinBoxAddOptionsAmount->setEnabled(true);
        ui->checkBoxAddOptionsDeep->setCheckState(Qt::Unchecked);
        ui->checkBoxAddOptionsDeep->setEnabled(true);
    }
    ui->pushButtonMineralsAdd->setDisabled(true);
    ui->stackedWidgetInfoOptions->setCurrentWidget(ui->pageInfoAddMineralsOptions);
}

void FarthestFrontierMapFrame::sendFetchDataPacket(const std::vector<uint32_t>& tags)
{
    uint32_t packetSize = sizeof(uint32_t) + sizeof(uint8_t) + sizeof(uint8_t) + (1 + tags.size()) * sizeof(uint32_t);
    uint8_t packetId = 0;
    uint8_t tagsCount = 1 + tags.size();
    gameSocket_.write(reinterpret_cast<char*>(&packetSize), sizeof(packetSize));
    gameSocket_.write(reinterpret_cast<char*>(&packetId), sizeof(packetId));
    gameSocket_.write(reinterpret_cast<char*>(&tagsCount), sizeof(tagsCount));
    gameSocket_.write(reinterpret_cast<const char*>(&ModTag), sizeof(ModTag));
    for (uint tag : tags) {
        gameSocket_.write(reinterpret_cast<char*>(&tag), sizeof(tag));
    }
}

void FarthestFrontierMapFrame::sendChangeForagerRangePacket(float foragerRange)
{
    uint32_t packetSize = sizeof(uint32_t) + sizeof(uint8_t) + sizeof(float);
    uint8_t packetId = 1;
    gameSocket_.write(reinterpret_cast<char*>(&packetSize), sizeof(packetSize));
    gameSocket_.write(reinterpret_cast<char*>(&packetId), sizeof(packetId));
    gameSocket_.write(reinterpret_cast<const char*>(&foragerRange), sizeof(foragerRange));
}

void FarthestFrontierMapFrame::sendVeterancyKillsForEveryonePacket(bool enable)
{
    uint32_t packetSize = sizeof(uint32_t) + sizeof(uint8_t) + sizeof(uint8_t);
    uint8_t packetId = 2;
    uint8_t v = enable;
    gameSocket_.write(reinterpret_cast<char*>(&packetSize), sizeof(packetSize));
    gameSocket_.write(reinterpret_cast<char*>(&packetId), sizeof(packetId));
    gameSocket_.write(reinterpret_cast<char*>(&v), sizeof(v));
}

void FarthestFrontierMapFrame::sendRemoveScaredAnimalsPacket(const std::vector<uint16_t>& spawnAreas)
{
    uint32_t packetSize = sizeof(uint32_t) + sizeof(uint8_t) + sizeof(uint8_t) + spawnAreas.size() * sizeof(uint16_t);
    uint8_t packetId = 3;
    gameSocket_.write(reinterpret_cast<char*>(&packetSize), sizeof(packetSize));
    gameSocket_.write(reinterpret_cast<char*>(&packetId), sizeof(packetId));
    uint8_t s = spawnAreas.size();
    gameSocket_.write(reinterpret_cast<const char*>(&s), sizeof(s));
    for(uint16_t area : spawnAreas) {
        gameSocket_.write(reinterpret_cast<const char*>(&area), sizeof(area));
    }
}

void FarthestFrontierMapFrame::sendChangeFlattenTerrainRatioPacket(int32_t ratio)
{
    uint32_t packetSize = sizeof(uint32_t) + sizeof(uint8_t) + sizeof(uint32_t);
    uint8_t packetId = 4;
    gameSocket_.write(reinterpret_cast<char*>(&packetSize), sizeof(packetSize));
    gameSocket_.write(reinterpret_cast<char*>(&packetId), sizeof(packetId));
    gameSocket_.write(reinterpret_cast<const char*>(&ratio), sizeof(ratio));
}

void FarthestFrontierMapFrame::on_actionSaveSav_triggered()
{
    SaveDialog* dialog = new SaveDialog(this);
    if (!pendingData_.newMinerals.empty()) {
        dialog->addInfo(QString("%1 Mineral(s) will be added").arg(pendingData_.newMinerals.size()));
    }
    if (pendingData_.changedSpawns) {
        for (auto i = pendingData_.spawns.begin(); i != pendingData_.spawns.end();) {
            if (!i->second.uuid.isEmpty()) {
                ++i;
                continue;
            }
            auto uuid = convertAnimalsSpawnTypeToUUID(cachedSaveData_.pacifist == 1 ? 0 : cachedSaveData_.wildlifeDifficulty, i->second.type);
            if (uuid.isEmpty()) {
                i = pendingData_.spawns.erase(i);
            } else {
                i->second.uuid = uuid;
                ++i;
            }
        }
        dialog->addInfo(tr("Animals spawns will be changed"));
    }
    connect(dialog, &QDialog::finished, this, [dialog, this](int result) {
        if (result != QDialog::Accepted) {
            return;
        }
        QString fileName = QFileDialog::getSaveFileName(this, windowTitle(), saveDirectory_, "Farthest Frontier Saves (*.sav)");
        if (fileName.isEmpty())
            return;
        GameMapChanger changer(dialog->options());
        if (!changer.copy(fileData_->saveFileName(), fileName, pendingData_)) {
            QMessageBox::critical(this, windowTitle(), "Can't open file");
            return;
        }
        pendingData_.newMinerals.clear();
        pendingData_.changedSpawns = false;
    });
    connect(dialog, &QDialog::finished, dialog, &QDialog::deleteLater);
    dialog->open();
}


void FarthestFrontierMapFrame::on_actionCloseSav_triggered()
{
    updateUiStateChanged(false);
    fileData_->closeSave();
    ui->textEdit->setPlainText("");
    ui->mapWidget->clear();
    pendingData_.newMinerals.clear();
    pendingData_.changedSpawns = false;
}

void FarthestFrontierMapFrame::on_toolButtonAddSand_clicked()
{
    startAddingMineral(MineralType::Sand);
}


void FarthestFrontierMapFrame::on_toolButtonAddClay_clicked()
{
    startAddingMineral(MineralType::Clay);
}


void FarthestFrontierMapFrame::on_toolButtonAddCoal_clicked()
{
    startAddingMineral(MineralType::Coal);
}


void FarthestFrontierMapFrame::on_toolButtonAddIron_clicked()
{
    startAddingMineral(MineralType::Iron);
}


void FarthestFrontierMapFrame::on_toolButtonAddGold_clicked()
{
    startAddingMineral(MineralType::Gold);
}

void FarthestFrontierMapFrame::on_toolButtonAddStone_clicked()
{
    startAddingMineral(MineralType::Stone, true);
}

void FarthestFrontierMapFrame::on_toolButtonAddAnimalsSpawns_clicked()
{
    ui->labelAnimalSpawnInfo->setText(SelectlocationStr);
    ui->labelSpawnAnimal->setVisible(true);
    ui->comboBoxAnimalSpawnType->setVisible(true);
    ui->widgetAnimalSpawnsMove->setVisible(false);
    ui->comboBoxAnimalSpawnType->setCurrentIndex(0);
    ui->mapWidget->startHighlightMouse(MapWidget::HighlightType::Rect);
    ui->stackedWidgetInfoOptions->setCurrentWidget(ui->pageInfoAnimalSpawnsOptions);
}

void FarthestFrontierMapFrame::on_toolButtonRemoveAnimalsSpawns_clicked()
{
    ui->labelAnimalSpawnInfo->setText(SelectlocationStr);
    ui->labelSpawnAnimal->setVisible(false);
    ui->comboBoxAnimalSpawnType->setVisible(false);
    ui->comboBoxAnimalSpawnType->setCurrentIndex(-1);
    ui->widgetAnimalSpawnsMove->setVisible(false);
    ui->mapWidget->startHighlightMouse(MapWidget::HighlightType::Rect);
    ui->stackedWidgetInfoOptions->setCurrentWidget(ui->pageInfoAnimalSpawnsOptions);
}

void FarthestFrontierMapFrame::on_pushButtonMineralsCancel_clicked()
{
    ui->mapWidget->cancelHighlight();
    ui->stackedWidgetInfoOptions->setCurrentWidget(ui->pageInfoViewOptions);
    pendingData_.newMinerals.pop_back();
}

void FarthestFrontierMapFrame::on_pushButtonSpawnCancel_clicked()
{
    ui->mapWidget->cancelHighlight();
    ui->stackedWidgetInfoOptions->setCurrentWidget(ui->pageInfoViewOptions);
}

void FarthestFrontierMapFrame::on_mapWidget_clicked(const QPointF& position)
{
    if (ui->stackedWidgetInfoOptions->currentWidget() == ui->pageInfoAddMineralsOptions) {
        ui->mapWidget->finishHighlightMouse();
        MineralData& mdata = pendingData_.newMinerals.back();
        Point& p = mdata.p;

        //uint hx = position.x() / 5;
        //uint hz = position.y() / 5;
        p.x = position.x(); //hx * 5;
        p.z = position.y(); //hz * 5;
        p.y = getHeightInterpolated(cachedHeights_, position.y(), position.x()); //cachedHeights_[hz][hx];

        ui->mapWidget->addHighlight(QPoint(p.x, p.z),  QColor(), mineralColor(mdata.type));
        ui->labelAddOptionsLocation->setText(QString("(%1, %2, %3)").arg(p.x).arg(p.y).arg(p.z));
        ui->pushButtonMineralsAdd->setEnabled(true);
    } else if (ui->stackedWidgetInfoOptions->currentWidget() == ui->pageInfoAnimalSpawnsOptions) {
        uint32_t spawnAreaX = position.x() / AnimalSpawnAreaSize;
        uint32_t spawnAreaY = position.y() / AnimalSpawnAreaSize;
        uint32_t SpawnAreaLineSize = cachedHeights_.size() * CellSize / AnimalSpawnAreaSize;
        uint32_t spawnArea = spawnAreaX + spawnAreaY * SpawnAreaLineSize;
        if (!memoryData_.isNull()) {
            std::vector<uint16_t> indexes(1, spawnArea);
            switch (buttonAnimalSpawnGroup->checkedId()) {
            case 0:
                if (spawnAreaY + 1 < SpawnAreaLineSize) {
                    indexes.push_back(spawnAreaX + (spawnAreaY + 1) * SpawnAreaLineSize);
                }
                if (spawnAreaX > 1) {
                    indexes.push_back(spawnAreaX - 1 + spawnAreaY * SpawnAreaLineSize);
                }
                if (spawnAreaX + 1 < SpawnAreaLineSize) {
                    indexes.push_back(spawnAreaX + 1 + spawnAreaY * SpawnAreaLineSize);
                }
                break;
            case 1:
                if (spawnAreaY + 1 < SpawnAreaLineSize) {
                    indexes.push_back(spawnAreaX + (spawnAreaY + 1) * SpawnAreaLineSize);
                }
                if (spawnAreaY > 1) {
                    indexes.push_back(spawnAreaX + (spawnAreaY - 1) * SpawnAreaLineSize);
                }
                if (spawnAreaX + 1 < SpawnAreaLineSize) {
                    indexes.push_back(spawnAreaX + 1 + spawnAreaY * SpawnAreaLineSize);
                }
                break;
            case 2:
                if (spawnAreaY > 1) {
                    indexes.push_back(spawnAreaX + (spawnAreaY - 1) * SpawnAreaLineSize);
                }
                if (spawnAreaX > 1) {
                    indexes.push_back(spawnAreaX - 1 + spawnAreaY * SpawnAreaLineSize);
                }
                if (spawnAreaX + 1 < SpawnAreaLineSize) {
                    indexes.push_back(spawnAreaX + 1 + spawnAreaY * SpawnAreaLineSize);
                }
                break;
            case 3:
                if (spawnAreaY + 1 < SpawnAreaLineSize) {
                    indexes.push_back(spawnAreaX + (spawnAreaY + 1) * SpawnAreaLineSize);
                }
                if (spawnAreaY > 1) {
                    indexes.push_back(spawnAreaX + (spawnAreaY - 1) * SpawnAreaLineSize);
                }
                if (spawnAreaX > 1) {
                    indexes.push_back(spawnAreaX - 1 + spawnAreaY * SpawnAreaLineSize);
                }
                break;
            }
            sendRemoveScaredAnimalsPacket(indexes);
        } else {
            BaseType animal = BaseType::Unknown;
            bool remove = false;
            switch (ui->comboBoxAnimalSpawnType->currentIndex()) {
            case -1:
                remove = true;
                break;
            case 0:
                animal = BaseType::Deer;
                break;
            case 1:
                animal = BaseType::Boar;
                break;
            case 2:
                animal = BaseType::Wolf;
                break;
            case 3:
                animal = BaseType::Bear;
                break;
            }
            if (remove) {
                auto i = pendingData_.spawns.find(spawnArea);
                if (i != pendingData_.spawns.end()) {
                    pendingData_.changedSpawns = true;
                    pendingData_.spawns.erase(i);
                    ui->mapWidget->addHighlight(QPoint((spawnAreaX + 1) * AnimalSpawnAreaSize, spawnAreaY * AnimalSpawnAreaSize),  QColor(192, 192, 192), QColor(192, 192, 192, 128));
                }
            } else {
                pendingData_.changedSpawns = true;
                pendingData_.spawns.insert({spawnArea, AnimalSpawnData{animal, QByteArray()}});
                auto c = animalSpawnColor(animal);
                ui->mapWidget->addHighlight(QPoint((spawnAreaX + 1) * AnimalSpawnAreaSize, spawnAreaY * AnimalSpawnAreaSize),  c.first, c.second);
            }
        }
        ui->mapWidget->finishHighlightMouse();
        ui->stackedWidgetInfoOptions->setCurrentWidget(ui->pageInfoViewOptions);
    }
}

void FarthestFrontierMapFrame::on_pushButtonMineralsAdd_clicked()
{
    MineralData& d = pendingData_.newMinerals.back();
    d.amount = ui->spinBoxAddOptionsAmount->value();
    d.r = ui->spinBoxAddOptionsRadius->value();
    d.deep = ui->checkBoxAddOptionsDeep->isChecked();
    ui->stackedWidgetInfoOptions->setCurrentWidget(ui->pageInfoViewOptions);
}

void FarthestFrontierMapFrame::updateHeights(const std::vector<std::vector<float>>& heightMap)
{
    cachedHeights_ = heightMap;
    if (cachedHeights_.empty()) {
        return;
    }
    float min = 1000.0;
    float max = -1000.0;
    size_t limit = cachedHeights_.size() / 10;
    size_t width = cachedHeights_.size();
    size_t height = cachedHeights_[0].size();

    for (size_t x = limit; x < width - limit; ++x) {
        for (size_t y = limit; y < height - limit; ++y) {
            float v = cachedHeights_[x][y];
            if (v > max) {
                max = v;
            } else if (v < min) {
                min = v;
            }
        }
    }
    ui->spinBoxHeightMapMin->blockSignals(true);
    ui->spinBoxHeightMapMin->setValue(min);
    ui->spinBoxHeightMapMin->blockSignals(false);
    ui->spinBoxHeightMapMax->blockSignals(true);
    ui->spinBoxHeightMapMax->setValue(max);
    ui->spinBoxHeightMapMax->blockSignals(false);

}

void FarthestFrontierMapFrame::on_actionFetchDataFromClient_triggered()
{
    if (!memoryData_.isNull() && gameSocket_.state() == QLocalSocket::ConnectedState) {
        const std::vector<uint32_t>& tags = getBaseTypes();
        memoryData_->addWaitingData(tags, ModTag);
        sendFetchDataPacket(tags);
    } else {
        memoryData_.reset(new MemoryDataReader());
        gameSocket_.connectToServer("ffdata");
        fileData_.reset();
        firstUpdate_ = true;
        updateUiStateChanged(false);
    }
}

void FarthestFrontierMapFrame::on_actionCloseConnection_triggered()
{
    memoryData_.reset(new MemoryDataReader());
    gameSocket_.close();
    updateUiStateChanged(false);
    ui->textEdit->setPlainText("");
    ui->mapWidget->clear();
}

void FarthestFrontierMapFrame::on_actionChangeForagerRange_triggered()
{
    bool ok = false;
    float v = QInputDialog::getDouble(this, windowTitle(), tr("Enter Forager Range"), 200.f, 60.f, 500.f, 1, &ok);
    if (ok) {
        sendChangeForagerRangePacket(v);
    }
}

void FarthestFrontierMapFrame::on_actionVeterancyKillsForEveryone_triggered(bool checked)
{
    sendVeterancyKillsForEveryonePacket(checked);
}

void FarthestFrontierMapFrame::on_actionMoveAnimalsSpawn_triggered()
{
    ui->labelAnimalSpawnInfo->setText(SelectlocationStr);
    ui->labelSpawnAnimal->setVisible(false);
    ui->comboBoxAnimalSpawnType->setVisible(false);
    ui->comboBoxAnimalSpawnType->setCurrentIndex(-1);
    ui->widgetAnimalSpawnsMove->setVisible(true);
    ui->mapWidget->startHighlightMouse(MapWidget::HighlightType::Rect);
    ui->stackedWidgetInfoOptions->setCurrentWidget(ui->pageInfoAnimalSpawnsOptions);
}

void FarthestFrontierMapFrame::on_actionChangeFlattenTerrainRatio_triggered()
{
    bool ok = false;
    float v = QInputDialog::getInt(this, windowTitle(), tr("Enter Flatten Terrain Ratio"), 5, 1, 100, 1, &ok);
    if (ok) {
        sendChangeFlattenTerrainRatioPacket(v);
    }
}

void FarthestFrontierMapFrame::on_actionHeightmapExport_triggered()
{
    float min = ui->spinBoxHeightMapMin->value();
    float max = ui->spinBoxHeightMapMax->value();
    if (cachedHeights_.empty() || min >= max) {
        return;
    }
    QString fileName = QFileDialog::getSaveFileName(this, windowTitle(), saveDirectory_, "Image (*.png)");
    if (fileName.isEmpty())
        return;
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
        return;
    int pixWidth = cachedHeights_.size();
    int pixHeight = cachedHeights_[0].size();
    QPixmap image(pixWidth, pixHeight);
    QPainter p(&image);
    for (int x = 0; x < pixWidth; ++x) {
        for (int y = 0; y < pixHeight; ++y) {
            float v = (cachedHeights_[x][y] - min) / (max - min);
            QColor color = QColor::fromRgbF(v, v, v, 1);
            p.setPen(color);
            p.setBrush(color);
            p.drawPoint(pixHeight - y, x);
        }
    }
    image.save(&file, "PNG");
    file.close();
}

void FarthestFrontierMapFrame::on_actionHeightmapImport_triggered()
{

}

void FarthestFrontierMapFrame::readGameData()
{
    static const QString RaidTitleInfoStr("---Raid---\n");
    static const QString RaidInfoLimitStr("limit: %1 / %2\n");
    static const QString RaidInfoMonthsStr("months passed: %1/%2\n");
    static const QString RaidInfoResumeStr("months until resume: %3\n");
    static const QString RaidInfoSeasonStr("till next seasson: %1\n");
    static const QString RaidInfoDatesStr("previous raids: ");

    if (memoryData_->appendData(gameSocket_.readAll())) {
        updateStats(mineralsLabels_, memoryData_->minerals());
        updateStats(itemLabels_, memoryData_->forageables());
        cachedSaveData_ = memoryData_->generalSaveData();
        TerrainData td = memoryData_->terrainData();
        pendingData_.spawns = memoryData_->animalsSpawns();
        villagerModel_.reset(new VillagerModel(memoryData_->villagers(), cachedSaveData_.startYear));

        ui->viewVillages->setModel(villagerModel_.get());
        updateHeights(td.heights);
        firstUpdate_ = false;
        ui->textEdit->setPlainText(metaDataStr(cachedSaveData_, td));

        ModData modData = memoryData_->modData();
        QString raidStr = RaidTitleInfoStr;
        raidStr.append(RaidInfoLimitStr.arg(modData.previousRaidDates.size()).arg(cachedSaveData_.numRaidsPerRaidCountLimit));
        raidStr.append(RaidInfoMonthsStr.arg(modData.monthsPassed).arg(modData.monthsToNextRaid));
        if (modData.monthsUntilRaidAttacksCanResume > 0) {
            raidStr.append(RaidInfoResumeStr.arg(modData.monthsUntilRaidAttacksCanResume));
        }
        if (modData.monthsPassed >= modData.monthsToNextRaid) {
            int left = 0;
            if (cachedSaveData_.currentMonth > cachedSaveData_.endingMonthInclusive) {
                left = 12 - cachedSaveData_.currentMonth + cachedSaveData_.startingMonthInclusive;
            } else if (cachedSaveData_.currentMonth < cachedSaveData_.startingMonthInclusive) {
                left = cachedSaveData_.startingMonthInclusive - cachedSaveData_.currentMonth;
            }
            raidStr.append(RaidInfoSeasonStr.arg(left));
        }
        if (!modData.previousRaidDates.empty()) {
            std::vector<std::pair<int32_t, int32_t>>::const_iterator i = modData.previousRaidDates.begin();
            raidStr.append(RaidInfoDatesStr);
            raidStr.append(QString("%1.%2").arg(i->first, 2, 10, QChar(u'0')).arg(i->second));
            ++i;
            for (; i != modData.previousRaidDates.end(); ++i) {
                raidStr.append(QString(", %1.%2").arg(i->first, 2, 10, QChar(u'0')).arg(i->second));
            }
        }
        ui->textEdit->appendPlainText(raidStr);
        drawMapFromUi(memoryData_);
    }
}

void FarthestFrontierMapFrame::connectedToClient()
{
    ui->textEdit->setPlainText("Connected to Game Client.");
    const std::vector<uint32_t>& tags = getBaseTypes();
    memoryData_->addWaitingData(tags, ModTag);
    sendFetchDataPacket(tags);
}

void FarthestFrontierMapFrame::errorGameData(QLocalSocket::LocalSocketError socketError)
{
    switch (socketError) {
    case QLocalSocket::ServerNotFoundError:
        QMessageBox::information(this, windowTitle(),
                                 tr("The game mod was not found. Please make sure that the game is running and the mod is installed."));
        break;
    case QLocalSocket::ConnectionRefusedError:
        QMessageBox::information(this, windowTitle(),
                                 tr("The connection was refused by the peer. Make sure the game is running, and the mod is installed."));
        break;
    case QLocalSocket::PeerClosedError:
        break;
    default:
        QMessageBox::information(this, windowTitle(), tr("The following error occurred: %1.").arg(gameSocket_.errorString()));
    }
}
