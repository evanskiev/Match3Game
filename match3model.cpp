#include "match3model.h"

#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QModelIndex>

Match3Model::Match3Model(QObject *parent) : QAbstractListModel(parent)
{
    currentIndex = -1;
    score = 0;
    fileParser();

    createGameGrid();
    fillGameGrid();
}

int Match3Model::rowCount(const QModelIndex &parent) const
{
    return listOfElements.size();
}

QHash<int, QByteArray> Match3Model::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[TypeRole] = "type";
    roles[PathRole] = "path";
    return roles;
}

void Match3Model::activate(int i)
{
    if (currentIndex == -1)
    {
        currentIndex = i;
        return;
    }
    else
    {
        if (i != currentIndex)
        {
            if (isMoved())
            {
                if (qAbs(currentIndex - i) == 1)
                {
                    move(currentIndex, i);
                    currentIndex = -1;
                    findMatches();
                    return;
                }

                if (currentIndex == i - width)
                {
                    for (int i = 0; i < width; i++)
                    {
                        move(currentIndex, currentIndex+1);
                        currentIndex += 1;
                    }
                    currentIndex -= 1;

                    for(int i = 0; i < width-1; i++)
                    {
                        move(currentIndex, currentIndex-1);
                        currentIndex -= 1;
                    }
                    currentIndex = -1;
                    findMatches();
                    return;
                }

                if (currentIndex == i + width)
                {
                    for (int i = 0; i < width; i++)
                    {
                        move(currentIndex, currentIndex-1);
                        currentIndex -= 1;
                    }
                    currentIndex += 1;

                    for(int i = 0; i < width-1; i++)
                    {
                        move(currentIndex, currentIndex+1);
                        currentIndex += 1;
                    }
                    currentIndex = -1;
                    findMatches();
                    return;
                }
            }
        }
    }
}

QVariant Match3Model::data(const QModelIndex &index, int role) const {
    if(!index.isValid())
    {
        return QVariant();
    }
    const Element& element = listOfElements[index.row()];

    if(role == TypeRole)
    {
        return element.type();
    }
    if (role == PathRole)
    {
        return element.path();
    }
    return QVariant();
}


int Match3Model::getWidth() const
{
    return width;
}

int Match3Model::getHeight() const
{
    return height;
}
void Match3Model::fileParser()
{
    //    QString fileName = QFileDialog::getOpenFileName(0, tr("Open File"));

    QFile inputFile("../param.json");

    if (inputFile.open(QIODevice::ReadOnly))
    {
        QByteArray buffer = inputFile.readAll();

        QJsonDocument document = QJsonDocument::fromJson(buffer);

        //        qDebug() << document;

        QJsonObject obj = document.object();

        width           = obj["width"].toInt();
        height          = obj["height"].toInt();
        elementScore    = obj["elementScore"].toInt();
        minScore        = obj["minScore"].toInt();
        maxMoves        = obj["maxMoves"].toInt();

        QJsonArray arr = obj["types"].toArray();

        for (int i = 0; i < arr.size(); i++)
        {
            types.append(arr[i].toInt());
        }
    }
}

void Match3Model::createGameGrid()
{
    while (listOfElements.size() != width * height)
    {
        listOfElements.append(Element(-1, ""));
    }
}

void Match3Model::fillGameGrid()
{
    for (int i = 0; i < listOfElements.size(); i++)
    {
        while (listOfElements[i].type() == -1)
        {
            int type = rand() % types.last();

            if (types.contains(type))
            {
                beginRemoveRows(QModelIndex(), i, i);
                endRemoveRows();

                QString path = QString("qrc:/sprites/%1.png").arg(type);

                Element element(type, path);

                listOfElements.replace(i, element);

                beginInsertRows(QModelIndex(), i, i);
                endInsertRows();
            }
        }
    }
    findMatches();
}

void Match3Model::move(int from, int to)
{
    if (to > from)
    {
        beginMoveRows(QModelIndex(), from, from, QModelIndex(), to+1);
    }
    else
    {
        beginMoveRows(QModelIndex(), from, from, QModelIndex(), to);
    }
    qSwap(listOfElements[from], listOfElements[to]);
    endMoveRows();
}

void Match3Model::findMatches()
{
    for (int i = 0; i < height*width; i += width)
    {
        findHorizontalMatches(i);
    }

    for (int i = 0; i < width; i++)
    {
        findVerticalMatches(i);
    }

    if (matches.size() > 0)
    {
        removeMatches();
        elementShift();
        return;
    }
    if (!isFullGrid())
    {
        fillGameGrid();
    }

}

void Match3Model::findHorizontalMatches(int index)
{
    QVector<int> temp;
    temp.append(index);

    for (int i = 0; i < width-1; i++)
    {
        if (listOfElements[index].type() == listOfElements[index+1].type() && listOfElements[index].type() != -1)
        {
            temp.append(index+1);
        }
        else
        {
            if (temp.size() >= 3)
            {
                for (int i = 0; i < temp.size(); i++)
                {
                    if (!matches.contains(temp[i]))
                    {
                        matches.append(temp[i]);
                    }
                }
            }

            temp.clear();
            temp.append(index+1);
        }
        index += 1;
    }
    if (temp.size() >= 3)
    {
        for (int i = 0; i < temp.size(); i++)
        {
            if (!matches.contains(temp[i]))
            {
                matches.append(temp[i]);
            }

        }
    }
}

void Match3Model::findVerticalMatches(int index)
{
    QVector<int> temp;
    temp.append(index);

    for (int i = 0; i < height-1; i++)
    {
        if (listOfElements[index].type() == listOfElements[index+width].type() && listOfElements[index].type() != -1)
        {
            temp.append(index+width);
        }
        else
        {
            if (temp.size() >= 3)
            {
                for (int i = 0; i < temp.size(); i++)
                {
                    if (!matches.contains(temp[i]))
                    {
                        matches.append(temp[i]);
                    }
                }
            }
            temp.clear();
            temp.append(index+width);
        }
        index += width;
    }
    if (temp.size() >= 3)
    {
        for (int i = 0; i < temp.size(); i++)
        {
            if (!matches.contains(temp[i]))
            {
                matches.append(temp[i]);
            }

        }
    }
}

void Match3Model::removeMatches()
{
    for (int i = 0; i < matches.size(); i++)
    {
        listOfElements.replace(matches[i], Element(-1, ""));
    }
    score += matches.size() * 100;
    qDebug() << score;
    matches.clear();
}

void Match3Model::elementShift()
{
    for (int i = 0; i < width; i++)
    {
        int index = i;

        for (int j = 0; j < height - 1; j++)
        {
            if (listOfElements[index].type() == -1)
            {
                shiftAboveElement(index);
            }
            index += width;
        }
    }
    findMatches();
}

void Match3Model::shiftAboveElement(int index)
{
    int cIndex = index;
    index += width;

    while (index < width*height)
    {
        if (listOfElements[index].type() != -1)
        {
            cIndex = index - cIndex;
            for (int i = 0; i < cIndex; i++)
            {
                move(index, index-1);
                index -= 1;
            }
            index += 1;

            for (int i = 0; i < cIndex - 1; i++)
            {
                move(index, index+1);
                index += 1;
            }
            return;
        }
        index += width;
    }
}

bool Match3Model::isMoved()
{
    return true;
}

bool Match3Model::isFullGrid()
{
    for (int i = 0; i < listOfElements.size(); i++)
    {
        if (listOfElements[i].type() == -1)
        {
            return false;
        }
    }
    return true;
}
