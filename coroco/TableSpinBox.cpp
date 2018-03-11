#include "TableSpinBox.h"

TableSpinBox::TableSpinBox(QWidget *parent)
    : QDoubleSpinBox(parent)
{
}

TableSpinBox::~TableSpinBox()
{
}

void TableSpinBox::setTable(const std::vector<float>& table, int index)
{
    m_table = table;
    m_index = index;
}

int TableSpinBox::index() const
{
    return m_index;
}

void TableSpinBox::setIndex(int i)
{
    if (i < 0) m_index = 0;
    else if (i >= m_table.size()) m_index = m_table.size()-1;
    else m_index = i;

    auto value = m_table[m_index];

    if (value < 1.0) setDecimals(2);
    else if (value < 100.0) setDecimals(1);
    else setDecimals(0);

    setValue(value);
}

void TableSpinBox::stepBy(int steps)
{
    m_index += steps;

    setIndex(m_index);
}
