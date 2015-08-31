#include "element.h"

Element::Element() {

}

Element::Element(int type, QString path) : m_type(type), m_path(path) {

}

Element::~Element() {

}

int Element::type() const {
    return m_type;
}

QString Element::path() const {
    return m_path;
}
