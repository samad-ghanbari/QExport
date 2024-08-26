#include "jsontable.h"
#include <QString>
#include <QFile>
#include <QByteArray>
#include <QJsonParseError>
#include <QFontMetrics> // gui config
#include <QPixmap>
#include <QtMath>

JsonTable::JsonTable(QString _default_color, QString _default_background_color, QString _default_font_family, double _default_font_size, int _default_hPadding, int _default_vPadding, QObject *parent )
    : QObject{parent}
{
    this->default_height = 0;
    this->default_background_color = _default_background_color;
    this->default_color = _default_color;
    this->default_font_size = _default_font_size;
    this->default_font_family = _default_font_family;
    this->default_hPadding = _default_hPadding;
    this->default_vPadding = _default_vPadding;
}

QJsonObject JsonTable::createStyle(QString _name, double _width, double _height, QString _color, QString _backgroundColor, QString _fontFamily, double _fontSize, bool _bold, QString _align, int _border, int _hPadding, int _vPadding, int _rowSpan)
{
    QJsonObject obj;

    obj["name"] = _name;
    obj["width"] = (_width < 0)? 0 : _width;
    obj["height"] = (_height == 0)? default_height : _height;
    obj["occupy"] = -1;
    obj["background-color"] = (_backgroundColor.isNull())? default_background_color : _backgroundColor ;
    obj["color"] = (_color.isNull())? default_color : _color;
    obj["font-size"]= (_fontSize == 0)? default_font_size : _fontSize;
    obj["font-family"]= (_fontFamily.isNull())? default_font_family : _fontFamily;
    obj["bold"]= _bold;
    obj["align"]= _align;// left center right
    obj["border"] = _border;
    obj["h-padding"] = (_hPadding == 0)? default_hPadding : _hPadding;
    obj["v-padding"] = (_vPadding == 0)? default_vPadding : _vPadding;
    obj["row-span"] = _rowSpan; // 0:default -1:skip n>0:n-span this field will be updated by rowSpanAnalyser
    return obj;
}

QJsonObject JsonTable::createObject(QString _type, QString _value, QJsonObject _style)
{
    QJsonObject obj;
    obj["type"] = _type;
    obj["value"] = _value;
    obj["style"] = _style;
    // update occupy
    obj = updateOccupy(obj);

    return obj;
}

QJsonArray JsonTable::createObjects(QString _type, QStringList _values, QJsonObject _style)
{
    QJsonArray array;
    QJsonObject obj;
    for(int i=0 ; i < _values.size() ; i++  )
    {
        obj["type"] = _type;
        obj["value"] = _values[i];
        obj["style"] = _style;
        // update occupy
        obj = updateOccupy(obj);

        array.append(obj);
    }

    return array;
}

QJsonArray JsonTable::addObjectToRow(QJsonArray &row, QJsonObject item)
{
    QJsonObject style = item["style"].toObject();
    QString type = item["type"].toString();
    double  height = style.value("height").toDouble();
    if(type.compare("img", Qt::CaseInsensitive) == 0)
    {
        if(height == 0)
        {
            QPixmap img(item.value("value").toString());
            height = img.size().height();

            style["height"] = height;

            item.remove("style");
            item["style"] = style;
        }
    }
    row.append(item);
    return row;
}

QJsonArray JsonTable::addArrayToRow(QJsonArray &row, QJsonArray array)
{
    for(int i=0; i<array.count(); i++)
    {
        row.append(array[i].toObject());
    }

    return row;
}

void JsonTable::addRowToTable(QJsonArray &row)
{
    table.append(row);
}

void JsonTable::addRowToTable()
{
    // add empty row
    QJsonArray row;
    table.append(row);
}

bool JsonTable::isEmptyArray(int row)
{
    QJsonArray Row = table[row].toArray();
    if(Row.count()==0)
        return true;
    else
        return false;
}

QJsonArray JsonTable::emptyJsonArray(QJsonArray &array)
{
    while(array.count() > 0)
        array.removeAt(0);

    return array;
}

QByteArray JsonTable::toByteArray()
{
    QJsonDocument doc(table);
    QByteArray bytes = doc.toJson(QJsonDocument::Indented);
    return bytes;
}

QByteArray JsonTable::toByteArray(QJsonObject obj)
{
    QJsonDocument doc(obj);
    QByteArray bytes = doc.toJson(QJsonDocument::Indented);
    return bytes;
}

QByteArray JsonTable::toByteArray(QJsonArray array)
{
    QJsonDocument doc(array);
    QByteArray bytes = doc.toJson(QJsonDocument::Indented);
    return bytes;
}

void JsonTable::setAlternativeRows(QString background1, QString background2, int startRow)
{
    QString bg;
    QJsonArray Row;
    for(int row=startRow; row < table.size(); row++)
    {
        if(row%2 == 0)
            bg = background1;
        else
            bg = background2;

        Row = table[row].toArray();
        Row = updateStyle(Row, "background-color", bg);

        table.removeAt(row);
        table.insert(row, Row);
    }
}

void JsonTable::highlightRow(int row, QString highlightColor)
{
    QJsonArray Row = table[row].toArray();
    Row = updateStyle(Row, "background-color", highlightColor);

    table.removeAt(row);
    table.insert(row, Row);
}

void JsonTable::highlightRow(QList<int> rows, QString highlightColor)
{
    QJsonArray Row;
    for(int r=0; r< rows.size(); r++)
    {
        Row = table[rows[r]].toArray();
        Row = updateStyle(Row, "background-color", highlightColor);
        table.removeAt(rows[r]);
        table.insert(rows[r], Row);
    }
}

bool JsonTable::saveJsonAs(QString fileName)
{
    QFile file(fileName);
    if(!file.open(QIODevice::WriteOnly))
        return false;

    QJsonDocument doc(table);
    QByteArray bytes = doc.toJson(QJsonDocument::Indented);

    file.write(bytes);
    file.close();
    return true;
}

bool JsonTable::loadJson(QString fileName)
{
    QFile file(fileName);
    if(!file.open(QIODevice::ReadOnly))
        return false;

    QByteArray bytes = file.readAll();

    QJsonParseError parsError;
    QJsonDocument doc = QJsonDocument::fromJson(bytes, &parsError);

    if(parsError.error != QJsonParseError::NoError)
        return false;

    table = doc.array();
    return true;
}

float JsonTable::getRowMaxHeight(QJsonArray Row)
{
    double maxHeight = 0, height;
    int vPadding = 0;
    QJsonObject item, style;
    QString type;
    for (int i=0; i < Row.count(); i++)
    {
        item = Row.at(i).toObject();
        style = item.value("style").toObject();
        type = style["type"].toString();
        height = style.value("height").toDouble();
        if( (height == 0) && (type.compare("img", Qt::CaseInsensitive) == 0) )
        {
            QPixmap img(item.value("value").toString());
            height = img.size().height();
        }
        if(maxHeight < height)
        {
            maxHeight = height;
            vPadding = style.value("v-padding").toInt();
        }
    }

    return maxHeight + 2 * vPadding;
}

QJsonObject JsonTable::updateStyle(QJsonObject _object, QString _key, QString _val)
{
    QJsonObject style;
    if(_object.contains("style"))
    {
        style = _object["style"].toObject();
        if( style.contains(_key) )
                  style[_key] = _val;

        _object["style"] = style;
    }

    return _object;
}

QJsonObject JsonTable::updateStyle(QJsonObject _object, QString _key, double _val)
{
    QJsonObject style;
    if(_object.contains("style"))
    {
        style = _object["style"].toObject();
        if( style.contains(_key) )
                  style[_key] = _val;

        _object["style"] = style;
    }

    return _object;
}

QJsonArray JsonTable::updateStyle(QJsonArray row, QString key, QString val)
{
    QJsonObject item;
    for(int i=0; i < row.count(); i++)
    {
        item = this->updateStyle(row.at(i).toObject(), key , val);
        row.removeAt(i);
        row.insert(i, item);
    }

    return row;
}

QJsonArray JsonTable::updateStyle(QJsonArray row, QString key, double val)
{
    QJsonObject item;
    for(int i=0; i < row.count(); i++)
    {
        item = this->updateStyle(row.at(i).toObject(), key , val);
        row.removeAt(i);
        row.insert(i, item);
    }

    return row;
}

void JsonTable::updateHeight()
{
    double maxHeight;
    QJsonArray row;
    for(int i = 0; i < table.count(); i++ )
    {
        maxHeight = this->getRowMaxHeight(table[i].toArray());
        row = this->updateStyle(table[i].toArray(), "height", maxHeight);
        table.removeAt(i);
        table.insert(i, row);
    }
}

void JsonTable::updateHeight(int row, int column, double height)
{
    QJsonArray Row = table[row].toArray();
    QJsonObject obj = Row[column].toObject();
    obj = updateStyle(obj, "height", height);

    Row.removeAt(column);
    Row.insert(column, obj);

    table.removeAt(row);
    table.insert(row, Row);
}

void JsonTable::updateWidth(int row, int column, double width)
{
    QJsonArray Row = table[row].toArray();
    QJsonObject obj = Row[column].toObject();
    obj = updateStyle(obj, "width", width);

    Row.removeAt(column);
    Row.insert(column, obj);

    table.removeAt(row);
    table.insert(row, Row);
}

double JsonTable::getHeight(int startRow, int endRow)
{
    double height=0;
    QJsonArray row;
    QJsonObject obj;

    int lastRow = table.count() -1 ;
    if(endRow > lastRow) endRow = lastRow;

    for(int i=startRow;  i <= endRow; i++)
    {
        row = table[i].toArray();
        if(row.count() > 0)
        {
            obj = row[0].toObject();
            height += obj["style"].toObject()["height"].toDouble();
        }
    }

    return height;
}

QJsonObject JsonTable::getObject(int row, int column)
{
    QJsonObject obj = table[row].toArray()[column].toObject();
    return obj;
}

QString JsonTable::getType(int row, int column)
{
    QJsonObject obj = table[row].toArray()[column].toObject();
    QString type = obj.value("type").toString();
    return type;
}

QString JsonTable::getValue(int row, int column)
{
    QJsonObject obj = table[row].toArray()[column].toObject();
    QString value = obj.value("value").toString();
    return value;
}

QJsonObject JsonTable::getStyle(int row, int column)
{
    QJsonObject obj = table[row].toArray()[column].toObject();
    obj = obj.value("style").toObject();
    return obj;
}

int JsonTable::getRowSpan(int row, int column)
{
    QJsonObject obj = table[row].toArray()[column].toObject();
    int value = obj["style"].toObject()["row-span"].toInt();
    return value;
}

bool JsonTable::updateRowSpan(int row, int column, int rowSpan)
{
    QJsonArray array = table[row].toArray();
    QJsonObject obj = array[column].toObject();
    QJsonObject style = obj["style"].toObject();
    style["row-span"] = rowSpan;
    obj["style"] = style;
    array.removeAt(column);
    array.insert(column, obj);
    table.removeAt(row);
    table.insert(row,array);

    int span = table[row].toArray()[column].toObject()["style"].toObject().value("row-span").toInt();
    if(span == rowSpan) return true; else return false;
}

bool JsonTable::updateRowSpan(int row, int column, bool SPAN)
{
    int span = 1;
    bool res = true;
    QString baseValue = this->getValue(row, column);
    int baseSpan = this->getRowSpan(row, column);
    if(baseSpan != 0) return res;

    if(isEmptyArray(row)) return true;

    if( (row == (table.count()-1)) || !SPAN )
    {
        res = res && updateRowSpan(row, column, span);
        return res;
    }

    QString tempValue;
    for(int r=row+1; r<table.count(); r++)
    {
        if(table[r].toArray().count() >= column)
        {
            tempValue = getValue(r, column);
            if(baseValue.compare(tempValue, Qt::CaseSensitive) == 0)
            {
                span++;
                res = res && updateRowSpan(row, column, span);
                res = res && updateRowSpan(r, column, -1);
            }
            else
            {
                res = res && updateRowSpan(row, column, span);
                break;
            }
        }
        else
        {
            res = res && updateRowSpan(row, column, span);
            break;
        }
    }

    return res;
}

bool JsonTable::updateRowSpan(int row, bool SPAN)
{
    QJsonArray Row = table[row].toArray();
    bool res = true;
    for(int i=0; i<Row.count(); i++)
    {
        res = res && updateRowSpan(row,i, SPAN);
    }
    return res;
}

bool JsonTable::updateRowSpan(bool SPAN)
{
    bool res = true;
    for(int r=0; r<table.count(); r++)
    {
        res = res && updateRowSpan(r, SPAN);
    }
    return res;
}

bool JsonTable::updateRowSpan(int ColumnIndex)
{
    bool res = true;
    for(int r=0; r<table.count(); r++)
    {
        res = res && updateRowSpan(r,ColumnIndex);
    }
    return res;
}

void JsonTable::updateSameWidth(double viewPortWidth)
{
    QJsonArray row;
    QJsonObject obj;
    QList<int> zeroIndexes;
    double width, usedWidth;
    int rowCount;
    for(int r=0; r < table.count(); r++)
    {
        row = table[r].toArray();
        zeroIndexes.clear();
        usedWidth = 0;
        rowCount = row.count();
        // one row
        for(int o=0; o < row.count(); o++)
        {
            obj = row[o].toObject();
            width = obj["style"].toObject()["width"].toDouble();
            if(width <= 0)
                zeroIndexes << o;
            else
                usedWidth += width;
        }

        int zeroLen = zeroIndexes.length();
        if(zeroLen == rowCount)
        {
            // width all zero-width object
            // same width
            width = viewPortWidth / rowCount;
            //update row object
            updateWidth(r,width);
        }
        else if(zeroLen < rowCount)
        {
            double leftWidth = viewPortWidth - usedWidth;
            double width = leftWidth / zeroLen;
            updateWidth(r,zeroIndexes, width);
        }
    }
}

void JsonTable::updateFairCell(double viewPortWidth, bool wrapAll)
{
    // checks width and height
    // * given width and hight    : no change
    // * given width and height=0 : hight calculation - for wrap
    // * width=0 and given hight  : width = max possible occupy
    // * width=0 and hight=0      : fine width then hight calculation
    // wrapAll : wrap cells even if they are with given height(increase hight)

    QJsonArray Row;
    QJsonObject Obj;
    QMap<QString, double> who; // width-height-occupy
    double width, height, occupy, fontSize;
    QString type;
    bool textFlag;

    resetColumnMap(); // columnOccupy and columnWidth
    calculateColumnMap(viewPortWidth);

    for(int r=0; r < table.count(); r++)
    {
        Row = table[r].toArray();
        for(int c=0; c < Row.count(); c++)
        {
            Obj = Row[c].toObject();
            type = Obj.value("type").toString();
            textFlag = (type.compare("text", Qt::CaseInsensitive) == 0)? true: false;
            who = getWHO(Obj);
            fontSize = Obj["style"].toObject()["font-size"].toDouble();

             // * given width and hight
            if( (who.value("width") > 0 ) && (who.value("height") > 0) )
            {
                if(wrapAll)
                {
                    if(textFlag)
                    {
                         height = calculateWrapHeight(who.value("occupy"), who.value("width"), fontSize);
                         updateHeight(r, c, height);
                    }
                }
                else
                    continue;
            }
            // * given width and height=0 - TWO times wrap allowed
            if( (who.value("width") > 0 ) && (who.value("height") == 0) )
            {
                width = who.value("width");
                occupy = who.value("occupy");

                if(width < occupy)
                {
                    height = calculateWrapHeight(who.value("occupy"), width, fontSize);
                }
                else height = fontSize;
                if(textFlag)
                {
                    updateHeight(r,c, height);
                }
                continue;
            }
            // * width=0 and given hight
            if( (who.value("width") == 0 ) && (who.value("height") > 0) )
            {
                if(columnWidth.contains(c))
                    width = columnWidth[c];
                else
                {
                    occupy = who.value("occupy");
                    if(occupy <= 200)
                        width = occupy;
                    else
                        width = 200;
                }
                updateWidth(r, c, width);

                if(wrapAll)
                {
                    if(textFlag)
                    {
                        height = calculateWrapHeight(who.value("occupy"), width, fontSize);
                        updateHeight(r,c, height);
                    }
                }
            }
            // * width=0 and hight=0
            if( (who.value("width") == 0 ) && (who.value("height") == 0) )
            {
                width = 200;
                if(columnWidth.contains(c))
                    width = columnWidth.value(c);

                occupy = who.value("occupy");

                if(width < occupy)
                    height = calculateWrapHeight(who.value("occupy"), width, fontSize);
                else height = fontSize;
                if(textFlag)
                {
                    updateHeight(r, c, height);
                }
                updateWidth(r, c, width);
            }
        }
    }


    updateHeight();
}

QMap<QString, double> JsonTable::getWHO(QJsonObject obj)
{
    QJsonObject style = obj["style"].toObject();
    double width = style["width"].toDouble();
    double height = style["height"].toDouble();
    double occupy = style["occupy"].toDouble();

    QMap<QString, double> map;
    map["width"] = width;
    map["height"] = height;
    map["occupy"] = occupy;

    return map;
}

double JsonTable::getMaxOccupy(int column)
{
    double occupy = 0, temp;
    QJsonArray Row;
    QJsonObject obj;
    for(int r=0; r<table.count(); r++)
    {
        Row = table[r].toArray();
        obj = Row[column].toObject();

        temp = obj["style"].toObject()["occupy"].toDouble();

        if(temp > occupy)
            occupy = temp;
    }

    return occupy;
}

void JsonTable::updateWidth(int row, double width)
{
    QJsonArray Row = table[row].toArray();
    QJsonObject obj;
    for( int o=0; o < Row.count(); o++)
    {
        obj = Row[o].toObject();
        obj = updateStyle(obj,"width", width);
        Row.removeAt(o);
        Row.insert(o, obj);
    }

    table.removeAt(row);
    table.insert(row, Row);
}

void JsonTable::updateWidth(int row, QList<int> index, double width)
{
    QJsonArray Row = table[row].toArray();
    QJsonObject obj;
    for( int o=0; o < Row.count(); o++)
    {
        if(index.contains(o))
        {
            obj = Row[o].toObject();
            obj = updateStyle(obj,"width", width);
            Row.removeAt(o);
            Row.insert(o, obj);
        }
    }

    table.removeAt(row);
    table.insert(row, Row);
}

void JsonTable::resetColumnMap()
{
    QJsonArray Row;
    QJsonObject Obj;
    columnOccupy.clear(); // max occupy for columns
    columnWidth.clear(); // given width columns
    double occupy, width;

    for(int r=0; r < table.count(); r++)
    {
        Row = table[r].toArray();
        for(int c=0; c < Row.count(); c++)
        {
            Obj = Row[c].toObject();
            width = Obj["style"].toObject()["width"].toDouble();
            if(width > 0)
            {
                if(columnWidth.contains(c))
                {
                    if(columnWidth.value(c) < width)
                        columnWidth[c] = width;
                }
                else
                    columnWidth[c] = width;
            }
            else
            {
                occupy = calculateOccupy(Obj);
                if(columnOccupy.contains(c))
                {
                    if(columnOccupy.value(c) < occupy)
                        columnOccupy[c] = occupy;
                }
                else
                    columnOccupy[c] = occupy;
            }
        }
    }

    // columnWidth is prior to columnOccupy
    foreach (int key, columnOccupy.keys())
    {
        if(columnWidth.contains(key))
            columnOccupy.remove(key);
    }

    maxColumnOccupy = columnOccupy;
}

void JsonTable::calculateColumnMap(double viewPortWidth)
{
    double totalOccupy = 0, totalGivenWidth = 0, total;

    foreach (int key, columnWidth.keys())
    {
        totalGivenWidth += columnWidth.value(key); // fixed width
    }

    foreach (int key, columnOccupy.keys())
    {
        totalOccupy += columnOccupy.value(key); // based on calculation
    }

    total = totalOccupy + totalGivenWidth;

    if(total == viewPortWidth)
    {
        foreach (int key, columnOccupy.keys())
        {
            columnWidth[key] = columnOccupy.value(key);
        }
    }
    else if(total < viewPortWidth)
    {
        //weight column width to fill the page
        double diff = viewPortWidth - total;
        int count = columnOccupy.size();
        double increment = diff/count;
        foreach (int key, columnOccupy.keys())
        {
            columnWidth[key] = columnOccupy.value(key) + increment;
        }
    }
    else
    {
        // total > viewPort
        // step by step threshold-based
        // all columns     : if still decrease needed do it on all columns

        updateColumnOccupy(viewPortWidth, 4000, 4000); // columns bigger than 4000 contributes but floor-value is 3500
        updateColumnOccupy(viewPortWidth, 3000, 3000);
        updateColumnOccupy(viewPortWidth, 2000, 2000);
        updateColumnOccupy(viewPortWidth, 1500, 1500);
        updateColumnOccupy(viewPortWidth, 1200, 1200);
        updateColumnOccupy(viewPortWidth, 1000, 1000);
        updateColumnOccupy(viewPortWidth, 800, 800);
        updateColumnOccupy(viewPortWidth, 600, 600);
        updateColumnOccupy(viewPortWidth, 400, 400);
        updateColumnOccupy(viewPortWidth, 200, 200);
        updateColumnOccupy(viewPortWidth, 150, 150);
        updateColumnOccupy(viewPortWidth); // all columns contribute

        foreach (int key, columnOccupy.keys())
        {
            columnWidth[key] = columnOccupy.value(key);
        }

    }
}

double JsonTable::calculateWrapHeight(double occupy, double width, double fontSize)
{
    //fontSize pt
    // px = 4/3pt
    int lines = ceil(occupy / width) + 2;
    double height = lines * fontSize * 4/3;
    return height;
}

void JsonTable::updateColumnOccupy(double viewPortWidth, int upperThreshold, int breakPoint)
{
    double diff,total = 0, totalGivenWidth=0,totalOccupy=0, selectedSum=0;
    QMap<int, double> initialMap = columnOccupy;

    foreach (int key, columnWidth.keys())
    {
        totalGivenWidth += columnWidth.value(key); // fixed width
    }

    foreach (int key, columnOccupy.keys())
    {
        totalOccupy += columnOccupy.value(key); // based on calculation
        if(columnOccupy.value(key) > upperThreshold)
        {
            selectedSum += columnOccupy.value(key);
        }
    }

    total = totalOccupy + totalGivenWidth;

    diff = total - viewPortWidth;

    if(diff > 0)
    {
        double val, totalDecrease = 0, weight;
        foreach (int key, columnOccupy.keys())
        {
            if(columnOccupy.value(key) > upperThreshold)
            {
                val = columnOccupy.value(key) * (1 - diff / selectedSum);
                if(val < breakPoint)
                    val = breakPoint;

                totalDecrease += columnOccupy[key] - val;
                columnOccupy[key] = val;
            }
        }

        //justify columns based on their weight
        foreach (int key, columnOccupy.keys())
        {
            if(columnOccupy.value(key) > upperThreshold)
            {
                weight = columnOccupy.value(key) / maxColumnOccupy.value(key); // 0~1
                columnOccupy[key] = initialMap.value(key) - totalDecrease * weight;
            }
        }
    }
}

double JsonTable::calculateOccupy(QJsonObject &obj)
{
    QString fontFamily = obj["style"].toObject()["font-family"].toString();
    int fontSize = obj["style"].toObject()["font-size"].toInt();
    double width = obj["style"].toObject()["width"].toDouble();
    bool bold = obj["style"].toObject()["bold"].toBool();

    QString value = obj["value"].toString();
    QString type =  obj["type"].toString();
    if(type.compare("img", Qt::CaseInsensitive) == 0)
    {
        if(width > 0)
            return width;
        else
        {
            // find image width
            QPixmap img(value);
            width = img.size().width();
            return width;
        }
    }

    //text-value object
    QFont font(fontFamily, fontSize);
    //font.setPointSize(fontSize);
    font.setBold(bold);
    QFontMetrics fm(font);
    width = fm.horizontalAdvance(value);
    return width;
}

void JsonTable::updateOccupy(int row, int column)
{
    QJsonArray Row = table[row].toArray();
    QJsonObject obj = Row[column].toObject();

    obj = updateOccupy(obj);

    Row.removeAt(column);
    Row.insert(column, obj);

    table.removeAt(row);
    table.insert(row, Row);
}

QJsonObject JsonTable::updateOccupy(QJsonObject obj)
{
    double occupy = calculateOccupy(obj);
    QJsonObject style = obj["style"].toObject();
    QString value = obj["value"].toString();
    QString type = obj["type"].toString();

    if(type.compare("text", Qt::CaseInsensitive) == 0)
    {
        // check occupy validation
        int len = value.length();
        // consider minimum pixel count to reprtesent character
        int min = 2;
        double temp = len * min;
        if(occupy < temp)
            occupy = temp;
    }

    style["occupy"] = occupy;
    obj.remove("style");
    obj["style"] = style;

    return obj;
}



