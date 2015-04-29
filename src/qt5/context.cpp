/*
 * Copyright (C) 2015 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: William Hua <william.hua@canonical.com>
 */

#include "context.h"

#include <QMetaMethod>

namespace ubuntu
{

ProxyContext::ProxyContext(QPlatformInputContext *parent)
    : m_slave(0),
      m_keyboardRect(keyboardRect()),
      m_animating(isAnimating()),
      m_inputPanelVisible(isInputPanelVisible()),
      m_locale(locale()),
      m_inputDirection(inputDirection())
{
    Q_UNUSED(parent)
}

ProxyContext::~ProxyContext()
{
    delete m_slave;
}

void ProxyContext::slaveChanged(const ProxyPlugin &plugin)
{
    delete m_slave;

    m_slave = plugin.createSlave();

    emitChangeSignals();
}

bool ProxyContext::isValid() const
{
    return true;
}

bool ProxyContext::hasCapability(Capability capability) const
{
    Q_UNUSED(capability)

    return true;
}

void ProxyContext::reset()
{
    if (m_slave)
        m_slave->reset();
    else
        QPlatformInputContext::reset();
}

void ProxyContext::commit()
{
    if (m_slave)
        m_slave->commit();
    else
        QPlatformInputContext::commit();
}

void ProxyContext::update(Qt::InputMethodQueries queries)
{
    if (m_slave)
        m_slave->update(queries);
    else
        QPlatformInputContext::update(queries);
}

void ProxyContext::invokeAction(QInputMethod::Action a, int cursorPosition)
{
    if (m_slave)
        m_slave->invokeAction(a, cursorPosition);
    else
        QPlatformInputContext::invokeAction(a, cursorPosition);
}

bool ProxyContext::filterEvent(const QEvent *event)
{
    if (m_slave)
        return m_slave->filterEvent(event);
    else
        return QPlatformInputContext::filterEvent(event);
}

QRectF ProxyContext::keyboardRect() const
{
    if (m_slave)
        return m_slave->keyboardRect();
    else
        return QPlatformInputContext::keyboardRect();
}

bool ProxyContext::isAnimating() const
{
    if (m_slave)
        return m_slave->isAnimating();
    else
        return QPlatformInputContext::isAnimating();
}

void ProxyContext::showInputPanel()
{
    if (m_slave)
        m_slave->showInputPanel();
    else
        QPlatformInputContext::showInputPanel();
}

void ProxyContext::hideInputPanel()
{
    if (m_slave)
        m_slave->hideInputPanel();
    else
        QPlatformInputContext::hideInputPanel();
}

bool ProxyContext::isInputPanelVisible() const
{
    if (m_slave)
        return m_slave->isInputPanelVisible();
    else
        return QPlatformInputContext::isInputPanelVisible();
}

QLocale ProxyContext::locale() const
{
    if (m_slave)
        return m_slave->locale();
    else
        return QPlatformInputContext::locale();
}

Qt::LayoutDirection ProxyContext::inputDirection() const
{
    if (m_slave)
        return m_slave->inputDirection();
    else
        return QPlatformInputContext::inputDirection();
}

void ProxyContext::setFocusObject(QObject *object)
{
    if (m_slave)
        m_slave->setFocusObject(object);
    else
        QPlatformInputContext::setFocusObject(object);
}

void ProxyContext::emitChangeSignals()
{
    QRectF newKeyboardRect = keyboardRect();
    bool newAnimating = isAnimating();
    bool newInputPanelVisible = isInputPanelVisible();
    QLocale newLocale = locale();
    Qt::LayoutDirection newInputDirection = inputDirection();

    if (newKeyboardRect != m_keyboardRect)
    {
        m_keyboardRect = newKeyboardRect;
        emitKeyboardRectChanged();
    }

    if (newAnimating != m_animating)
    {
        m_animating = newAnimating;
        emitAnimatingChanged();
    }

    if (newInputPanelVisible != m_inputPanelVisible)
    {
        m_inputPanelVisible = newInputPanelVisible;
        emitInputPanelVisibleChanged();
    }

    if (newLocale != m_locale)
    {
        m_locale = newLocale;
        emitLocaleChanged();
    }

    if (newInputDirection != m_inputDirection)
    {
        m_inputDirection = newInputDirection;
        emitInputDirectionChanged(newInputDirection);
    }
}

bool ProxyContext::x11FilterEvent(uint keyval, uint keycode, uint state, bool press)
{
    QMetaMethod method;

    if (m_slave)
    {
        int methodIndex = m_slave->metaObject()->indexOfMethod("x11FilterEvent(uint,uint,uint,bool)");

        if (methodIndex != -1)
            method = m_slave->metaObject()->method(methodIndex);
    }

    if (method.isValid())
    {
        bool retval = false;

        method.invoke(m_slave,
                      Qt::DirectConnection,
                      Q_RETURN_ARG(bool, retval),
                      Q_ARG(uint, keyval),
                      Q_ARG(uint, keycode),
                      Q_ARG(uint, state),
                      Q_ARG(bool, press));

        if (retval)
            return retval;
    }

    return false;
}

} /* namespace ubuntu */
