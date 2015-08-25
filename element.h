#ifndef ELEMENT_H
#define ELEMENT_H

#include <QString>


class Element
{
protected:
    int _type;
    QString _path;

public:
    Element();
    Element(int type, QString path);
    ~Element();

    int type() const;
    QString path() const;
};

#endif // ELEMENT_H
