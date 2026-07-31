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


#include "NavigationAnimation.h"
#include <Inventor/nodes/SoCamera.h>
#include <Inventor/nodes/SoOrthographicCamera.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>

#include <cmath>
#include <numbers>

using namespace Gui;

NavigationAnimation::NavigationAnimation(NavigationStyle* navigation)
    : navigation(navigation)
{
    auto* animation = static_cast<QVariantAnimation*>(this);
    QObject::connect(animation, &QVariantAnimation::finished, animation, [this]() {
        Q_EMIT completed();
    });
}

void NavigationAnimation::updateCurrentValue(const QVariant& value)
{
    if (state() != QAbstractAnimation::State::Running) {
        return;
    }
    update(value);
}

void NavigationAnimation::onStop([[maybe_unused]] bool finished)
{}

FixedTimeAnimation::FixedTimeAnimation(
    NavigationStyle* navigation,
    const SbRotation& orientation,
    const SbVec3f& rotationCenter,
    const SbVec3f& translation,
    int duration,
    const QEasingCurve::Type easingCurve
)
    : NavigationAnimation(navigation)
    , targetOrientation(orientation)
    , targetTranslation(translation)
    , rotationCenter(rotationCenter)
{
    setDuration(duration);
    setStartValue(0.0);
    setEndValue(duration * 1.0);
    setEasingCurve(easingCurve);
}

void FixedTimeAnimation::initialize()
{
#if (COIN_MAJOR_VERSION * 100 + COIN_MINOR_VERSION * 10 + COIN_MICRO_VERSION < 403)
    navigation->findBoundingSphere();
#endif

    prevAngle = 0;
    prevTranslation = SbVec3f(0, 0, 0);

    // Find an axis and angle to rotate from the camera orientation to the target orientation using
    // post-multiplication
    SbVec3f rotationAxisPost;
    float angle;
    SbRotation(navigation->getCamera()->orientation.getValue().inverse() * targetOrientation)
        .getValue(rotationAxisPost, angle);
    if (angle > std::numbers::pi) {
        angle -= float(2 * std::numbers::pi);
    }

    // Convert post-multiplication axis to a pre-multiplication axis
    navigation->getCamera()->orientation.getValue().inverse().multVec(rotationAxisPost, rotationAxis);

    angularVelocity = angle / duration();
    linearVelocity = targetTranslation / duration();
}

/**
 * @param value The elapsed time
 */
void FixedTimeAnimation::update(const QVariant& value)
{
    SoCamera* camera = navigation->getCamera();
    if (!camera) {
        return;
    }

    float angle = value.toFloat() * angularVelocity;
    SbVec3f translation = value.toFloat() * linearVelocity;

    SbRotation rotation(rotationAxis, angle - prevAngle);

    camera->position = camera->position.getValue() - prevTranslation;
    navigation->reorientCamera(
        camera,
        rotation,
        rotationCenter,
        NavigationStyle::OrientationChangeSource::Programmatic
    );
    camera->position = camera->position.getValue() + translation;

    prevAngle = angle;
    prevTranslation = translation;
}

/**
 * @param finished True when the animation is finished, false when interrupted
 */
void FixedTimeAnimation::onStop(bool finished)
{
    if (finished) {
        SoCamera* camera = navigation->getCamera();
        if (!camera) {
            return;
        }

        // Set exact target orientation
        navigation->setCameraOrientationValue(
            camera,
            targetOrientation,
            NavigationStyle::OrientationChangeSource::Programmatic
        );
        camera->position = camera->position.getValue() + targetTranslation - prevTranslation;
    }
}

CameraPose CameraPose::capture(const SoCamera* camera)
{
    CameraPose pose;
    if (!camera) {
        return pose;
    }

    pose.position = camera->position.getValue();
    pose.orientation = camera->orientation.getValue();
    pose.focalDistance = camera->focalDistance.getValue();
    pose.type = camera->getTypeId();

    if (camera->isOfType(SoOrthographicCamera::getClassTypeId())) {
        pose.height = static_cast<const SoOrthographicCamera*>(camera)->height.getValue();
    }
    else if (camera->isOfType(SoPerspectiveCamera::getClassTypeId())) {
        pose.height = static_cast<const SoPerspectiveCamera*>(camera)->heightAngle.getValue();
    }

    return pose;
}

void CameraPose::apply(SoCamera* camera) const
{
    if (!camera) {
        return;
    }

    camera->position = position;
    camera->orientation = orientation;
    camera->focalDistance = focalDistance;

    // The height fields of the two projections are not the same quantity, so a pose taken
    // from one is meaningless on the other and only the pose it does describe is written.
    if (camera->getTypeId() != type) {
        return;
    }

    if (camera->isOfType(SoOrthographicCamera::getClassTypeId())) {
        static_cast<SoOrthographicCamera*>(camera)->height = height;
    }
    else if (camera->isOfType(SoPerspectiveCamera::getClassTypeId())) {
        static_cast<SoPerspectiveCamera*>(camera)->heightAngle = height;
    }
}

CameraAnimation::CameraAnimation(
    NavigationStyle* navigation,
    const CameraPose& target,
    int duration,
    const QEasingCurve::Type easingCurve
)
    : NavigationAnimation(navigation)
    , target(target)
{
    setDuration(duration);
    setStartValue(0.0);
    setEndValue(1.0);
    setEasingCurve(easingCurve);
}

void CameraAnimation::initialize()
{
#if (COIN_MAJOR_VERSION * 100 + COIN_MINOR_VERSION * 10 + COIN_MICRO_VERSION < 403)
    navigation->findBoundingSphere();
#endif

    // Read at the last moment rather than on construction, so that a move interrupting an
    // earlier one picks up from where the camera actually is instead of jumping.
    start = CameraPose::capture(navigation->getCamera());
}

/**
 * @param value The progress of the move, from 0 at the start pose to 1 at the target
 */
void CameraAnimation::update(const QVariant& value)
{
    SoCamera* camera = navigation->getCamera();
    if (!camera) {
        return;
    }

    const float t = value.toFloat();

    // Zoom is a scale, so it is walked geometrically. Interpolating it linearly makes a
    // large change rush at one end and crawl at the other.
    const auto scaleStep = [t](float from, float to) {
        if (from <= 0.0F || to <= 0.0F) {
            return from + t * (to - from);
        }
        return from * std::pow(to / from, t);
    };

    navigation->setCameraOrientationValue(
        camera,
        SbRotation::slerp(start.orientation, target.orientation, t),
        NavigationStyle::OrientationChangeSource::Programmatic
    );
    camera->position = start.position + t * (target.position - start.position);
    camera->focalDistance = scaleStep(start.focalDistance, target.focalDistance);

    if (camera->getTypeId() == start.type && start.type == target.type) {
        const float height = scaleStep(start.height, target.height);
        if (camera->isOfType(SoOrthographicCamera::getClassTypeId())) {
            static_cast<SoOrthographicCamera*>(camera)->height = height;
        }
        else if (camera->isOfType(SoPerspectiveCamera::getClassTypeId())) {
            static_cast<SoPerspectiveCamera*>(camera)->heightAngle = height;
        }
    }
}

/**
 * @param finished True when the animation is finished, false when interrupted
 */
void CameraAnimation::onStop(bool finished)
{
    if (finished) {
        target.apply(navigation->getCamera());
    }
}

/**
 * @param navigation The navigation style
 * @param axis The rotation axis in screen coordinates
 * @param velocity The angular velocity in radians per second
 */
SpinningAnimation::SpinningAnimation(NavigationStyle* navigation, const SbVec3f& axis, float velocity)
    : NavigationAnimation(navigation)
    , rotationAxis(axis)
{
    setDuration((2 * std::numbers::pi / velocity) * 1000.0);
    setStartValue(0.0);
    setEndValue(2 * std::numbers::pi);
    setLoopCount(-1);
}

void SpinningAnimation::initialize()
{
#if (COIN_MAJOR_VERSION * 100 + COIN_MINOR_VERSION * 10 + COIN_MICRO_VERSION < 403)
    navigation->findBoundingSphere();
#endif

    prevAngle = 0;

    navigation->setViewing(true);
    navigation->setViewingMode(NavigationStyle::SPINNING);
}

/**
 * @param value The angle in radians
 */
void SpinningAnimation::update(const QVariant& value)
{
    SoCamera* camera = navigation->getCamera();
    if (!camera) {
        return;
    }

    SbRotation deltaRotation = SbRotation(rotationAxis, value.toFloat() - prevAngle);
    navigation->reorientCamera(
        camera,
        deltaRotation,
        NavigationStyle::OrientationChangeSource::Interactive
    );

    prevAngle = value.toFloat();
}

/**
 * @param finished True when the animation is finished, false when interrupted
 */
void SpinningAnimation::onStop([[maybe_unused]] bool finished)
{
    if (navigation->getViewingMode() != NavigationStyle::SPINNING) {
        return;
    }
    navigation->setViewingMode(
        navigation->isViewing() ? NavigationStyle::IDLE : NavigationStyle::INTERACT
    );
}
