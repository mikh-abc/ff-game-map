#include "stdafx.h"
#include "GameMapChanger.h"
#include "ParseData.h"

GameMapChanger::GameMapChanger(const Options &options)
    : options_(options)
{
}

bool GameMapChanger::copy(const QString& from, const QString& to, const std::vector<MineralData>& addMinerals)
{
    QFile toFile(to);
    QFile fromFile(from);
    if (toFile.fileName() == fromFile.fileName()) {
        return false;
    }
    if (!toFile.open(QIODevice::WriteOnly) || !fromFile.open(QIODevice::ReadOnly)) {
        return false;
    }

    QDataStream in(&fromFile);
    in.setByteOrder(QDataStream::LittleEndian);
    QDataStream out(&toFile);
    out.setByteOrder(QDataStream::LittleEndian);

    while(!in.atEnd()) {
        quint8 componentType;
        in >> componentType;
        QByteArray field = readArray<quint8>(in);
        uint fieldSize;
        in >> fieldSize;
        uint id = 0;
        in >> id;
        QByteArray buf(fieldSize - 4, Qt::Uninitialized);
        in.readRawData(buf.data(), fieldSize - 4);

        bool removeField = false;
        switch (parseBaseType(id)) {
        case BaseType::FoWSystem: {
            if (options_.removeFoW) {
                for (int i = 0; i < 512; ++i) {
                    for (int j = 0; j < 512; ++j) {
                        buf[(1 + i + j * 512) * 4 + 1] = 0xff;
                        buf[(1 + i + j * 512) * 4 + 2] = 0xff;
                    }
                }
            }
            break;
        }
        case BaseType::MetaData:
            if (!options_.name.isEmpty() || options_.pacifist != 1) {
                handleMetaData(buf);
            }
            break;
        case BaseType::BuildingBuildSite:
            removeField = options_.removeBuildingSites;
            break;
        case BaseType::MineralManager: {
            if (!addMinerals.empty() || options_.doubleMinerals) {
                handleMinerals(buf, addMinerals);
            }
            break;
        }
        }
        if (!removeField) {
            fieldSize = buf.size() + 4;

            out << componentType;
            out << quint8(field.size());
            out.writeRawData(field.data(), field.size());
            out << fieldSize;
            out << id;
            out.writeRawData(buf.data(), buf.size());
        }
    }
    return true;

}

void GameMapChanger::handleMinerals(QByteArray& buf, const std::vector<MineralData> &addMinerals)
{
    QBuffer outBuf;
    outBuf.open(QIODeviceBase::WriteOnly);
    QDataStream fout(&outBuf);
    QDataStream fin(&buf, QIODeviceBase::ReadOnly);
    fout.setByteOrder(QDataStream::LittleEndian);
    fout.setFloatingPointPrecision(QDataStream::SinglePrecision);
    fin.setByteOrder(QDataStream::LittleEndian);
    fin.setFloatingPointPrecision(QDataStream::SinglePrecision);

    QByteArray tmp(1, Qt::Uninitialized);
    fin.readRawData(tmp.data(), 1);
    fout.writeRawData(tmp.data(), 1);

    auto hanleMineral = [&](std::set<MineralType> types, uint dataSize) -> uint {
        uint count;
        fin >> count;
        uint adding = 0;
        for (const auto& m : addMinerals) {
            if (types.count(m.type) != 0) {
                ++adding;
            }
        }
        fout << uint(count + adding);
        for (uint i = 0; i < count; ++i) {
            tmp.resize(dataSize);
            fin.readRawData(tmp.data(), tmp.size());
            fout.writeRawData(tmp.data(), tmp.size());
            uint amount;
            fin >> amount;
            if (options_.doubleMinerals) {
                amount *= 2;
            }
            fout << amount;
        }
        return count;
    };

    MineralType dataTypes[3] = {MineralType::Clay, MineralType::Sand, MineralType::Unknown};
    for (const auto mineralType : dataTypes) {
        hanleMineral({mineralType}, 16);
        for (const auto& m : addMinerals) {
            if (m.type == mineralType) {
                fout << m.p;
                fout << m.r;
                fout << m.amount;
                fout << m.deep;
            }
        }
    }
    uint mineralIndex = hanleMineral({MineralType::Gold, MineralType::Coal, MineralType::Iron}, 24);
    for (const auto& m : addMinerals) {
        if (m.type == MineralType::Gold || m.type == MineralType::Coal || m.type == MineralType::Iron) {
            fout << ++mineralIndex;
            fout << mineralTypeId(m.type);
            fout << m.p;
            fout << m.r;
            fout << m.amount;
            fout << m.deep;
        }
    }
    quint32 count;
    fin >> count;
    tmp.resize(count * 32 + 1);
    fin.readRawData(tmp.data(), tmp.size());
    fout << count;
    fout.writeRawData(tmp.data(), tmp.size());

    quint32 mineralCount;
    fin >> mineralCount;
    tmp.resize(mineralCount * 20);
    fin.readRawData(tmp.data(), tmp.size());
    for (const auto& m : addMinerals) {
        if (m.type == MineralType::Gold || m.type == MineralType::Coal || m.type == MineralType::Iron) {
            ++mineralCount;
        }
    }
    fout << mineralCount;
    fout.writeRawData(tmp.data(), tmp.size());
    for (const auto& m : addMinerals) {
        if (m.type == MineralType::Gold || m.type == MineralType::Coal || m.type == MineralType::Iron) {
            fout << mineralTypeId(m.type);
            fout << m.p;
        }
    }
    tmp.resize(1);
    fin.readRawData(tmp.data(), 1);
    fout.writeRawData(tmp.data(), 1);

    buf = outBuf.data();
}

void GameMapChanger::handleMetaData(QByteArray& buf)
{
    QBuffer outBuf;
    outBuf.open(QIODeviceBase::WriteOnly);
    QDataStream fout(&outBuf);
    QDataStream fin(&buf, QIODeviceBase::ReadOnly);
    fout.setByteOrder(QDataStream::LittleEndian);
    fin.setByteOrder(QDataStream::LittleEndian);

    uint readed = 5;
    QByteArray tmp(5, Qt::Uninitialized);
    fin.readRawData(tmp.data(), tmp.size());
    fout.writeRawData(tmp.data(), tmp.size());

    quint8 len;
    fin >> len;
    tmp.resize(len);
    fin.readRawData(tmp.data(), tmp.size());
    fout << len;
    fout.writeRawData(tmp.data(), tmp.size());
    readed += len + 1;

    fin >> len;
    tmp.resize(len);
    fin.readRawData(tmp.data(), tmp.size());
    fout << len;
    fout.writeRawData(tmp.data(), tmp.size());
    readed += len + 1;

    tmp.resize(4);
    fin.readRawData(tmp.data(), 4);
    fout.writeRawData(tmp.data(), 4);

    quint8 pacifist;
    fin >> pacifist;
    switch (options_.pacifist)
    {
    case 0:
        pacifist = 0;
        break;
    case 1:
        //keep value
        break;
    case 2:
        pacifist = 1;
        break;
    }
    fout << pacifist;
    readed += 5;

    if (!options_.name.isEmpty()) {
        fin >> len;
        tmp.resize(len);
        fin.readRawData(tmp.data(), tmp.size());
        fout << quint8(options_.name.size());
        fout.writeRawData(options_.name.data(), options_.name.size());
        readed += len + 1;
    }

    fout.writeRawData(buf.data() + readed, buf.size() - readed);
    buf = outBuf.data();
}
