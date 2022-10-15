#ifndef GAMEMAPCHANGER_H
#define GAMEMAPCHANGER_H

#include <vector>

#include "DataDefines.h"

class GameMapChanger
{

public:
    struct Options
    {
        bool removeFoW = false;
        bool removeBuildingSites = false;
        bool doubleMinerals = false;
    };

    explicit GameMapChanger(const Options& options);

    bool copy(const QString& from, const QString& to, const std::vector<MineralData>& addMinerals);

signals:

private:
    Options options_;

    void handleMinerals(QByteArray& buf, const std::vector<MineralData>& addMinerals);

};

#endif // GAMEMAPCHANGER_H
