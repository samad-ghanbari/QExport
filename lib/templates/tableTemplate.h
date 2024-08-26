#ifndef TABLETEMPLATE_H
#define TABLETEMPLATE_H

#include <QJsonObject>
#include <QJsonArray>
#include "jsontable.h"
#include <QStringList>
#include <QByteArray>

class TableTemplate
{
public:
    explicit TableTemplate(int _columnCount ) :
        table("#000", "#FFF","tahoma",14), title("#000", "#EEE","Tahoma",14)
    {
        columnCount = _columnCount;
    }

    bool appendTitle(QStringList types, QStringList values, QString name, QStringList colors, QStringList aligns)
    {
        if(types.size() != values.size())
            return false;
        if(types.size() != colors.size())
            return false;
        if(aligns.size() != colors.size())
            return false;

        QJsonArray row;
        QJsonObject style;
        for(int i = 0; i < types.size(); i++)
        {
           style = title.createStyle(name, 0, 0, colors[i], "#EEE", "Tahoma", 16, true, aligns[i], 1 );

            QJsonObject obj = title.createObject(types[i], values[i], style);
            row = title.addObjectToRow(row, obj);
        }

        title.addRowToTable(row);
        return true;
    }

    bool appendTitle(QStringList types, QStringList values, QString name, QList<double> widths, QList<double> heights, QStringList colors, QStringList aligns)
    {
        if(types.size() != values.size())
            return false;
        if(types.size() != colors.size())
            return false;
        if(aligns.size() != colors.size())
            return false;
        if(colors.size() != widths.size())
            return false;
        if(widths.size() != heights.size())
            return false;

        QJsonArray row;
        QJsonObject style;
        for(int i = 0; i < types.size(); i++)
        {
            style = title.createStyle(name, 0, 0, colors[i], "#EEE", "Tahoma", 16, true, aligns[i], 1 );

            QJsonObject obj = title.createObject(types[i], values[i], style);
            row = title.addObjectToRow(row, obj);
        }

        title.addRowToTable(row);
        return true;
    }

    QJsonArray getTitle(double width, bool justifyWidth = true)
    {
        if(justifyWidth)
        {
            title.updateFairCell(width, true); // updates width and height
            title.updateRowSpan(false);
        }
        else
        {
            title.updateSameWidth(width);
            title.updateHeight();
            title.updateRowSpan(false);
        }


        return title.table;
    }

    QByteArray getTitleByteArray()
    {
        return title.toByteArray();
    }

    bool setTableHeader(QStringList headerList)
    {
        QJsonObject style = title.createStyle("Table Header", 0, 0, "#005", "#EEF", "Tahoma", 14, true, "center", 1 );

        if(headerList.size() == columnCount)
        {
            QJsonArray row = table.createObjects("text", headerList, style);
            table.addRowToTable(row);
        }
        else
            return false;

        return true;
    }

    void appentRow()
    {
        table.addRowToTable();
    }

    bool appentRow(QString name, QStringList rowList)
    {
        if(rowList.size() == columnCount)
        {
            QJsonObject style = title.createStyle(name, 0, 0, "#005", "#FFF", "Tahoma", 14, false, "left", 1 );

            QJsonArray row = table.createObjects("text", rowList, style);
            table.addRowToTable(row);
        }
        else
            return false;

        return true;
    }

    bool appendRow(QStringList types, QStringList values, QString name, QList<double> widths, QList<double> heights, QStringList colors, QStringList aligns)
    {
        if(types.size() != values.size())
            return false;
        if(types.size() != colors.size())
            return false;
        if(aligns.size() != colors.size())
            return false;
        if(colors.size() != widths.size())
            return false;
        if(widths.size() != heights.size())
            return false;

        QJsonArray row;
        QJsonObject style;
        for(int i = 0; i < types.size(); i++)
        {
            style = title.createStyle(name, widths[i], heights[i],colors[i], "#FFF", "Tahoma", 14, false, "left", 1 );

            QJsonObject obj = table.createObject(types[i], values[i], style);
            row = table.addObjectToRow(row, obj);
        }

        table.addRowToTable(row);
        return true;
    }

    void highlight(int row)
    {
        table.highlightRow(row);
    }

    QJsonArray getTable(double width, bool justifyWidth = true)
    {
        if(justifyWidth)
        {
            table.updateFairCell(width, true);
            table.updateRowSpan(false);
        }
        else
        {
            table.updateSameWidth(width);
            table.updateHeight();
            table.updateRowSpan(false);
        }

        return table.table;
    }

    QByteArray getTableByteArray()
    {
        return table.toByteArray();
    }

    int columnCount;
    JsonTable table, title;
};

#endif // TABLETEMPLATE_H
