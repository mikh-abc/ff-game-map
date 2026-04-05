// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software distributed under the License
// is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
// or implied.  See the License for the specific language governing permissions and limitations
// under the License.

#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QWidget>
#include <QSharedPointer>

class DataReader;

class MapWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MapWidget(QWidget *parent = nullptr);
    ~MapWidget();

    struct DrawOptions
    {
        bool sand = false;
        bool clay = false;
        bool coal = false;
        bool iron = false;
        bool gold = false;
        bool stone = false;

        bool greens = false;
        bool herbs = false;
        bool roots = false;
        bool willow = false;

        bool animals = false;
        bool animalsSpawns = false;
        bool enemies = false;
        bool buildings = false;

        uint fertility = 0;
        uint fodder = 0;
        uint water = 0;

        bool heightMap = false;
        float heightMin = 0;
        float heightMax = 0;
    };

    enum class HighlightType {
        Empty,
        Circle,
        Rect
    };

    void setScale(float v);
    void startHighlightMouse(HighlightType type);
    void finishHighlightMouse();
    void addHighlight(const QPoint& position, QColor pen, QColor brush);
    void resetHighlight();
    void cancelHighlight();
    void update(const DrawOptions& opt, const QSharedPointer<DataReader>& map);
    void clear();

signals:
    void clicked(const QPointF& position);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    void widgetUpdate(const QPoint& p, HighlightType type);

    QFuture<QPixmap> future_;
    QPixmap mapImage_;
    float scale_;

    struct HighlightMouse {
        bool enabled = false;
        bool show = false;
        bool added = false;
        HighlightType type = HighlightType::Empty;
        QPoint p;
    };
    HighlightMouse highlightMouse_;

    struct HighlightColor {
        QPoint p;
        QColor pen;
        QColor brush;
    };

    std::vector<HighlightColor> highlightCircles_;
    std::vector<HighlightColor> highlightRects_;
};

#endif // MAPWIDGET_H
