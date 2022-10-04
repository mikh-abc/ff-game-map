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
    case GameItem::Greens:
        return QColor(90, 200, 90);
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

struct DrawOptions
{
    bool sand = false;
    bool clay = false;
    bool coal = false;
    bool iron = false;
    bool gold = false;

    bool greens = false;
    bool herbs = false;
    bool roots = false;
    bool willow = false;

    bool animals = false;
    bool enemies = false;
    bool buildings = false;

    int fertility = 0;
    int fooder = 0;
    int water = 0;
};

bool checkMineralOption(MineralType v, const DrawOptions &opt)
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

void drawMap(QPromise<QPixmap>& promise, const DrawOptions& opt, QSharedPointer<FarthestFrontierMap> map)
{
    if (map.isNull()) {
        promise.addResult(QPixmap());
        return;
    }
    auto reader = map->reader();
    auto agricultureData = reader.agricultureData();
    uint imageWidth = agricultureData.worldWidth / 2;
    QPixmap image(imageWidth, agricultureData.worldHeight / 2);
    image.fill(Qt::white);
    QPainter p(&image);
    auto drawLevel = [&](const QColor& color, const std::vector<std::vector<float>>& array, float limit) {
        p.setPen(color);
        p.setBrush(color);
        for (int x = 0; x < array.size(); ++x) {
            for (int y = 0; y < array[x].size(); ++y) {
                if (array[x][y] > limit) {
                    p.drawRect(imageWidth - y * 2.5, x * 2.5, 2, 2);
                }
            }
        }
        if (promise.isCanceled()) {
            return;
        }
    };
    if (opt.water)
    {
        drawLevel(QColor(128, 194, 255), agricultureData.data[AgricultureInfo::Water], opt.water / 100.0);
    }
    if (opt.fertility)
    {
        drawLevel(QColor(194, 255, 194), agricultureData.data[AgricultureInfo::Fertility], opt.fertility / 100.0);
    }
    if (opt.fooder)
    {
        drawLevel(QColor(128, 255, 194), agricultureData.data[AgricultureInfo::Fooder], opt.fooder / 100.0);
    }

    std::vector<MineralData> mineralsList;
    if (opt.clay || opt.sand || opt.coal || opt.iron || opt.gold) {
        mineralsList = reader.minerals();
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
    if (opt.greens || opt.herbs || opt.roots || opt.willow) {
        for (const auto& m : reader.forageables()) {
            switch (m.type)
            {
            case GameItem::Greens:
                if (!opt.greens)
                    continue;
                break;
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
        for (const auto& m : reader.animals()) {
            QColor circleColor;
            QColor crossColor;
            QColor borderCircleColor;
            int ls = 5;
            int r = 3;
            switch (m.type)
            {
            case BaseType::Wolf:
                crossColor = Qt::red;
                circleColor = Qt::darkMagenta;
                borderCircleColor = Qt::darkMagenta;
                break;
            case BaseType::WolfDen:
                crossColor = Qt::black;
                circleColor = Qt::darkMagenta;
                borderCircleColor = Qt::black;
                break;
            case BaseType::Bear:
                crossColor = Qt::red;
                circleColor = Qt::red;
                borderCircleColor = Qt::red;
                break;
            case BaseType::Deer:
                crossColor = Qt::green;
                circleColor = QColor(128, 216, 0);
                borderCircleColor = Qt::green;
                ls = 4;
                r = 2;
                break;
            case BaseType::Boar:
                crossColor = Qt::green;
                circleColor = QColor(255, 216, 0);
                borderCircleColor = Qt::green;
                ls = 4;
                r = 2;
                break;
            default:
                continue;
            }
            p.setPen(crossColor);
            p.drawLine(imageWidth - m.p.x / 2, m.p.z / 2 - ls, imageWidth - m.p.x / 2, m.p.z / 2 + ls);
            p.drawLine(imageWidth - m.p.x / 2 - ls, m.p.z / 2, imageWidth - m.p.x / 2  + ls, m.p.z / 2);
            p.setPen(borderCircleColor);
            p.setBrush(circleColor);
            p.drawEllipse(imageWidth - m.p.x / 2 - r, m.p.z / 2 - r, r * 2, r * 2);
        }
    }
    if (opt.enemies) {
        for (const auto& m : reader.raiders()) {
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
        for (const auto& m : reader.houses()) {
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
    {
        auto start = reader.camera();
        p.setPen(Qt::blue);
        constexpr int ls = 5;
        p.drawLine(imageWidth - start.x / 2, start.z / 2 - ls, imageWidth - start.x / 2, start.z / 2 + ls);
        p.drawLine(imageWidth - start.x / 2 - ls, start.z / 2, imageWidth - start.x / 2  + ls, start.z / 2);
    }
    promise.addResult(std::move(image));
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

template<class L, class T>
void updateStats(const std::unordered_map<L, QLabel*>& labels, const std::vector<T>& list)
{
    for (const auto& l : labels) {
        l.second->setText("");
    }
    std::unordered_map<L, std::pair<uint, uint>> numbers;
    for (const auto& i : list) {
        std::pair<uint, uint>& ni = numbers[i.type];
        ni.first += i.count;
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
    map_.reset(new FarthestFrontierMap(this));
    if (!map_->loadSave(fileName)) {
        QMessageBox::critical(this, windowTitle(), "Can't open file");
        return;
    }
    ui->actionSaveSav->setEnabled(true);
    ui->actionCloseSav->setEnabled(true);
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
    future_.cancel();
    DrawOptions opt;
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
    opt.buildings = ui->checkBoxBuildings->isChecked();

    future_ = QtConcurrent::run(drawMap, opt, map_);
    auto watcher = new QFutureWatcher<QPixmap>(this);
    auto label = ui->labelMap;
    connect(watcher, &QFutureWatcher<QPixmap>::finished, this, [watcher, label]() {
        if (!watcher->isCanceled()) {
            label->setPixmap(watcher->result());
        }
    });
    connect(watcher, &QFutureWatcher<QPixmap>::finished, watcher, &QFutureWatcher<QPixmap>::deleteLater);
    watcher->setFuture(future_);
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
    QString fileName = QFileDialog::getSaveFileName(this, windowTitle(), saveDirectory_, "Farthest Frontier Saves (*.sav)");
    if (fileName.isEmpty())
        return;
    if (!map_->copySave(fileName, ui->checkBoxSaveFOW->isChecked(), ui->checkBoxSaveBuildingState->isChecked())) {
        QMessageBox::critical(this, windowTitle(), "Can't open file");
    }

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
