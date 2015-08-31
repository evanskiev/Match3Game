#ifndef ELEMENT_H
#define ELEMENT_H

#include <QString>


class Element
{
protected:
    int m_type;
    QString m_path;

public:
    Element();
    Element(int type, QString path);
    ~Element();

    int type() const;
    QString path() const;
};

#endif // ELEMENT_H
