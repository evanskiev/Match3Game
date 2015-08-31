#ifndef MATCH3MODEL_H
#define MATCH3MODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QVector>
#include <QTimer>
#include "element.h"

class Match3Model : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int columns READ getWidth NOTIFY columnsChanged)
    Q_PROPERTY(int rows READ getHeight NOTIFY rowsChanged)
    Q_PROPERTY(int score READ getScore NOTIFY scoreChanged)
    Q_PROPERTY(int minScore READ getMinScore NOTIFY minScoreChanged)
    Q_PROPERTY(int maxMoves READ getMaxMoves NOTIFY maxMovesChanged)
    Q_PROPERTY(int moveCounter READ getMoveCounter NOTIFY moveCounterChanged)

private:
    QVector<Element> m_listOfElements;
    int m_width;
    int m_height;
    QVector<int> m_types;
    QVector<int> m_matches;
    int m_elementScore;
    int m_minScore;
    int m_maxMoves;
    int m_currentIndex;
    int m_score;
    int m_moveCounter;
    QTimer *timer;

private:
    void fileParser();
    void createGameGrid();

    void move(int from, int to);

    void findHorizontalMatches(int index);
    void findVerticalMatches(int index);
    void removeMatches();
    void elementShift();

    void shiftAboveElement(int index);
    bool isFullGrid();
    void checkForVictory();
    bool isMoved();
    void moveIncrement();
    void scoreIncrement();
    void scoreReset();
    void checkForMoveRestore(int from, int to);

    void delay(int n);

public:
    explicit Match3Model(QObject *parent = nullptr);

    enum Roles {
        TypeRole = Qt::UserRole + 1,
        PathRole = Qt::UserRole + 2,
    };

    virtual int rowCount(const QModelIndex &parent) const;
    virtual QVariant data(const QModelIndex &index, int role) const;

    QHash<int, QByteArray> roleNames() const;

    Q_INVOKABLE void activate(int i);

    int getWidth() const;
    int getHeight() const;
    int getScore() const;
    int getMinScore() const;
    int getMaxMoves() const;
    int getMoveCounter() const;

signals:
    void rowsChanged();
    void columnsChanged();
    void scoreChanged();
    void minScoreChanged();
    void maxMovesChanged();
    void moveCounterChanged();
    void victory();
    void defeat();

public slots:
    void newGame();
    void fillGameGrid();
    void findMatches();
};

#endif // MATCH3MODEL_H
