// Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
// in compliance with the License.  You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software distributed under the License
// is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
// or implied.  See the License for the specific language governing permissions and limitations
// under the License.

#include "VillagerModel.h"
#include "StaticData.h"

struct VillagerModelComparator
{
    VillagerModelComparator(int c, Qt::SortOrder o) : column(c), order(o) {}

    bool operator()(const VillagerData& a, const VillagerData& b) const
    {
        switch (column)
        {
        case 0:
            if (order == Qt::DescendingOrder)
                return a.name > b.name;
            else
                return a.name < b.name;
        case 1:
            if (order == Qt::DescendingOrder)
                return a.year == b.year ? a.month > b.month : a.year > b.year;
            else
                return a.year == b.year ? a.month < b.month : a.year < b.year;
        case 2:
            if (order == Qt::DescendingOrder)
                return a.occupation > b.occupation;
            else
                return a.occupation < b.occupation;
        case 3:
            if (order == Qt::DescendingOrder)
                return a.diseases.size() > b.diseases.size();
            else
                return a.diseases.size() < b.diseases.size();
        case 4:
            if (order == Qt::DescendingOrder)
                return a.education > b.education;
            else
                return a.education < b.education;
        case 5:
            if (order == Qt::DescendingOrder)
                return a.veterancyKills > b.veterancyKills;
            else
                return a.veterancyKills < b.veterancyKills;
        }
        return false;
    }
    int column;
    Qt::SortOrder order;
};

VillagerModel::VillagerModel(std::vector<VillagerData>&& data, uint32_t startYear)
    : data_(std::move(data))
    , startYear_(startYear)
{}

int VillagerModel::columnCount(const QModelIndex &parent) const
{
    return 6;
}

int VillagerModel::rowCount(const QModelIndex &parent) const
{
    return data_.size();
}

QVariant VillagerModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    static const QStringList headers = QStringList() << tr("Name") << tr("Birth Date") << tr("Occupation") << tr("Diseases") << tr("Education") << tr("Kills");


    if (role == Qt::DisplayRole)
        return headers[section];
    return QVariant();
}

QVariant VillagerModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();
    int row = index.row();
    switch (role)
    {
    case Qt::TextAlignmentRole:
    {
        return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
        //else
            //return QVariant(Qt::AlignRight | Qt::AlignVCenter);
    }
    case Qt::DisplayRole:
    {
        if (row >= data_.size()) return QVariant();
        const VillagerData& villager = data_[row];
        int column = index.column();
        switch (column)
        {
        case 0:
            return villager.name;
        case 1:
            return QString("%1.%2").arg(villager.month, 2, 10, QChar(u'0')).arg(villager.year);
        case 2:
            return occupationStr(villager.occupation);
        case 3:
            return QVariant();
        case 4:
            return educationStr(villager.education);
        case 5:
            return villager.veterancyKills;
        }
    }
    case Qt::DecorationRole:
    {
        if (row >= data_.size()) return QVariant();
        const VillagerData& villager = data_[row];
        int column = index.column();
        switch (column)
        {
        case 3:
            if (villager.diseases.size() > 0) {
                return QPixmap(":/res/Biohazard.svg").scaled(16, 16);
            }
            break;
        }
    }
    }
    return QVariant();
}

void VillagerModel::sort(int column, Qt::SortOrder order)
{
    beginResetModel();
    std::sort(data_.begin(), data_.end(), VillagerModelComparator(column, order));
    endResetModel();
}

QModelIndex VillagerModel::index(int row, int column, const QModelIndex& parent) const
{
    return hasIndex(row, column, parent) ? createIndex(row, column) : QModelIndex();
}

QModelIndex VillagerModel::parent(const QModelIndex& child) const
{
    return QModelIndex();
}
