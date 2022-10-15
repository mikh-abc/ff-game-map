#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QWidget>
#include <QSharedPointer>

class GameMap;

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

        bool greens = false;
        bool herbs = false;
        bool roots = false;
        bool willow = false;

        bool animals = false;
        bool animalsSpawns = false;
        bool enemies = false;
        bool buildings = false;

        uint fertility = 0;
        uint fooder = 0;
        uint water = 0;
    };

    void setScale(float v);
    void setHighlightMouse(bool v);
    void addHighlight(const QPoint& position);
    void resetHighlight();
    void update(const DrawOptions& opt, const QSharedPointer<GameMap>& map);
    void clear();

signals:
    void clicked(const QPointF& position);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void leaveEvent(QEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private:
    void widgetUpdate(const QPoint& p);

    QFuture<QPixmap> future_;
    QPixmap mapImage_;
    float scale_;

    struct HighlightMouse {
        bool enabled = false;
        bool show = false;
        QPoint p;
    };
    HighlightMouse highlightMouse_;
    std::vector<QPoint> highlight_;
};

#endif // MAPWIDGET_H
