#include "QZeroPropsServerPrivate.h"

#include "QZeroPropsServicePrivate.h"
#include <QtZeroProps/QZeroPropsService.h>

namespace QtZeroProps
{

QZeroPropsServerPrivate::QZeroPropsServerPrivate(QObject *parent) :
    QObject(parent)
{
}

QZeroPropsServerPrivate::~QZeroPropsServerPrivate()
{
}

QZeroPropsService* QZeroPropsServerPrivate::startPublishing(const ServiceConfiguration& config)
{
    m_currentService = new QZeroPropsService;
    auto temp = createService(config);
    m_currentService->d = temp;
    m_currentService->setDebounceTime(0);

    return m_currentService;
}

void QZeroPropsServerPrivate::stopService()
{
    if (!m_currentService) {
        return;
    }

    m_currentService->d->disconnect();
    m_currentService = nullptr;
}

} // namespace QtZeroProps
