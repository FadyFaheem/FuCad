// SPDX-License-Identifier: LGPL-2.1-or-later
/****************************************************************************
 *                                                                          *
 *   Copyright (c) 2023 Bas Ruigrok (Rexbas) <Rexbas@proton.me>             *
 *                                                                          *
 *   This file is part of FreeCAD.                                          *
 *                                                                          *
 *   FreeCAD is free software: you can redistribute it and/or modify it     *
 *   under the terms of the GNU Lesser General Public License as            *
 *   published by the Free Software Foundation, either version 2.1 of the   *
 *   License, or (at your option) any later version.                        *
 *                                                                          *
 *   FreeCAD is distributed in the hope that it will be useful, but         *
 *   WITHOUT ANY WARRANTY; without even the implied warranty of             *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU       *
 *   Lesser General Public License for more details.                        *
 *                                                                          *
 *   You should have received a copy of the GNU Lesser General Public       *
 *   License along with FreeCAD. If not, see                                *
 *   <https://www.gnu.org/licenses/>.                                       *
 *                                                                          *
 ***************************************************************************/

#pragma once

#include "Navigation/NavigationStyle.h"
#include <Inventor/SbRotation.h>
#include <Inventor/SbVec3f.h>
#include <Inventor/SoType.h>
#include <QVariantAnimation>

namespace Gui
{

class GuiExport NavigationAnimation: protected QVariantAnimation
{
    Q_OBJECT
public:
    explicit NavigationAnimation(NavigationStyle* navigation);
    using QVariantAnimation::state;

Q_SIGNALS:
    // QVariantAnimation::finished is intentionally hidden by protected inheritance.
    void completed();
    void interrupted();

protected:
    NavigationStyle* navigation;

    virtual void initialize() = 0;
    virtual void update(const QVariant& value) = 0;
    virtual void onStop(bool finished);

private:
    void updateCurrentValue(const QVariant& value) override;

    friend class NavigationAnimator;
    friend class QObject;
};

class GuiExport FixedTimeAnimation: public NavigationAnimation
{
public:
    explicit FixedTimeAnimation(
        NavigationStyle* navigation,
        const SbRotation& orientation,
        const SbVec3f& rotationCenter,
        const SbVec3f& translation,
        int duration,
        const QEasingCurve::Type easingCurve
    );

private:
    float angularVelocity;   // [rad/ms]
    SbVec3f linearVelocity;  // [/ms]

    SbRotation targetOrientation;
    SbVec3f targetTranslation;

    float prevAngle;
    SbVec3f prevTranslation;

    SbVec3f rotationCenter;
    SbVec3f rotationAxis;

    void initialize() override;
    void update(const QVariant& value) override;
    void onStop(bool finished) override;
};

/**
 * A complete camera pose. An orientation and a position alone do not describe what the
 * viewer shows, because the zoom lives in a field whose name and meaning depend on the
 * projection, so it is read and written here rather than at every call site.
 */
struct GuiExport CameraPose
{
    SbVec3f position {0.0F, 0.0F, 0.0F};
    SbRotation orientation;
    float focalDistance {1.0F};
    /// Orthographic height or perspective height angle, whichever the camera carries.
    float height {1.0F};
    /// The projection the pose was taken from. A pose does not carry over to another one.
    SoType type;

    static CameraPose capture(const SoCamera* camera);
    void apply(SoCamera* camera) const;
};

/**
 * Carries the camera from wherever it is to a given pose, zoom included.
 *
 * FixedTimeAnimation cannot do this: it turns the camera through a fixed angle about a
 * fixed centre and never touches the zoom, so it cannot land on a framing that was worked
 * out somewhere else, which is what entering and leaving a sketch needs.
 */
class GuiExport CameraAnimation: public NavigationAnimation
{
public:
    explicit CameraAnimation(
        NavigationStyle* navigation,
        const CameraPose& target,
        int duration,
        QEasingCurve::Type easingCurve
    );

private:
    CameraPose start;
    CameraPose target;

    void initialize() override;
    void update(const QVariant& value) override;
    void onStop(bool finished) override;
};

class GuiExport SpinningAnimation: public NavigationAnimation
{
public:
    explicit SpinningAnimation(NavigationStyle* navigation, const SbVec3f& axis, float velocity);

private:
    SbVec3f rotationAxis;
    float prevAngle;

    void initialize() override;
    void update(const QVariant& value) override;
    void onStop(bool finished) override;
};

}  // namespace Gui
