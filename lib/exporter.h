#ifndef EXPORTER_H
#define EXPORTER_H

#include <QObject>
#include <QJsonArray>

class TableTemplate;

class Exporter : public QObject
{
    Q_OBJECT
public:
    explicit Exporter(TableTemplate *_tableTemplate, QObject *parent = nullptr);
    void pdf(QString outputPath, int rowSpanColIndex, bool fairWidth,  QString Creator, QString DocName, QString pageSize="A3", QString orientation="landscape", qreal pageMarginLeft=18, qreal pageMarginTop=5, qreal pageMarginRight=18, qreal pageMarginBottom=5);
    void excel(QString outputPath, bool fairWidth=true, QList<int> repeatedRows={0}, bool skipImages = false);
    void prepareTables(int rowSpanColIndex=-1, bool justifyWidth=true);

    QJsonArray title, table;
    double width;
    TableTemplate *tableTemplate;
};

#endif // EXPORTER_H
