#include "jsontablemodel.h"

JsonTableModel::JsonTableModel(QObject *parent)
    : QAbstractTableModel{parent}
{

}

bool JsonTableModel::setJson(const QJsonDocument &json)
{
    beginResetModel();

    m_jsonData = QJsonArray();
    m_dataTypes.clear();
    m_colOrder.clear();

    const QJsonObject& obj = json.object();
    m_jsonData = obj["data"].toArray();

    const QJsonObject& metaObj = obj["metadata"].toObject();

    if(!convertDataTypes(metaObj["datatypes"].toObject()))
        return false;

    if(!convertOrder(metaObj["order"].toArray()))
        return false;

    if(m_colOrder.isEmpty())
    {
        for(QJsonObject::const_iterator it = metaObj.constBegin(); it != metaObj.constEnd(); ++it)
            m_colOrder << it.key();
    }

    endResetModel();
    return true;
}

//QJsonObject JsonTableModel::getJson()
//{
//    QJsonObject metadataObj;
//    metadataObj["datatypes"] = QJsonObject::fromVariantMap(revertDataTypes());
//    QJsonArray orderArray;
//    orderArray.fromStringList(m_colOrder);
//    metadataObj["order"] = orderArray;
//    QJsonObject obj;
//    obj["metadata"] = metadataObj;
//    obj["data"] = m_jsonData;

//    return obj;
//}

QJsonObject JsonTableModel::getJsonObject(const QModelIndex &index) const
{
    return m_jsonData[index.row()].toObject();
}

QVariant JsonTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    switch(role)
    {
    case Qt::DisplayRole:
    {
        switch( orientation )
        {
        case Qt::Horizontal:
        {
            QString index = m_colOrder[section];

            if(m_prettyName.contains(index))
                return m_prettyName[index];
            else
                return index;
        }
        case Qt::Vertical:
            return section + 1;
        default:
            return QVariant();
        }
    }
    default:
        return QVariant();
    }
}

int JsonTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return m_jsonData.size();
}

int JsonTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)

    return m_colOrder.size();
}

QVariant JsonTableModel::data(const QModelIndex &index, int role) const
{
    switch( role )
    {
    case Qt::DisplayRole:
    {
        QString idx = m_colOrder[index.column()];
        QJsonValue v = getJsonObject( index )[idx];
        return convertHelper(v.toVariant(), m_dataTypes[idx]);
    }
    case Qt::ToolTipRole:
        return QVariant();
    default:
        return QVariant();
    }
}

QVariant JsonTableModel::convertHelper(const QVariant & value, DataTypes type) const
{
    switch (type) {
    case Boolean:
        return value.toBool();
        break;
    case Integer:
        return value.toLongLong();
        break;
    case Floating:
        return value.toDouble();
        break;
    case Character:
        return value.toString().isEmpty() == true ? QChar() : value.toString()[0];
        break;
    case String:
        return value.toString();
        break;
    case Binary:
        return value.toByteArray();
        break;
    case Uuid:
        return value.toUuid();
        break;
    case Date:
        return value.toDate();
        break;
    case Time:
        return value.toTime();
        break;
    case Timestamp:
        return value.toDateTime();
        break;
    default:
        return value.toString();
        break;
    }
}

bool JsonTableModel::convertDataTypes(const QJsonObject &dataType)
{
    for(QJsonObject::const_iterator it = dataType.constBegin(); it != dataType.constEnd(); ++it)
        m_dataTypes[it.key()] = convertData(it.value().toString());

    return true;
}

bool JsonTableModel::convertOrder(const QJsonArray &arr)
{
    for(QJsonArray::const_iterator it = arr.constBegin(); it != arr.constEnd(); ++it)
        m_colOrder << it->toString();

    return true;
}

QMap<JsonTableModel::col_index_t, QVariant> JsonTableModel::revertDataTypes() const
{
    QMap<col_index_t, QVariant> map;
    for(QMap<col_index_t, DataTypes>::const_iterator it = m_dataTypes.constBegin(); it != m_dataTypes.constEnd(); ++it)
        map[it.key()] = revertData(it.value());

    return map;
}

JsonTableModel::DataTypes JsonTableModel::convertData(const QString &dataType)
{
    if(dataType == "boolean")
        return Boolean;
    if(dataType == "integer")
        return Integer;
    if(dataType == "floating")
        return Floating;
    if(dataType == "character")
        return Character;
    if(dataType == "string")
        return String;
    if(dataType == "binary")
        return Binary;
    if(dataType == "uuid")
        return Uuid;
    if(dataType == "date")
        return Date;
    if(dataType == "time")
        return Time;
    if(dataType == "timestamp")
        return Timestamp;

    return String;
}

QString JsonTableModel::revertData(DataTypes dataType)
{
    switch(dataType)
    {
    case Boolean:
        return "boolean";
        break;
    case Integer:
        return "integer";
        break;
    case Floating:
        return "floating";
        break;
    case Character:
        return "character";
        break;
    case String:
        return "string";
        break;
    case Binary:
        return "binary";
        break;
    case Uuid:
        return "uuid";
        break;
    case Date:
        return "date";
        break;
    case Time:
        return "time";
        break;
    case Timestamp:
        return "timestamp";
        break;
    }
    return "string";
}

