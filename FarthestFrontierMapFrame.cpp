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
#include "FarthestFrontierMap.h"
#include "FarthestFrontierMapFrame.h"
#include "ui_FarthestFrontierMapFrame.h"

namespace {

const QLatin1String WindowTitle("Farthest Frontier Map");



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

}

FarthestFrontierMapFrame::FarthestFrontierMapFrame(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::FarthestFrontierMapFrame)
{
    ui->setupUi(this);
    connect(ui->checkBoxAnimals, &QCheckBox::stateChanged, this, &FarthestFrontierMapFrame::checkBoxStateChanged);
    connect(ui->checkBoxBuildings, &QCheckBox::stateChanged, this, &FarthestFrontierMapFrame::checkBoxStateChanged);
    connect(ui->checkBoxEnemies, &QCheckBox::stateChanged, this, &FarthestFrontierMapFrame::checkBoxStateChanged);
    connect(ui->groupBoxMinerals, &QGroupBox::toggled, this, &FarthestFrontierMapFrame::checkBoxStateChanged);
    connect(ui->checkBoxClay, &QCheckBox::stateChanged, this, &FarthestFrontierMapFrame::checkBoxStateChanged);
    connect(ui->checkBoxSand, &QCheckBox::stateChanged, this, &FarthestFrontierMapFrame::checkBoxStateChanged);
    connect(ui->checkBoxGold, &QCheckBox::stateChanged, this, &FarthestFrontierMapFrame::checkBoxStateChanged);
    connect(ui->checkBoxIron, &QCheckBox::stateChanged, this, &FarthestFrontierMapFrame::checkBoxStateChanged);
    connect(ui->checkBoxCoal, &QCheckBox::stateChanged, this, &FarthestFrontierMapFrame::checkBoxStateChanged);
    connect(ui->groupBoxForageables, &QGroupBox::toggled, this, &FarthestFrontierMapFrame::checkBoxStateChanged);
    connect(ui->checkBoxHerbs, &QCheckBox::stateChanged, this, &FarthestFrontierMapFrame::checkBoxStateChanged);
    connect(ui->checkBoxRoots, &QCheckBox::stateChanged, this, &FarthestFrontierMapFrame::checkBoxStateChanged);
    connect(ui->checkBoxWillow, &QCheckBox::stateChanged, this, &FarthestFrontierMapFrame::checkBoxStateChanged);

    mineralsLabels[MineralType::Clay] = ui->labelStatsClay;
    mineralsLabels[MineralType::Sand] = ui->labelStatsSand;
    mineralsLabels[MineralType::Coal] = ui->labelStatsCoal;
    mineralsLabels[MineralType::Iron] = ui->labelStatsIron;
    mineralsLabels[MineralType::Gold] = ui->labelStatsGold;

    addPixmap(mineralColor(MineralType::Clay), ui->labelIconClay);
    addPixmap(mineralColor(MineralType::Sand), ui->labelIconSand);
    addPixmap(mineralColor(MineralType::Coal), ui->labelIconCoal);
    addPixmap(mineralColor(MineralType::Iron), ui->labelIconIron);
    addPixmap(mineralColor(MineralType::Gold), ui->labelIconGold);

    addPixmap(itemColor(GameItem::Herbs), ui->labelIconHerbs);
    addPixmap(itemColor(GameItem::Roots), ui->labelIconRoots);
    addPixmap(itemColor(GameItem::Willow), ui->labelIconWillow);

    auto list = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);
    if (list.size() > 0) {
        QDir path = QDir(list[0]);
        path.cd("My Games");
        path.cd("Farthest Frontier");
        path.cd("Save");
        savePath_ = path.path();
    }
}

FarthestFrontierMapFrame::~FarthestFrontierMapFrame()
{
    delete ui;
}


void FarthestFrontierMapFrame::on_actionOpenMap_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, windowTitle(), savePath_, "Farthest Frontier Maps (*.map)");
    if (fileName.isEmpty())
        return;

    map_.reset(new FarthestFrontierMap(this));
    map_->loadMap(fileName);
    QPixmap m = map_->landscape();
    ui->labelMap->setPixmap(m);
    ui->textEdit->setPlainText(QString("width: %1 height: %2").arg(m.width()).arg(m.height()));
}


void FarthestFrontierMapFrame::on_actionOpenSav_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, windowTitle(), savePath_, "Farthest Frontier Saves (*.sav)");
    if (fileName.isEmpty())
        return;
    map_.reset(new FarthestFrontierMap(this));
    if (!map_->loadSave(fileName)) {
        QMessageBox::critical(this, windowTitle(), "Can't open file");
        return;
    }
    ui->actionSaveSav->setEnabled(true);
    ui->actionCloseSav->setEnabled(true);
    drawMapFromUi();
    updateStats();
    auto saveData = map_->generalSaveData();
    ui->textEdit->setPlainText(QString("name: %1\nseed: %2\nversion: %3\nvillagers: %4\n???: %5\n???: %6\nwildlife: %7\nraiders: %8\npacifist: %9\nyears: %10\n")
                               .arg(saveData.name).arg(saveData.seed).arg(saveData.version).arg(saveData.villagers).arg(saveData.v1).arg(saveData.v2)
                               .arg(saveData.wildlifeDifficulty).arg(saveData.raidersDifficulty).arg(saveData.pacifist).arg(saveData.years));
}


void FarthestFrontierMapFrame::checkBoxStateChanged()
{
    drawMapFromUi();
}

void FarthestFrontierMapFrame::drawMapFromUi()
{
    DrawOptions opt;
    if (ui->groupBoxMinerals->isChecked()) {
        opt.clay = ui->checkBoxClay->isChecked();
        opt.sand = ui->checkBoxSand->isChecked();
        opt.coal = ui->checkBoxCoal->isChecked();
        opt.iron = ui->checkBoxIron->isChecked();
        opt.gold = ui->checkBoxGold->isChecked();
    }

    if (ui->groupBoxForageables->isChecked()) {
        opt.herbs = ui->checkBoxHerbs->isChecked();
        opt.roots = ui->checkBoxRoots->isChecked();
        opt.willow = ui->checkBoxWillow->isChecked();
    }

    opt.enemies = ui->checkBoxEnemies->isChecked();
    opt.animals = ui->checkBoxAnimals->isChecked();
    opt.buildings = ui->checkBoxBuildings->isChecked();

    drawMap(opt);
}

void FarthestFrontierMapFrame::drawMap(const DrawOptions &opt)
{
    if (map_.isNull()) {
        return;
    }
    auto size = map_->size();
    uint imageWidth = size.x / 2;
    QPixmap image(imageWidth, size.y / 2);
    image.fill(Qt::white);
    QPainter p(&image);
    {
        auto start = map_->start();
        p.setPen(Qt::blue);
        constexpr int ls = 5;
        p.drawLine(imageWidth - start.x / 2, start.z / 2 - ls, imageWidth - start.x / 2, start.z / 2 + ls);
        p.drawLine(imageWidth - start.x / 2 - ls, start.z / 2, imageWidth - start.x / 2  + ls, start.z / 2);

    }
    std::vector<MineralData> mineralsList;
    if (opt.clay || opt.sand || opt.coal || opt.iron || opt.gold) {
        mineralsList = map_->minerals();
        for (const auto& m : mineralsList) {
            if (!checkMineralOption(m.type, opt)) {
                continue;
            }
            auto c = mineralColor(m.type);
            p.setPen(c);
            p.setBrush(c);
            p.drawEllipse(imageWidth - m.p.x / 2 - 10, m.p.z / 2 - 10, 20, 20);
        }
    }
    if (opt.herbs || opt.roots || opt.willow) {
        for (const auto& m : map_->forageables()) {
            switch (m.type)
            {
            case GameItem::Herbs:
                if (!opt.herbs)
                    continue;
                break;
            case GameItem::Willow:
                if (!opt.willow)
                    continue;
                break;
            case GameItem::Roots:
                if (!opt.roots)
                    continue;
                break;
            default:
                continue;
            }
            QColor bc = itemColor(m.type);
            int r = 2;
            if (m.count > 19)
                r = 4;
            else if (m.count > 14)
                r = 3;
            p.setPen(Qt::black);
            p.setBrush(bc);
            p.drawEllipse(imageWidth - m.p.x / 2 - r, m.p.z / 2 - r, r * 2, r * 2);
        }
    }
    if (opt.animals) {
        for (const auto& m : map_->animals()) {
            QColor bc;
            QColor pc;
            switch (m.type)
            {
            case BaseType::Wolf:
                pc = Qt::red;
                bc = Qt::darkMagenta;
                break;
            case BaseType::Bear:
                pc = Qt::red;
                bc = Qt::red;
                break;
            case BaseType::Deer:
                pc = Qt::green;
                bc = QColor(128, 255, 0);
                break;
            case BaseType::Boar:
                pc = Qt::green;
                bc = QColor(255, 216, 0);
                break;
            default:
                continue;
            }
            p.setPen(pc);
            constexpr int ls = 5;
            p.drawLine(imageWidth - m.p.x / 2, m.p.z / 2 - ls, imageWidth - m.p.x / 2, m.p.z / 2 + ls);
            p.drawLine(imageWidth - m.p.x / 2 - ls, m.p.z / 2, imageWidth - m.p.x / 2  + ls, m.p.z / 2);
            p.setPen(bc);
            p.setBrush(bc);
            constexpr int r = 3;
            p.drawEllipse(imageWidth - m.p.x / 2 - r, m.p.z / 2 - r, r * 2, r * 2);
        }
    }
    if (opt.enemies) {
        for (const auto& m : map_->raiders()) {
            QColor bc;
            constexpr int ls = 5;
            p.setPen(Qt::lightGray);
            p.drawLine(imageWidth - m.p.x / 2, m.p.z / 2, imageWidth - m.spawn.x / 2, m.spawn.z / 2);
            p.setPen(Qt::magenta);
            p.drawLine(imageWidth - m.p.x / 2, m.p.z / 2 - ls, imageWidth - m.p.x / 2, m.p.z / 2 + ls);
            p.drawLine(imageWidth - m.p.x / 2 - ls, m.p.z / 2, imageWidth - m.p.x / 2  + ls, m.p.z / 2);
            p.setPen(bc);
            p.setBrush(bc);
            constexpr int r = 3;
            p.drawEllipse(imageWidth - m.p.x / 2 - r, m.p.z / 2 - r, r * 2, r * 2);
        }
    }
    if (opt.buildings) {
        for (const auto& m : map_->houses()) {
            int ls = 0;
            switch (m.type)
            {
            case BaseType::Shelter:
                ls = 5;
                break;
            case BaseType::TownCenter:
                ls = 10;
                break;
            default:
                continue;
            }
            p.setPen(Qt::blue);
            p.setBrush(Qt::blue);
            p.drawRect(imageWidth - m.p.x / 2 - ls, m.p.z / 2 - ls, ls, ls);
        }
    }
    if (opt.clay || opt.sand || opt.coal || opt.iron || opt.gold) {
        p.setPen(Qt::black);
        for (const auto& m : mineralsList) {
            if (checkMineralOption(m.type, opt)) {
                p.drawText(QRect(imageWidth - m.p.x / 2 - 20, m.p.z / 2 + 10, 40, 16), Qt::AlignCenter, QString::number(m.count));
            }
        }
    }

    ui->labelMap->setPixmap(image);
}

void FarthestFrontierMapFrame::updateStats()
{
    for (const auto& l : mineralsLabels) {
        l.second->setText("");
    }
    std::unordered_map<MineralType, std::pair<uint, uint>> numbers;
    for (const auto& i : map_->minerals()) {
        std::pair<uint, uint>& mi = numbers[i.type];
        mi.first += i.count;
        ++mi.second;
    }

    QLocale loc(QLocale::English);
    for (const auto& m : numbers) {
        auto label = mineralsLabels.find(m.first);
        if (label == mineralsLabels.end()) {
            continue;
        }
        label->second->setText(QString("x%1 total: %2").arg(m.second.second).arg(loc.toString(m.second.first)));
    }
}

void FarthestFrontierMapFrame::on_actionSaveSav_triggered()
{
    ui->stackedWidget->setCurrentWidget(ui->pageSave);
}


void FarthestFrontierMapFrame::on_pushButtonCancel_clicked()
{
   ui->stackedWidget->setCurrentWidget(ui->pageInfo);
}


void FarthestFrontierMapFrame::on_pushButtonSave_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this, windowTitle(), savePath_, "Farthest Frontier Saves (*.sav)");
    if (fileName.isEmpty())
        return;
    if (!map_->copySave(fileName, ui->checkBoxSaveFOW->isChecked())) {
        QMessageBox::critical(this, windowTitle(), "Can't open file");
    }

}

bool FarthestFrontierMapFrame::checkMineralOption(MineralType v, const DrawOptions &opt)
{
    switch (v) {
    case MineralType::Iron:
        if (!opt.iron)
            return false;
        break;
    case MineralType::Gold:
        if (!opt.gold)
            return false;
        break;
    case MineralType::Coal:
        if (!opt.coal)
            return false;
        break;
    case MineralType::Clay:
        if (!opt.clay)
            return false;
        break;
    case MineralType::Sand:
        if (!opt.sand)
            return false;
        break;
    }
    return true;
}

void FarthestFrontierMapFrame::on_actionCloseSav_triggered()
{
    ui->stackedWidget->setCurrentWidget(ui->pageInfo);
    ui->actionSaveSav->setEnabled(false);
    ui->actionCloseSav->setEnabled(false);
    map_->closeSave();
    ui->textEdit->setPlainText("");
    ui->labelMap->setPixmap(QPixmap());
}

