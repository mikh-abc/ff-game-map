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
#include "GameMap.h"

namespace {

constexpr int HighlightRadius = 10;
constexpr int HighlightRect = 15;

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

void drawMap(QPromise<QPixmap>& promise, const MapWidget::DrawOptions& opt, QSharedPointer<GameMap> map, float scale)
{
    if (map.isNull()) {
        promise.addResult(QPixmap());
        return;
    }
    auto reader = map->reader();
    auto agricultureData = reader.agricultureData();
    uint imageWidth = agricultureData.worldWidth / scale;
    uint imageHeight = agricultureData.worldHeight / scale;
    constexpr float cellSize = 5;
    constexpr float areaSize = 64;
    QPixmap image(imageWidth, imageHeight);
    image.fill(Qt::white);
    QPainter p(&image);
    auto drawLevel = [&](const QColor& color, const std::vector<std::vector<float>>& array, float limit) {
        p.setPen(color);
        p.setBrush(color);
        for (int x = 0; x < array.size(); ++x) {
            for (int y = 0; y < array[x].size(); ++y) {
                if (array[x][y] > limit) {
                    p.drawRect(imageWidth - y * cellSize / scale, x * cellSize / scale, cellSize / scale, cellSize / scale);
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
        drawLevel(QColor(194, 255, 194), agricultureData.data[AgricultureInfo::EnvFertility], opt.fertility / 100.0);
    }
    if (opt.fodder)
    {
        drawLevel(QColor(128, 255, 194), agricultureData.data[AgricultureInfo::Fodder], opt.fodder / 100.0);
    }
    if (opt.animalsSpawns) {
        uint lx = imageWidth / areaSize * scale;

        for (const auto& s : reader.animalsSpawns()) {
            QColor pc;
            QColor bc;
            switch (s.type)
            {
            case BaseType::Bear:
            case BaseType::Wolf:
                pc = Qt::red;
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
            uint y = s.spawnArea / lx;
            uint x = s.spawnArea % lx + 1;
            p.drawRect(imageWidth - x * areaSize / scale, y * areaSize / scale, areaSize / scale, areaSize / scale);
        }
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
            p.drawEllipse(imageWidth - m.p.x / scale - 10, m.p.z / scale - 10, 20, 20);
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
            if (m.amount > 19)
                r = 4;
            else if (m.amount > 14)
                r = 3;
            p.setPen(Qt::black);
            p.setBrush(bc);
            p.drawEllipse(imageWidth - m.p.x / scale - r, m.p.z / scale - r, r * 2, r * 2);
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
            p.drawLine(imageWidth - m.p.x / scale, m.p.z / scale - ls, imageWidth - m.p.x / scale, m.p.z / scale + ls);
            p.drawLine(imageWidth - m.p.x / scale - ls, m.p.z / scale, imageWidth - m.p.x / scale  + ls, m.p.z / scale);
            p.setPen(borderCircleColor);
            p.setBrush(circleColor);
            p.drawEllipse(imageWidth - m.p.x / scale - r, m.p.z / scale - r, r * 2, r * 2);
        }
    }
    if (opt.enemies) {
        for (const auto& m : reader.raiders()) {
            constexpr int ls = 5;
            p.setPen(Qt::lightGray);
            p.drawLine(imageWidth - m.p.x / scale, m.p.z / scale, imageWidth - m.spawn.x / scale, m.spawn.z / scale);
            p.setPen(Qt::magenta);
            p.drawLine(imageWidth - m.p.x / scale, m.p.z / scale - ls, imageWidth - m.p.x / scale, m.p.z / scale + ls);
            p.drawLine(imageWidth - m.p.x / scale - ls, m.p.z / scale, imageWidth - m.p.x / scale  + ls, m.p.z / scale);
        }
        for (const auto& m : reader.raiders()) {
            p.setPen(Qt::black);
            p.setBrush(Qt::black);
            constexpr int r = 3;
            p.drawEllipse(imageWidth - m.p.x / scale - r, m.p.z / scale - r, r * scale, r * scale);
            if (m.type == RaiderType::BatteringRam) {
                p.setPen(Qt::magenta);
                constexpr int inner = 2;
                p.drawEllipse(imageWidth - m.p.x / scale - r, m.p.z / scale - inner, inner * scale, inner * scale);
            }
        }
    }
    if (opt.buildings) {
        for (const auto& m : reader.houses()) {
            int ls = 0;
            switch (m.type)
            {
            case BaseType::Shelter:
                ls = 3 * cellSize / scale;
                break;
            case BaseType::TownCenter:
                ls = 5 * cellSize / scale;
                break;
            default:
                continue;
            }
            p.setPen(Qt::blue);
            p.setBrush(Qt::blue);
            p.drawRect(imageWidth - m.p.x / scale - ls, m.p.z / scale - ls, ls, ls);
        }
    }
    if (opt.clay || opt.sand || opt.coal || opt.iron || opt.gold) {
        p.setPen(Qt::black);
        for (const auto& m : mineralsList) {
            if (checkMineralOption(m.type, opt)) {
                p.drawText(QRect(imageWidth - m.p.x / scale - 20, m.p.z / scale + 10, 40, 16), Qt::AlignCenter, m.deep ? QString("∞") : QString::number(m.amount));
            }
        }
    }
    {
        auto start = reader.camera();
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

void MapWidget::setHighlightMouse(bool v)
{
    highlightMouse_.enabled = v;
    setMouseTracking(v);
    if (!v && highlightMouse_.show) {
        widgetUpdate(highlightMouse_.p);
        highlightMouse_.show = false;
    }
}

void MapWidget::addHighlight(const QPoint &position)
{
    QRect br = mapImage_.rect();
    int mapWidth = br.width();
    QPoint& p = highlight_.emplace_back(QPoint(mapWidth - position.x() / scale_, position.y() / scale_));
    widgetUpdate(p);
}

void MapWidget::resetHighlight()
{
    for (const QPoint& p : highlight_) {
        widgetUpdate(p);
    }
    highlight_.clear();
}

void MapWidget::update(const DrawOptions &opt, const QSharedPointer<GameMap>& map)
{
    future_.cancel();
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
            widgetUpdate(highlightMouse_.p);
            highlightMouse_.show = false;
        }
        return;
    }
    widgetUpdate(highlightMouse_.p);
    highlightMouse_.show = true;
    highlightMouse_.p = p;
    widgetUpdate(p);
}

void MapWidget::leaveEvent(QEvent* /*event*/)
{
    if (!highlightMouse_.enabled) {
        return;
    }
    if (highlightMouse_.show) {
        highlightMouse_.show = false;
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
    painter.setPen(Qt::black);
    painter.setBrush(Qt::transparent);
    if (highlightMouse_.show) {
        painter.drawEllipse(highlightMouse_.p.x() - HighlightRadius, highlightMouse_.p.y() - HighlightRadius, HighlightRadius * 2, HighlightRadius * 2);
    }
    for (const QPoint& p : highlight_) {
        painter.drawEllipse(xo + p.x() - HighlightRadius, yo + p.y() - HighlightRadius, HighlightRadius * 2, HighlightRadius * 2);
    }
}

void MapWidget::widgetUpdate(const QPoint& p)
{
    QWidget::update(QRect(p, p).adjusted(-HighlightRect, -HighlightRect, HighlightRect, HighlightRect));
}

