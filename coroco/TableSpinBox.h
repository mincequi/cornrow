#ifndef TABLESPINBOX_H
#define TABLESPINBOX_H

#include <QDoubleSpinBox>

class TableSpinBox : public QDoubleSpinBox
{
public:
    explicit TableSpinBox(QWidget *parent = Q_NULLPTR);
    ~TableSpinBox();

    virtual void stepBy(int steps) override;

    void setTable(const std::vector<float>& table, int index);

private:
    std::vector<float>  m_table;
    int                 m_index = 0;
};

#endif // TABLESPINBOX_H
