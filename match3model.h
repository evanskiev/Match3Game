#ifndef MATCH3MODEL_H
#define MATCH3MODEL_H

#include <QObject>
#include <QAbstractListModel>
#include <QVector>
#include <QSet>
#include "element.h"

class Match3Model : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int columns READ getWidth)
    Q_PROPERTY(int rows READ getHeight)

private:
    QVector<Element> listOfElements;
    int width;
    int height;
    QVector<int> types;
    QVector<int> matches;
    int elementScore;
    int minScore;
    int maxMoves;
    int currentIndex;
    int score;

    void fileParser();
    void createGameGrid();
    void fillGameGrid();
    void move(int from, int to);
    void findMatches();
    void findHorizontalMatches(int index);
    void findVerticalMatches(int index);
    void removeMatches();
    void elementShift();
    void shiftAboveElement(int index);
    bool isMoved();
    bool isFullGrid();

public:
    explicit Match3Model(QObject *parent = 0);

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

signals:

public slots:
};

#endif // MATCH3MODEL_H
