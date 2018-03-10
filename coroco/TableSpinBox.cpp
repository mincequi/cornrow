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

void TableSpinBox::stepBy(int steps)
{
    m_index += steps;

    if (m_index < 0) m_index = 0;
    if (m_index >= m_table.size()) m_index = m_table.size()-1;

    setValue(m_table[m_index]);
}
