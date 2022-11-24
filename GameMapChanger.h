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
        QByteArray name;
        int pacifist = 1;
    };

    explicit GameMapChanger(const Options& options);

    bool copy(const QString& from, const QString& to, const std::vector<MineralData>& addMinerals);

signals:

private:
    Options options_;

    void handleMinerals(QByteArray& buf, const std::vector<MineralData>& addMinerals);
    void handleMetaData(QByteArray& buf);
};

#endif // GAMEMAPCHANGER_H
