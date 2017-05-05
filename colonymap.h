#ifndef COLONYMAP_H
#define COLONYMAP_H

#include <QtCore>
#include <QtGui>
#include <QWidget>

class ColonyMap : public QWidget
{
    Q_OBJECT

public:
    typedef QBitArray MapRow;
    typedef QVector<MapRow> Map;

    explicit ColonyMap(
        QWidget *parent = 0,
        int size = 30,
        int interval = 100);
    ~ColonyMap();

    bool isRunning();
    bool isEmpty();
    void setGenerationCount(int);

protected:
    // events
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void paintEvent(QPaintEvent *);
    void resizeEvent(QResizeEvent *e);

private:    
    QTimer *timer;

    int mapSize;
    QColor cellColor;
    Map *curGenMap;
    Map *nextGenMap;
    Map *visitedCells;
    int generationCount;

    // for storing initial values of loaded map
    Map *originalMap;
    Map *originalVisited;
    int originalCount;

    bool running;
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
    void gameRunning(bool);
    void gamePaused();
    void gameResumed();
    void colorChanged(const QColor &);
    void mapSaved();
    void mapLoaded();
    void brushInverting(bool);
    void brushErasing(bool);
    void mapCleaned();
    void generationLived(int);

public slots:
    void setMapSize(int size);
    void nextGen();
    void setInterval(int);
    void gameStart();
    void gameStop();
    void gamePause();
    void gameResume();
    void gameReset();
    void chooseCellColor();
    void setCellColor(const QColor&);
    void saveMap();
    void loadMap();
    void cleanMap();
};

#endif // COLONYMAP_H
