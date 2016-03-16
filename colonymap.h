#ifndef COLONYMAP_H
#define COLONYMAP_H

#include <QtCore>
#include <QtGui>
#include <QWidget>

class ColonyMap : public QWidget
{
    Q_OBJECT

public:
    typedef QVector<QBitArray> Map;
    typedef QBitArray MapRow;

    explicit ColonyMap(QWidget *parent = 0, int size = 30, int interval = 100);
    ~ColonyMap();

    bool isRunning();
    bool isEmpty();

    // events
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void paintEvent(QPaintEvent *);
    void timerEvent(QTimerEvent *);
    void resizeEvent(QResizeEvent *e);

private:
    int mapSize;
    QColor cellColor;
    QTimer *timer;
    Map *curGenMap;
    Map *nextGenMap;
    Map *visitedCells;
    Map *originalMap;
    Map *originalVisited;
    bool running;
    bool brushInverts;
    QPoint lastCell;
    QPoint shadowCell;


    void drawGrid(QPainter &canvas);
    void drawColony(QPainter &canvas);
    void setCell(Map *map, int row, int col, bool val);
    bool getCell(Map *map, int row, int col);
    bool stillLive(int row, int col);
    bool mapEmpty(Map *map);

signals:
    void sizeChanged(int size);
    void intervalChanged(int interval);
    void started();
    void stopped();
    void gameRunning(bool);
    void paused();
    void unpaused();
    void colorChanged(QColor &);
    void mapSaved();
    void mapLoaded();
    void brushInverting(bool);
    void mapCleaned();

public slots:
    void setMapSize(int size);
    void nextGen();
    void setInterval(int);
    void gameStart();
    void gameStop();
    void gamePause();
    void gameResume();
    void gameReset();
    void setCellColor(QColor &newColor);
    void saveMap(QDataStream &out);
    void loadMap(QDataStream &in);
    void setBrushInverse(bool);
    void cleanMap();
};

#endif // COLONYMAP_H
