#ifndef COLONYMAP_H
#define COLONYMAP_H

#include <QtCore>
#include <QtGui>
#include <QWidget>

class ColonyMap : public QWidget
{
    Q_OBJECT

public:
    explicit ColonyMap(QWidget *parent = 0, uint height = 0, uint width = 0);
    ~ColonyMap();

    // events
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void paintEvent(QPaintEvent *);
    void timerEvent(QTimerEvent *);

private:
    uint mapHeight;
    uint mapWidth;
    QColor cellColor;
    QTimer *timer;
    QVector<QBitArray> *curGenMap;
    QVector<QBitArray> *nextGenMap;

    void drawGrid(QPainter &canvas);
    void drawColony(QPainter &canvas);
    void setCell(QVector<QBitArray> * map, uint row, uint col, bool val);
    bool getCell(QVector<QBitArray> * map, uint row, uint col);
    bool stillLive(uint row, uint col);

signals:

public slots:
    void setMapHeight(uint height);
    void setMapWidth(uint width);
    void nextGen();
    void setInterval(int);

};

#endif // COLONYMAP_H
