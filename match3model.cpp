#include "match3model.h"

#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QModelIndex>
#include <QEventLoop>

Match3Model::Match3Model(QObject *parent) : QAbstractListModel(parent) {
    fileParser();

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(findMatches()));

    newGame();
}

int Match3Model::rowCount(const QModelIndex &parent) const {
    return m_listOfElements.size();
}

QHash<int, QByteArray> Match3Model::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[TypeRole] = "type";
    roles[PathRole] = "path";
    return roles;
}

void Match3Model::activate(int index) {
    if (m_currentIndex == -1) {
        m_currentIndex = index;
        return;
    }
    else {
        if (index != m_currentIndex) {
            if (qAbs(m_currentIndex - index) == 1) {
                move(m_currentIndex, index);

                checkForMoveRestore(m_currentIndex, index);
                return;
            }

            if (m_currentIndex == index - m_width) {
                for (int i = 0; i < m_width; i++) {
                    move(m_currentIndex, m_currentIndex+1);
                    m_currentIndex += 1;
                }
                m_currentIndex -= 1;

                for(int i = 0; i < m_width-1; i++) {
                    move(m_currentIndex, m_currentIndex-1);
                    m_currentIndex -= 1;
                }

                checkForMoveRestore(m_currentIndex, index);
                return;
            }

            if (m_currentIndex == index + m_width) {
                for (int i = 0; i < m_width; i++) {
                    move(m_currentIndex, m_currentIndex-1);
                    m_currentIndex -= 1;
                }
                m_currentIndex += 1;

                for(int i = 0; i < m_width-1; i++) {
                    move(m_currentIndex, m_currentIndex+1);
                    m_currentIndex += 1;
                }
                checkForMoveRestore(m_currentIndex, index);
                return;
            }
            m_currentIndex = index;
        }
    }
}

QVariant Match3Model::data(const QModelIndex &index, int role) const {
    if(!index.isValid()) {
        return QVariant();
    }
    const Element& element = m_listOfElements[index.row()];

    if(role == TypeRole) {
        return element.type();
    }

    if (role == PathRole) {
        return element.path();
    }
    return QVariant();
}


int Match3Model::getWidth() const {
    return m_width;
}

int Match3Model::getHeight() const {
    return m_height;
}

int Match3Model::getScore() const {
    return m_score;
}

int Match3Model::getMinScore() const {
    return m_minScore;
}


int Match3Model::getMaxMoves() const {
    return m_maxMoves;
}

int Match3Model::getMoveCounter() const {
    return m_moveCounter;
}

void Match3Model::newGame() {
    m_listOfElements.clear();

    beginResetModel();
    endResetModel();

    m_currentIndex = -1;
    m_score = 0;
    emit scoreChanged();

    m_moveCounter = 0;
    emit moveCounterChanged();

    createGameGrid();
    QTimer::singleShot(100, this, SLOT(fillGameGrid()));
}

void Match3Model::fileParser() {
    //    QString fileName = QFileDialog::getOpenFileName(0, tr("Open File"));

    QFile inputFile("../Match3Game/param.json");

    if (inputFile.open(QIODevice::ReadOnly)) {
        QByteArray buffer = inputFile.readAll();

        QJsonDocument document = QJsonDocument::fromJson(buffer);

        QJsonObject obj = document.object();

        m_width           = obj["width"].toInt();
        m_height          = obj["height"].toInt() + 1;
        m_elementScore    = obj["elementScore"].toInt();
        m_minScore        = obj["minScore"].toInt();
        m_maxMoves        = obj["maxMoves"].toInt();

        QJsonArray arr = obj["types"].toArray();

        for (int i = 0; i < arr.size(); i++) {
            m_types.append(arr[i].toInt());
        }
    }
}

void Match3Model::createGameGrid() {
    while (m_listOfElements.size() != m_width * m_height) {
        m_listOfElements.append(Element(-1, ""));
    }
}

void Match3Model::fillGameGrid() {
    while (!isFullGrid()) {
        for (int i = m_listOfElements.size() - m_width; i < m_listOfElements.size(); i++) {
            while (m_listOfElements[i].type() == -1) {
                int type = rand() % m_types.last();

                if (m_types.contains(type)) {
                    beginRemoveRows(QModelIndex(), i, i);
                    endRemoveRows();

                    QString path = QString("qrc:/sprites/%1.png").arg(type);

                    Element element(type, path);

                    m_listOfElements.replace(i, element);

                    beginInsertRows(QModelIndex(), i, i);
                    endInsertRows();
                }
            }
        }
        delay(200);
        elementShift();
    }
//        timer->start(250);
    findMatches();
}

void Match3Model::move(int from, int to) {
    if (to > from) {
        beginMoveRows(QModelIndex(), from, from, QModelIndex(), to+1);
    }
    else {
        beginMoveRows(QModelIndex(), from, from, QModelIndex(), to);
    }
    qSwap(m_listOfElements[from], m_listOfElements[to]);
    endMoveRows();
}

void Match3Model::findMatches() {
    for (int i = 0; i < (m_height*m_width)-m_width; i += m_width) {
        findHorizontalMatches(i);
    }

    for (int i = 0; i < m_width; i++) {
        findVerticalMatches(i);
    }

    if (m_matches.size() > 0) {
        removeMatches();
        elementShift();
        timer->start(300);
        return;
    }
    if (!isFullGrid()) {
        checkForVictory();
        fillGameGrid();
    }
}

void Match3Model::findHorizontalMatches(int index) {
    QVector<int> temp;
    temp.append(index);

    for (int i = 0; i < m_width-1; i++) {
        if (m_listOfElements[index].type() == m_listOfElements[index+1].type()
                                           && m_listOfElements[index].type() != -1) {
            temp.append(index+1);
        }
        else {
            if (temp.size() >= 3) {
                m_matches.append(temp);
            }
            temp.clear();
            temp.append(index+1);
        }
        index += 1;
    }
    if (temp.size() >= 3) {
        m_matches.append(temp);
    }
}

void Match3Model::findVerticalMatches(int index) {
    QVector<int> temp;
    temp.append(index);

    for (int i = 0; i < m_height-2; i++) {
        if (m_listOfElements[index].type() == m_listOfElements[index+m_width].type()
                                               && m_listOfElements[index].type() != -1) {
            temp.append(index+m_width);
        }
        else {
            if (temp.size() >= 3) {
                m_matches.append(temp);
            }
            temp.clear();
            temp.append(index+m_width);
        }
        index += m_width;
    }
    if (temp.size() >= 3) {
        m_matches.append(temp);
    }
}

void Match3Model::removeMatches() {
    for (int i = 0; i < m_matches.size(); i++) {
        beginRemoveRows(QModelIndex(), m_matches[i], m_matches[i]);
        endRemoveRows();

        m_listOfElements.replace(m_matches[i], Element(-1, ""));
        beginInsertRows(QModelIndex(), m_matches[i], m_matches[i]);
        endInsertRows();
    }
    scoreIncrement();

    m_matches.clear();
}

void Match3Model::elementShift() {
    for (int i = 0; i < m_width; i++) {
        int index = i;

        for (int j = 0; j < m_height - 1; j++) {
            if (m_listOfElements[index].type() == -1) {
                shiftAboveElement(index);
            }
            index += m_width;
        }
    }
}

void Match3Model::shiftAboveElement(int index) {
    int cIndex = index;
    index += m_width;

    while (index < m_width*m_height) {
        if (m_listOfElements[index].type() != -1) {
            cIndex = index - cIndex;
            for (int i = 0; i < cIndex; i++) {
                move(index, index-1);
                index -= 1;
            }
            index += 1;
            for (int i = 0; i < cIndex - 1; i++) {
                move(index, index+1);
                index += 1;
            }
            return;
        }
        index += m_width;
    }
}

bool Match3Model::isMoved() {
    for (int i = 0; i < m_height*m_width; i += m_width) {
        findHorizontalMatches(i);
    }

    for (int i = 0; i < m_width; i++) {
        findVerticalMatches(i);
    }

    if (m_matches.size() > 0) {
        m_matches.clear();
        return true;
    }
    return false;
}

void Match3Model::moveIncrement() {
    m_moveCounter += 1;
    emit moveCounterChanged();
}

void Match3Model::scoreIncrement()
{
    if (m_moveCounter > 0) {
        m_score += m_matches.size() * m_elementScore;
        emit scoreChanged();
    }
}

void Match3Model::checkForMoveRestore(int fromIndex, int toIndex) {
    if (!isMoved()) {
        if (qAbs(fromIndex - toIndex) == 1) {
            delay(200);
            move(fromIndex, toIndex);
        }

        if (fromIndex == toIndex - m_width) {
            delay(200);

            for (int i = 0; i < m_width; i++) {
                move(fromIndex, fromIndex+1);
                fromIndex += 1;
            }
            fromIndex -= 1;

            for(int i = 0; i < m_width-1; i++) {
                move(fromIndex, fromIndex-1);
                fromIndex -= 1;
            }
        }

        if (fromIndex == toIndex + m_width) {
            delay(200);
            for (int i = 0; i < m_width; i++) {
                move(fromIndex, fromIndex-1);
                fromIndex -= 1;
            }
            fromIndex += 1;

            for(int i = 0; i < m_width-1; i++) {
                move(fromIndex, fromIndex+1);
                fromIndex += 1;
            }
        }
    }
    else {
        moveIncrement();
        timer->start(300);
    }

    m_currentIndex = -1;
}

void Match3Model::delay(int n) {
    QEventLoop loop;
    QTimer::singleShot(n, &loop, SLOT(quit()));
    loop.exec();
}

bool Match3Model::isFullGrid() {
    for (Element element : m_listOfElements) {
        if (element.type() == -1) {
            return false;
        }
    }
    return true;
}

void Match3Model::checkForVictory() {
    if (m_score >= m_minScore) {
        emit victory();
        return;
    }

    if (m_moveCounter == m_maxMoves) {
        emit defeat();
    }
}

void Match3Model::scoreReset() {
    m_score = 0;
    emit scoreChanged();
}

