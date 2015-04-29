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

#ifndef __CONTEXT_H__
#define __CONTEXT_H__

#include <QtGui/qpa/qplatforminputcontext.h>
#include <QLocale>
#include <QRectF>

#include "plugin.h"

namespace ubuntu
{

class ProxyContext : public QPlatformInputContext
{
private:

    Q_OBJECT

public:

    explicit ProxyContext(QPlatformInputContext *parent = 0);

    virtual ~ProxyContext();

    virtual bool isValid() const;
    virtual bool hasCapability(Capability capability) const;
    virtual void reset();
    virtual void commit();
    virtual void update(Qt::InputMethodQueries queries);
    virtual void invokeAction(QInputMethod::Action a, int cursorPosition);
    virtual bool filterEvent(const QEvent *event);
    virtual QRectF keyboardRect() const;
    virtual bool isAnimating() const;
    virtual void showInputPanel();
    virtual void hideInputPanel();
    virtual bool isInputPanelVisible() const;
    virtual QLocale locale() const;
    virtual Qt::LayoutDirection inputDirection() const;
    virtual void setFocusObject(QObject *object);
    Q_INVOKABLE bool x11FilterEvent(uint keyval, uint keycode, uint state, bool press);

public Q_SLOTS:

    void slaveChanged(const ProxyPlugin &plugin);

private:

    void emitChangeSignals();

    QPlatformInputContext *m_slave;
    QRectF m_keyboardRect;
    bool m_animating;
    bool m_inputPanelVisible;
    QLocale m_locale;
    Qt::LayoutDirection m_inputDirection;
};

} /* namespace ubuntu */

#endif /* __CONTEXT_H__ */
