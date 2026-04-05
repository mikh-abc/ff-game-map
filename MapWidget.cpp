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
#include "MapWidget.h"
#include "DataReader.h"

namespace {

constexpr int HighlightRadius = 10;

bool checkMineralOption(MineralType v, const MapWidget::DrawOptions &opt)
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
    case MineralType::Stone:
        if (!opt.stone)
            return false;
        break;
    case MineralType::FertilityBonus:
        if (opt.fertility == 0)
            return false;
        break;
    case MineralType::Unknown:
        break;
    }
    return true;
}

uint calcRectCoordinate(int dirtyBegin, int dirtyLen, int mapBegin, int mapLen, int& rDirtyBegin, int& rMapBegin)
{
    int d = mapBegin - dirtyBegin;
    int dirtyRight = dirtyBegin + dirtyLen;
    if (dirtyBegin <= mapBegin) {
        rDirtyBegin = mapBegin;
        if (d > dirtyLen) {
            return 0;
        } else if (dirtyRight < mapBegin + mapLen) {
            return dirtyLen - d;
        } else {
            return mapLen;
        }
    } else if (dirtyBegin > mapBegin && dirtyBegin < mapBegin + mapLen) {
        rDirtyBegin = dirtyBegin;
        rMapBegin = -d;
        if (dirtyRight < mapBegin + mapLen) {
            return dirtyLen;
        } else {
            return mapLen + d;
        }
    }
    return 0;
}

void drawMap(QPromise<QPixmap>& promise, const MapWidget::DrawOptions& opt, QSharedPointer<DataReader> map, float scale)
{
    if (map.isNull()) {
        promise.addResult(QPixmap());
        return;
    }
    if (promise.isCanceled()) {
        return;
    }
    auto agricultureData = map->agricultureData();
    float worldWidth = agricultureData.worldWidth;
    float worldHeight = agricultureData.worldHeight;
    uint imageWidth = worldWidth / scale;
    uint imageHeight = worldHeight / scale;
    QPixmap image(imageWidth, imageHeight);
    image.fill(Qt::white);
    QPainter p(&image);
    const float scaledCellSize = CellSize / scale;

    if (opt.heightMap && opt.heightMax > opt.heightMin) {
        auto heightMap = map->terrainData().heights;
        size_t border = heightMap.size() / 10;
        for (int x = 0; x < heightMap.size(); ++x) {
            for (int y = 0; y < heightMap[x].size(); ++y) {
                float v = 1 - (heightMap[x][y] - opt.heightMin) / (opt.heightMax - opt.heightMin);
                if (v < 0.5 && (x < border || x > heightMap.size() - border || y < border || y > heightMap[x].size() - border)) {
                    v = 0.5;
                }
                QColor color = QColor::fromRgbF(v, v, v, 1);
                p.setPen(color);
                p.setBrush(color);
                p.drawRect(imageWidth - y * scaledCellSize, x * scaledCellSize, scaledCellSize, scaledCellSize);
            }
        }
        if (promise.isCanceled()) {
            return;
        }
    }

    auto drawLevel = [imageWidth, scaledCellSize](QPainter& plevel, const QColor& color, const std::vector<std::vector<float>>& array, float limit) {
        plevel.setPen(color);
        plevel.setBrush(color);
        for (int x = 0; x < array.size(); ++x) {
            for (int y = 0; y < array[x].size(); ++y) {
                if (array[x][y] > limit) {
                    plevel.drawRect(imageWidth - y * scaledCellSize, x * scaledCellSize, scaledCellSize, scaledCellSize);
                }
            }
        }
    };
    if (opt.water)
    {
        drawLevel(p, QColor(128, 194, 255), agricultureData.data[AgricultureInfo::Water], opt.water / 100.0);
        if (promise.isCanceled()) {
            return;
        }
    }
    if (opt.fertility || opt.fodder) {
        QPixmap imageLevels(imageWidth, imageHeight);
        imageLevels.fill(Qt::transparent);
        QPainter pLevels(&imageLevels);
        if (opt.fertility)
        {
            drawLevel(pLevels, QColor(194, 255, 194), agricultureData.data[AgricultureInfo::EnvFertility], opt.fertility / 100.0);
            if (promise.isCanceled()) {
                return;
            }

        }
        if (opt.fodder)
        {
            drawLevel(pLevels, QColor(128, 255, 194), agricultureData.data[AgricultureInfo::Fodder], opt.fodder / 100.0);
            if (promise.isCanceled()) {
                return;
            }

        }
        p.setOpacity(0.5);
        p.drawPixmap(0, 0, imageLevels);
        p.setOpacity(1);
    }
    if (opt.animalsSpawns) {
        const float scaledAreaSize = AnimalSpawnAreaSize / scale;
        uint lx = worldWidth / AnimalSpawnAreaSize;

        for (const auto& s : map->animalsSpawns()) {
            auto c = animalSpawnColor(s.second.type);
            QColor pc;
            QColor bc;
            switch (s.second.type)
            {
            case BaseType::Bear:
                pc = QColor(192, 64, 255);
                bc = QColor(255, 212, 212, 128);
                break;
            case BaseType::Wolf:
                pc = QColor(255, 64, 64);
                bc = QColor(255, 212, 212, 128);
                break;
            case BaseType::Deer:
                pc = QColor(0, 192, 0);
                bc = QColor(212, 255, 212, 128);
                break;
            case BaseType::Boar:
                pc = QColor(255, 216, 0);
                bc = QColor(255, 233, 128, 128);
                break;
            default:
                continue;
            }
            p.setPen(pc);
            p.setBrush(bc);
            uint y = s.first / lx;
            uint x = s.first % lx + 1;
            p.drawRect(imageWidth - x * scaledAreaSize, y * scaledAreaSize, scaledAreaSize, scaledAreaSize);
        }
    }
    std::vector<MineralData> mineralsList;
    bool anyMinerals = opt.clay || opt.sand || opt.coal || opt.iron || opt.gold || opt.stone || (opt.fertility > 0);
    if (anyMinerals) {
        mineralsList = map->minerals();
        for (const auto& m : mineralsList) {
            if (!checkMineralOption(m.type, opt)) {
                continue;
            }
            auto c = mineralColor(m.type);
            p.setPen(c);
            p.setBrush(c);
            int r = m.r / scaledCellSize;
            p.drawEllipse(imageWidth - m.p.x / scale - r, m.p.z / scale - r, r * 2, r * 2);
        }
    }
    if (opt.greens || opt.herbs || opt.roots || opt.willow) {
        for (const auto& m : map->forageables()) {
            if (m.replenish.empty())
                continue;
            const GameItemCount& f = m.replenish[0];
            switch (static_cast<GameItem>(f.item))
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
            QColor bc = itemColor(f.item);
            int r = 2;
            if (f.count > 19)
                r = 4;
            else if (f.count > 14)
                r = 3;
            p.setPen(Qt::black);
            p.setBrush(bc);
            p.drawEllipse(imageWidth - m.p.x / scale - r, m.p.z / scale - r, r * 2, r * 2);
        }
    }
    if (opt.animals) {
        for (const auto& m : map->animals()) {
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
            p.drawLine(imageWidth - m.p.x / scale, m.p.z / scale - ls, imageWidth - m.p.x / scale, m.p.z / scale + ls);
            p.drawLine(imageWidth - m.p.x / scale - ls, m.p.z / scale, imageWidth - m.p.x / scale  + ls, m.p.z / scale);
            p.setPen(borderCircleColor);
            p.setBrush(circleColor);
            p.drawEllipse(imageWidth - m.p.x / scale - r, m.p.z / scale - r, r * 2, r * 2);
        }
    }
    if (opt.enemies) {
        for (const auto& m : map->raiders()) {
            switch (m.type) {
            case RaiderType::RaiderCamp:
            {
                p.setPen(Qt::black);
                p.setBrush(QColor(0xFF7F7F));
                constexpr int r = 10;
                p.drawEllipse(imageWidth - m.p.x / scale - r, m.p.z / scale - r, r * scale, r * scale);
                constexpr int ls = 3;
                p.fillRect(imageWidth - m.p.x / scale - ls, m.p.z / scale - ls, ls * scale, ls * scale, Qt::black);
                break;
            }
            case RaiderType::BatteringRam:
            {
                p.setPen(Qt::black);
                p.setBrush(Qt::black);
                constexpr int r = 3;
                p.drawEllipse(imageWidth - m.p.x / scale - r, m.p.z / scale - r, r * scale, r * scale);
                p.setPen(Qt::magenta);
                constexpr int inner = 2;
                p.drawEllipse(imageWidth - m.p.x / scale - r, m.p.z / scale - inner, inner * scale, inner * scale);
                break;
            }
            case RaiderType::Thief:
            case RaiderType::Brawler:
            case RaiderType::Warrior:
            case RaiderType::Shieldbearer:
            case RaiderType::Warmaster:
            case RaiderType::Arbalest:
            case RaiderType::Champion:
            case RaiderType::Footman:
            case RaiderType::HeavyInfantry:
            {
                constexpr int ls = 5;
                constexpr int r = 3;
                p.setPen(Qt::darkRed);
                // line to spawn
                p.drawLine(imageWidth - m.p.x / scale, m.p.z / scale, imageWidth - m.spawn.x / scale, m.spawn.z / scale);
                // cross lines
                p.drawLine(imageWidth - m.p.x / scale, m.p.z / scale - ls, imageWidth - m.p.x / scale, m.p.z / scale + ls);
                p.drawLine(imageWidth - m.p.x / scale - ls, m.p.z / scale, imageWidth - m.p.x / scale  + ls, m.p.z / scale);
                p.setBrush(Qt::black);
                p.drawEllipse(imageWidth - m.p.x / scale - r, m.p.z / scale - r, r * scale, r * scale);
                break;
            }
            default:
                break;
            }
        }
    }
    if (opt.buildings) {
        std::unordered_map<uint32_t, uint32_t> counter;
        for (const auto& i : map->villagers()) {
            ++counter[i.homeResidence];
        }

        for (const auto& i : map->sites()) {
            int ls = 0;
            QColor color = Qt::transparent;
            const BaseData& building = i.second;
            switch (building.type)
            {
            case BaseType::RelicExtraction:
                ls = 3 * scaledCellSize;
                color = QColor(0xDA, 0xA5, 0x20);
                break;
            case BaseType::SalvagingSite:
                ls = 3 * scaledCellSize;
                color = QColor(0xF0, 0xE6, 0x8C);
                break;
            case BaseType::Shelter:
            {
                ls = 3 * scaledCellSize;
                auto it = counter.find(i.first);
                if (it != counter.end() && it->second > 0) {
                    color = QColor(0x64, 0x95, 0xED);
                } else {
                    color = QColor(0xAD, 0xD8, 0xE6);
                }
                break;
            }
            case BaseType::TownCenter:
                ls = 5 * scaledCellSize;
                color = QColor(0x64, 0x95, 0xED);
                break;
            default:
                continue;
            }
            p.fillRect(imageWidth - building.p.x / scale - ls, building.p.z / scale - ls, ls, ls, color);
        }
    }
    if (anyMinerals) {
        p.setPen(Qt::black);
        for (const auto& m : mineralsList) {
            if (checkMineralOption(m.type, opt)) {
                int r = m.r / scaledCellSize * 5;
                p.drawText(QRect(imageWidth - m.p.x / scale - r, m.p.z / scale - r, r * 2, r * 2), Qt::AlignCenter, m.deep ? QString("∞") : QString::number(m.amount));
            }
        }
    }
    {
        auto start = map->camera();
        p.setPen(Qt::blue);
        constexpr int ls = 5;
        p.drawLine(imageWidth - start.x / scale, start.z / scale - ls, imageWidth - start.x / scale, start.z / scale + ls);
        p.drawLine(imageWidth - start.x / scale - ls, start.z / scale, imageWidth - start.x / scale  + ls, start.z / scale);
    }
    promise.addResult(std::move(image));
}

}

MapWidget::MapWidget(QWidget *parent)
    : QWidget{parent}
    , scale_(2)
{
}

MapWidget::~MapWidget()
{
}

void MapWidget::setScale(float v)
{
    scale_ = v;
}


void MapWidget::startHighlightMouse(HighlightType type)
{
    highlightMouse_.enabled = true;
    highlightMouse_.added = false;
    highlightMouse_.type = type;
    setMouseTracking(true);
}

void MapWidget::finishHighlightMouse()
{
    highlightMouse_.enabled = false;
    highlightMouse_.added = false;
    setMouseTracking(false);
    if (highlightMouse_.show) {
        widgetUpdate(highlightMouse_.p, highlightMouse_.type);
        highlightMouse_.show = false;
        highlightMouse_.added = true;
    }
}


void MapWidget::addHighlight(const QPoint& position, QColor pen, QColor brush)
{
    QRect br = mapImage_.rect();
    int mapWidth = br.width();
    switch (highlightMouse_.type) {
    case HighlightType::Circle:
    {
        auto& res = highlightCircles_.emplace_back(HighlightColor{QPoint(mapWidth - position.x() / scale_, position.y() / scale_), pen, brush});
        widgetUpdate(res.p, highlightMouse_.type);
        break;
    }
    case HighlightType::Rect:
    {
        auto& res = highlightRects_.emplace_back(HighlightColor{QPoint(mapWidth - position.x() / scale_, position.y() / scale_), pen, brush});
        widgetUpdate(res.p, highlightMouse_.type);
        break;
    }
    default:
        break;
    }
}

void MapWidget::resetHighlight()
{
    std::vector<QPoint> points;
    for (const auto& mdata : highlightCircles_) {
        points.push_back(mdata.p);
    }
    highlightCircles_.clear();
    for (const auto& p : points) {
        widgetUpdate(p, HighlightType::Circle);
    }

    for (const auto& mdata : highlightRects_) {
        points.push_back(mdata.p);
    }
    highlightRects_.clear();
    for (const auto& p : points) {
        widgetUpdate(p, HighlightType::Rect);
    }
}

void MapWidget::cancelHighlight()
{
    highlightMouse_.enabled = false;
    setMouseTracking(false);
    if (highlightMouse_.show) {
        widgetUpdate(highlightMouse_.p, highlightMouse_.type);
        highlightMouse_.show = false;
    } else if (highlightMouse_.added && !highlightCircles_.empty()) {
        widgetUpdate(highlightCircles_.back().p, highlightMouse_.type);
        highlightCircles_.pop_back();
    }
}

void MapWidget::update(const DrawOptions& opt, const QSharedPointer<DataReader>& map)
{
    future_.cancel();
    future_.waitForFinished();
    future_ = QtConcurrent::run(drawMap, opt, map, scale_);
    auto watcher = new QFutureWatcher<QPixmap>(this);

    connect(watcher, &QFutureWatcher<QPixmap>::finished, this, [watcher, this]() {
        if (!watcher->isCanceled()) {
            mapImage_ = watcher->result();
            QRect br = mapImage_.rect();
            setMinimumSize(br.size());
            repaint();
        }
    });
    connect(watcher, &QFutureWatcher<QPixmap>::finished, watcher, &QFutureWatcher<QPixmap>::deleteLater);
    watcher->setFuture(future_);
}

void MapWidget::clear()
{
    mapImage_ = QPixmap();
    repaint();
}

void MapWidget::mousePressEvent(QMouseEvent* event) {
    QRect cr = contentsRect();
    QRect br = mapImage_.rect();
    int mapWidth = br.width();
    int mapHeight = br.height();
    int xo = (cr.width() - mapWidth) / 2;
    int yo = (cr.height() - mapHeight) / 2;
    auto p = event->position();
    if (p.x() < xo || p.y() < yo || p.x() > xo + mapWidth || p.y() > yo + mapHeight) {
        return;
    }
    emit clicked(QPointF((mapWidth - p.x() + xo) * scale_, (p.y() - yo) * scale_));
}

void MapWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (!highlightMouse_.enabled) {
        return;
    }
    QRect cr = contentsRect();
    QRect mapRect = mapImage_.rect();
    int mapWidth = mapRect.width();
    int mapHeight = mapRect.height();
    int xo = (cr.width() - mapWidth) / 2;
    int yo = (cr.height() - mapHeight) / 2;
    QPoint p = event->position().toPoint();
    if (p.x() < xo || p.y() < yo || p.x() > xo + mapWidth || p.y() > yo + mapHeight) {
        if (highlightMouse_.show) {
            widgetUpdate(highlightMouse_.p, highlightMouse_.type);
            highlightMouse_.show = false;
        }
        return;
    }
    widgetUpdate(highlightMouse_.p, highlightMouse_.type);
    highlightMouse_.show = true;
    highlightMouse_.p = p;
    widgetUpdate(p, highlightMouse_.type);
}

void MapWidget::leaveEvent(QEvent* /*event*/)
{
    if (!highlightMouse_.enabled) {
        return;
    }
    if (highlightMouse_.show) {
        highlightMouse_.show = false;
        widgetUpdate(highlightMouse_.p, highlightMouse_.type);
    }
}

void MapWidget::paintEvent(QPaintEvent *event)
{
    QRect cr = contentsRect();
    QRect mapRect = mapImage_.rect();
    int mapWidth = mapRect.width();
    int mapHeight = mapRect.height();
    int xo = (cr.width() - mapWidth) / 2;
    int yo = (cr.height() - mapHeight) / 2;

    QPainter painter(this);
    QRect dirtyRect = event->rect();

    int dirtyLeft = 0;
    int mapLeft = 0;
    int w = calcRectCoordinate(dirtyRect.left(), dirtyRect.width(), xo, mapWidth, dirtyLeft, mapLeft);
    if (w == 0) {
        return;
    }
    dirtyRect.setX(dirtyLeft);
    dirtyRect.setWidth(w);
    mapRect.setX(mapLeft);
    mapRect.setWidth(w);

    int dirtyTop = 0;
    int mapTop = 0;
    int h = calcRectCoordinate(dirtyRect.top(), dirtyRect.height(), yo, mapHeight, dirtyTop, mapTop);
    if (h == 0) {
        return;
    }
    dirtyRect.setY(dirtyTop);
    dirtyRect.setHeight(h);
    mapRect.setY(mapTop);
    mapRect.setHeight(h);

    painter.drawPixmap(dirtyRect, mapImage_, mapRect);
    const int scaledAreaSize = AnimalSpawnAreaSize / scale_;
    for (const auto& mdata : highlightCircles_) {
        const auto& p = mdata.p;
        painter.setBrush(mdata.brush);
        painter.drawEllipse(xo + p.x() - HighlightRadius, yo + p.y() - HighlightRadius, HighlightRadius * 2, HighlightRadius * 2);
    }
    for (const auto& mdata : highlightRects_) {
        const auto& p = mdata.p;
        painter.setPen(mdata.pen);
        painter.setBrush(mdata.brush);
        painter.drawRect(xo + p.x(), yo + p.y(), scaledAreaSize, scaledAreaSize);
    }

    painter.setPen(Qt::black);
    painter.setBrush(Qt::transparent);
    if (highlightMouse_.show) {
        switch (highlightMouse_.type) {
        case HighlightType::Circle:
            painter.drawEllipse(highlightMouse_.p.x() - HighlightRadius, highlightMouse_.p.y() - HighlightRadius, HighlightRadius * 2, HighlightRadius * 2);
            break;
        case HighlightType::Rect:
        {
            uint x = (highlightMouse_.p.x() - xo) / scaledAreaSize * scaledAreaSize + xo;
            uint y = (highlightMouse_.p.y() - yo) / scaledAreaSize * scaledAreaSize + yo;
            painter.drawRect(x, y, scaledAreaSize, scaledAreaSize);
            break;
        }
        default:
            break;
        }
    }
}

void MapWidget::widgetUpdate(const QPoint& p, HighlightType type)
{
    constexpr int HighlightCircleSize = 15;
    constexpr int HighlightRectSize = 40;

    switch (type) {
    case HighlightType::Circle:
        QWidget::update(QRect(p, p).adjusted(-HighlightCircleSize, -HighlightCircleSize, HighlightCircleSize, HighlightCircleSize));
        break;
    case HighlightType::Rect:
        QWidget::update(QRect(p, p).adjusted(-HighlightRectSize, -HighlightRectSize, HighlightRectSize, HighlightRectSize));
        break;
    default:
        break;
    }
}

