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
#include "GameMap.h"
#include "FarthestFrontierMapFrame.h"
#include "ui_FarthestFrontierMapFrame.h"
#include "MapWidget.h"
#include "SaveDialog.h"
#include "GameMapChanger.h"

namespace {

const QLatin1String WindowTitle("Farthest Frontier Map");
const QLatin1String SelectlocationStr("Select location on map");


QLatin1String mineralStr(MineralType v)
{
    switch (v)
    {
    case MineralType::Iron:
        return QLatin1String("Iron");
    case MineralType::Gold:
        return QLatin1String("Gold");
    case MineralType::Coal:
        return QLatin1String("Coal");
    case MineralType::Clay:
        return QLatin1String("Clay");
    case MineralType::Sand:
        return QLatin1String("Sand");
    }
    return QLatin1String();
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
        ni.first += i.amount;
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

}

FarthestFrontierMapFrame::FarthestFrontierMapFrame(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::FarthestFrontierMapFrame)
{
    ui->setupUi(this);
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
    connect(ui->groupBoxForageables, &QGroupBox::toggled, this, &FarthestFrontierMapFrame::checkBoxStateChanged);
    connect(ui->checkBoxGreens, &QCheckBox::stateChanged, this, &FarthestFrontierMapFrame::checkBoxStateChanged);
    connect(ui->checkBoxHerbs, &QCheckBox::stateChanged, this, &FarthestFrontierMapFrame::checkBoxStateChanged);
    connect(ui->checkBoxRoots, &QCheckBox::stateChanged, this, &FarthestFrontierMapFrame::checkBoxStateChanged);
    connect(ui->checkBoxWillow, &QCheckBox::stateChanged, this, &FarthestFrontierMapFrame::checkBoxStateChanged);
    connect(ui->checkBoxFertility, &QCheckBox::stateChanged, this, &FarthestFrontierMapFrame::checkBoxStateChanged);
    connect(ui->checkBoxFooder, &QCheckBox::stateChanged, this, &FarthestFrontierMapFrame::checkBoxStateChanged);
    connect(ui->checkBoxWater, &QCheckBox::stateChanged, this, &FarthestFrontierMapFrame::checkBoxStateChanged);
    connect(ui->sliderFertility, &QSlider::valueChanged, this, &FarthestFrontierMapFrame::checkBoxStateChanged);
    connect(ui->sliderFooder, &QSlider::valueChanged, this, &FarthestFrontierMapFrame::checkBoxStateChanged);
    connect(ui->sliderWater, &QSlider::valueChanged, this, &FarthestFrontierMapFrame::checkBoxStateChanged);

    QObject::connect(ui->sliderFertility, &QSlider::sliderMoved, [&](int value) {
         QToolTip::showText(QCursor::pos(), QString("%1").arg(value), nullptr);
       });
    QObject::connect(ui->sliderFooder, &QSlider::sliderMoved, [&](int value) {
         QToolTip::showText(QCursor::pos(), QString("%1").arg(value), nullptr);
       });
    QObject::connect(ui->sliderWater, &QSlider::sliderMoved, [&](int value) {
         QToolTip::showText(QCursor::pos(), QString("%1").arg(value), nullptr);
       });


    mineralsLabels[MineralType::Clay] = ui->labelStatsClay;
    mineralsLabels[MineralType::Sand] = ui->labelStatsSand;
    mineralsLabels[MineralType::Coal] = ui->labelStatsCoal;
    mineralsLabels[MineralType::Iron] = ui->labelStatsIron;
    mineralsLabels[MineralType::Gold] = ui->labelStatsGold;

    itemLabels[GameItem::Greens] = ui->labelStatsGreens;
    itemLabels[GameItem::Herbs] = ui->labelStatsHerbs;
    itemLabels[GameItem::Roots] = ui->labelStatsRoots;
    itemLabels[GameItem::Willow] = ui->labelStatsWillow;

    addPixmap(mineralColor(MineralType::Clay), ui->labelIconClay);
    addPixmap(mineralColor(MineralType::Sand), ui->labelIconSand);
    addPixmap(mineralColor(MineralType::Coal), ui->labelIconCoal);
    addPixmap(mineralColor(MineralType::Iron), ui->labelIconIron);
    addPixmap(mineralColor(MineralType::Gold), ui->labelIconGold);

    addPixmap(itemColor(GameItem::Greens), ui->labelIconGreens);
    addPixmap(itemColor(GameItem::Herbs), ui->labelIconHerbs);
    addPixmap(itemColor(GameItem::Roots), ui->labelIconRoots);
    addPixmap(itemColor(GameItem::Willow), ui->labelIconWillow);

    auto list = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);
    if (list.size() > 0) {
        QDir dir = QDir(list[0]);
        dir.cd("My Games");
        dir.cd("Farthest Frontier");
        dir.cd("Save");
        saveDirectory_ = dir.path();
    }
}

FarthestFrontierMapFrame::~FarthestFrontierMapFrame()
{
    delete ui;
}

void FarthestFrontierMapFrame::on_actionOpenSav_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, windowTitle(), saveDirectory_, "Farthest Frontier Saves (*.sav)");
    if (fileName.isEmpty())
        return;
    map_.reset(new GameMap());
    if (!map_->loadSave(fileName)) {
        QMessageBox::critical(this, windowTitle(), "Can't open file");
        return;
    }

    mapStateChanged(true);

    auto reader = map_->reader();
    updateStats(mineralsLabels, reader.minerals());
    updateStats(itemLabels, reader.forageables());
    auto saveData = reader.generalSaveData();
    ui->textEdit->setPlainText(QString("name: %1\nseed: %2\nversion: %3\nvillagers: %4\n???: %5\n???: %6\nwildlife: %7\nraiders: %8\npacifist: %9\nyears: %10\n")
                               .arg(saveData.name).arg(saveData.seed).arg(saveData.version).arg(saveData.villagers).arg(saveData.v1).arg(saveData.v2)
                               .arg(saveData.wildlifeDifficulty).arg(saveData.raidersDifficulty).arg(saveData.pacifist).arg(saveData.years));
    drawMapFromUi();
}


void FarthestFrontierMapFrame::checkBoxStateChanged()
{
    drawMapFromUi();
}

void FarthestFrontierMapFrame::drawMapFromUi()
{
    MapWidget::DrawOptions opt;
    if (ui->groupBoxMinerals->isChecked()) {
        opt.clay = ui->checkBoxClay->isChecked();
        opt.sand = ui->checkBoxSand->isChecked();
        opt.coal = ui->checkBoxCoal->isChecked();
        opt.iron = ui->checkBoxIron->isChecked();
        opt.gold = ui->checkBoxGold->isChecked();
    }
    if (ui->groupBoxForageables->isChecked()) {
        opt.greens = ui->checkBoxGreens->isChecked();
        opt.herbs = ui->checkBoxHerbs->isChecked();
        opt.roots = ui->checkBoxRoots->isChecked();
        opt.willow = ui->checkBoxWillow->isChecked();
    }
    if (ui->checkBoxFertility->isChecked()) {
        opt.fertility = ui->sliderFertility->value();
    }
    if (ui->checkBoxFooder->isChecked()) {
        opt.fooder = ui->sliderFooder->value();
    }
    if (ui->checkBoxWater->isChecked()) {
        opt.water = ui->sliderWater->value();
    }
    opt.enemies = ui->checkBoxEnemies->isChecked();
    opt.animals = ui->checkBoxAnimals->isChecked();
    opt.animalsSpawns = ui->checkBoxAnimalsSpawns->isChecked();
    opt.buildings = ui->checkBoxBuildings->isChecked();
    ui->mapWidget->update(opt, map_);
}

void FarthestFrontierMapFrame::mapStateChanged(bool available)
{
    ui->actionSaveSav->setEnabled(available);
    ui->actionCloseSav->setEnabled(available);
    ui->toolButtonAddClay->setEnabled(available);
    ui->toolButtonAddSand->setEnabled(available);
    ui->toolButtonAddIron->setEnabled(available);
    ui->toolButtonAddCoal->setEnabled(available);
    ui->toolButtonAddGold->setEnabled(available);
    pendingNewMinerals.clear();
    ui->mapWidget->resetHighlight();
    ui->stackedWidgetInfoOptions->setCurrentWidget(ui->pageInfoViewOptions);
}

void FarthestFrontierMapFrame::startAddingMineral(MineralType type)
{
    ui->mapWidget->setHighlightMouse(true);
    ui->labelAddOptionsTop->setText(mineralStr(type));
    ui->labelAddOptionsLocation->setText(SelectlocationStr);
    auto& e = pendingNewMinerals.emplace_back(MineralData());
    e.type = type;
    e.amount = 0;
    ui->pushButtonAddOptions->setDisabled(true);
    ui->stackedWidgetInfoOptions->setCurrentWidget(ui->pageInfoAddOptions);
}

void FarthestFrontierMapFrame::on_actionSaveSav_triggered()
{
    SaveDialog* dialog = new SaveDialog(this);
    if (!pendingNewMinerals.empty()) {
        dialog->addInfo(QString("%1 Minerals will be added").arg(pendingNewMinerals.size()));
    }
    connect(dialog, &QDialog::finished, this, [dialog, this](int result) {
        if (result != QDialog::Accepted) {
            return;
        }
        QString fileName = QFileDialog::getSaveFileName(this, windowTitle(), saveDirectory_, "Farthest Frontier Saves (*.sav)");
        if (fileName.isEmpty())
            return;
        GameMapChanger changer(dialog->options());
        if (!changer.copy(map_->saveFileName(), fileName, pendingNewMinerals)) {
            QMessageBox::critical(this, windowTitle(), "Can't open file");
            return;
        }
        pendingNewMinerals.clear();
    });
    connect(dialog, &QDialog::finished, dialog, &QDialog::deleteLater);
    dialog->open();
}


void FarthestFrontierMapFrame::on_actionCloseSav_triggered()
{
    mapStateChanged(false);
    map_->closeSave();
    ui->textEdit->setPlainText("");
    ui->mapWidget->clear();
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


void FarthestFrontierMapFrame::on_pushButtonAddOptionsCancel_clicked()
{
    ui->mapWidget->setHighlightMouse(false);
    ui->mapWidget->resetHighlight();
    ui->stackedWidgetInfoOptions->setCurrentWidget(ui->pageInfoViewOptions);
    pendingNewMinerals.pop_back();

}

void FarthestFrontierMapFrame::on_mapWidget_clicked(const QPointF& position)
{
    if (ui->stackedWidgetInfoOptions->currentWidget() != ui->pageInfoAddOptions) {
        return;
    }
    ui->mapWidget->setHighlightMouse(false);
    Point& p = pendingNewMinerals.back().p;
    uint hx = position.x() / 5;
    uint hz = position.y() / 5;
    p.x = hx * 5;
    p.z = hz * 5;
    auto reader = map_->reader();
    auto heights = reader.heightMap();
    p.y = heights[hz][hx];
    ui->mapWidget->addHighlight(QPoint(p.x, p.z));
    ui->labelAddOptionsLocation->setText(QString("(%1, %2, %3)").arg(p.x).arg(p.y).arg(p.z));
    ui->pushButtonAddOptions->setEnabled(true);
}

void FarthestFrontierMapFrame::on_pushButtonAddOptions_clicked()
{
    ui->mapWidget->resetHighlight();
    MineralData& d = pendingNewMinerals.back();
    d.amount = ui->spinBoxAddOptionsAmount->value();
    d.r = ui->spinBoxAddOptionsRadius->value();
    ui->stackedWidgetInfoOptions->setCurrentWidget(ui->pageInfoViewOptions);
}
