#include "element.h"

Element::Element()
{

}

Element::Element(int type, QString path) : _type(type), _path(path)
{

}

Element::~Element()
{

}

int Element::type() const
{
    return _type;
}

QString Element::path() const
{
    return _path;
}
