/**
 * @file FusionAhrs.c
 * @brief AHRS algorithm to combine gyroscope, accelerometer, and magnetometer
 * measurements into a single measurement of orientation relative to the Earth.
 */

//------------------------------------------------------------------------------
// Includes

#include <float.h>
#include "FusionAhrs.h"
#include <math.h>

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Initial gain used during the initialisation.
 */
#define INITIAL_GAIN (10.0f)

/**
 * @brief Initialisation period in seconds.
 */
#define INITIALISATION_PERIOD (3.0f)

//------------------------------------------------------------------------------
// Function declarations

static inline FusionVector HalfGravity(const FusionAhrs *const ahrs);

static inline FusionVector HalfMagnetic(const FusionAhrs *const ahrs);

static inline FusionVector Feedback(const FusionVector sensor, const FusionVector reference);

static inline int Clamp(const int value, const int min, const int max);

//------------------------------------------------------------------------------
// Functions

/**
 * @brief Initialises the AHRS algorithm structure.
 * @param ahrs AHRS algorithm structure.
 */
void FusionAhrsInitialise(FusionAhrs *const ahrs) {
    const FusionAhrsSettings settings = {
            .convention = FusionConventionNwu,
            .gain = 0.5f,
            .gyroscopeRange = 0.0f,
            .accelerationRejection = 90.0f,
            .magneticRejection = 90.0f,
            .recoveryTriggerPeriod = 0,
    };
    FusionAhrsSetSettings(ahrs, &settings);
    FusionAhrsReset(ahrs);
}

/**
 * @brief Resets the AHRS algorithm. This is equivalent to reinitialising the
 * algorithm while maintaining the current settings.
 * @param ahrs AHRS algorithm structure.
 */
void FusionAhrsReset(FusionAhrs *const ahrs) {
    ahrs->quaternion = FUSION_IDENTITY_QUATERNION;
    ahrs->accelerometer = FUSION_VECTOR_ZERO;
    ahrs->initialising = true;
    ahrs->rampedGain = INITIAL_GAIN;
    ahrs->angularRateRecovery = false;
    ahrs->halfAccelerometerFeedback = FUSION_VECTOR_ZERO;
    ahrs->halfMagnetometerFeedback = FUSION_VECTOR_ZERO;
    ahrs->accelerometerIgnored = false;
    ahrs->accelerationRecoveryTrigger = 0;
    ahrs->accelerationRecoveryTimeout = ahrs->settings.recoveryTriggerPeriod;
    ahrs->magnetometerIgnored = false;
    ahrs->magneticRecoveryTrigger = 0;
    ahrs->magneticRecoveryTimeout = ahrs->settings.recoveryTriggerPeriod;
}

/**
 * @brief Sets the AHRS algorithm settings.
 * @param ahrs AHRS algorithm structure.
 * @param settings Settings.
 */
void FusionAhrsSetSettings(FusionAhrs *const ahrs, const FusionAhrsSettings *const settings) {
    ahrs->settings.convention = settings->convention;
    ahrs->settings.gain = settings->gain;
    ahrs->settings.gyroscopeRange = settings->gyroscopeRange == 0.0f ? FLT_MAX : 0.98f * settings->gyroscopeRange;
    ahrs->settings.accelerationRejection = settings->accelerationRejection == 0.0f ? FLT_MAX : powf(0.5f * sinf(FusionDegreesToRadians(settings->accelerationRejection)), 2);
    ahrs->settings.magneticRejection = settings->magneticRejection == 0.0f ? FLT_MAX : powf(0.5f * sinf(FusionDegreesToRadians(settings->magneticRejection)), 2);
    ahrs->settings.recoveryTriggerPeriod = settings->recoveryTriggerPeriod;
    ahrs->accelerationRecoveryTimeout = ahrs->settings.recoveryTriggerPeriod;
    ahrs->magneticRecoveryTimeout = ahrs->settings.recoveryTriggerPeriod;
    if ((settings->gain == 0.0f) || (settings->recoveryTriggerPeriod == 0)) { // disable acceleration and magnetic rejection features if gain is zero
        ahrs->settings.accelerationRejection = FLT_MAX;
        ahrs->settings.magneticRejection = FLT_MAX;
    }
    if (ahrs->initialising == false) {
        ahrs->rampedGain = ahrs->settings.gain;
    }
    ahrs->rampedGainStep = (INITIAL_GAIN - ahrs->settings.gain) / INITIALISATION_PERIOD;
}

/**
 * @brief Returns the linear acceleration measurement equal to the accelerometer
 * measurement with gravity removed.
 * @param ahrs AHRS algorithm structure.
 * @return Linear acceleration measurement in g.
 */
FusionVector FusionAhrsGetLinearAcceleration(const FusionAhrs *const ahrs) {
    switch (ahrs->settings.convention) {
        case FusionConventionNwu:
        case FusionConventionEnu: {
            return FusionVectorSubtract(ahrs->accelerometer, FusionAhrsGetGravity(ahrs));
        }
        case FusionConventionNed: {
            return FusionVectorAdd(ahrs->accelerometer, FusionAhrsGetGravity(ahrs));
        }
    }
    return FUSION_VECTOR_ZERO; // avoid compiler warning
}

/**
 * @brief Returns the Earth acceleration measurement equal to accelerometer
 * measurement in the Earth coordinate frame with gravity removed.
 * @param ahrs AHRS algorithm structure.
 * @return Earth acceleration measurement in g.
 */
FusionVector FusionAhrsGetEarthAcceleration(const FusionAhrs *const ahrs) {
#define Q ahrs->quaternion.element
#define A ahrs->accelerometer.axis

    // Calculate accelerometer measurement in the Earth coordinate frame
    const float qwqw = Q.w * Q.w; // calculate common terms to avoid repeated operations
    const float qwqx = Q.w * Q.x;
    const float qwqy = Q.w * Q.y;
    const float qwqz = Q.w * Q.z;
    const float qxqy = Q.x * Q.y;
    const float qxqz = Q.x * Q.z;
    const float qyqz = Q.y * Q.z;
    FusionVector accelerometer = {.axis = {
            .x = 2.0f * ((qwqw - 0.5f + Q.x * Q.x) * A.x + (qxqy - qwqz) * A.y + (qxqz + qwqy) * A.z),
            .y = 2.0f * ((qxqy + qwqz) * A.x + (qwqw - 0.5f + Q.y * Q.y) * A.y + (qyqz - qwqx) * A.z),
            .z = 2.0f * ((qxqz - qwqy) * A.x + (qyqz + qwqx) * A.y + (qwqw - 0.5f + Q.z * Q.z) * A.z),
    }}; // rotation matrix multiplied with the accelerometer

    // Remove gravity from accelerometer measurement
    switch (ahrs->settings.convention) {
        case FusionConventionNwu:
        case FusionConventionEnu:
            accelerometer.axis.z -= 1.0f;
            break;
        case FusionConventionNed:
            accelerometer.axis.z += 1.0f;
            break;
    }
    return accelerometer;
#undef Q
#undef A
}

/**
 * @brief Returns the AHRS algorithm internal states.
 * @param ahrs AHRS algorithm structure.
 * @return AHRS algorithm internal states.
 */
FusionAhrsInternalStates FusionAhrsGetInternalStates(const FusionAhrs *const ahrs) {
    const FusionAhrsInternalStates internalStates = {
            .accelerationError = FusionRadiansToDegrees(FusionAsin(2.0f * FusionVectorMagnitude(ahrs->halfAccelerometerFeedback))),
            .accelerometerIgnored = ahrs->accelerometerIgnored,
            .accelerationRecoveryTrigger = ahrs->settings.recoveryTriggerPeriod == 0 ? 0.0f : (float) ahrs->accelerationRecoveryTrigger / (float) ahrs->settings.recoveryTriggerPeriod,
            .magneticError = FusionRadiansToDegrees(FusionAsin(2.0f * FusionVectorMagnitude(ahrs->halfMagnetometerFeedback))),
            .magnetometerIgnored = ahrs->magnetometerIgnored,
            .magneticRecoveryTrigger = ahrs->settings.recoveryTriggerPeriod == 0 ? 0.0f : (float) ahrs->magneticRecoveryTrigger / (float) ahrs->settings.recoveryTriggerPeriod,
    };
    return internalStates;
}

/**
 * @brief Returns the AHRS algorithm flags.
 * @param ahrs AHRS algorithm structure.
 * @return AHRS algorithm flags.
 */
FusionAhrsFlags FusionAhrsGetFlags(const FusionAhrs *const ahrs) {
    const FusionAhrsFlags flags = {
            .initialising = ahrs->initialising,
            .angularRateRecovery = ahrs->angularRateRecovery,
            .accelerationRecovery = ahrs->accelerationRecoveryTrigger > ahrs->accelerationRecoveryTimeout,
            .magneticRecovery= ahrs->magneticRecoveryTrigger > ahrs->magneticRecoveryTimeout,
    };
    return flags;
}

/**
 * @brief Sets the heading of the orientation measurement provided by the AHRS
 * algorithm. This function can be used to reset drift in heading when the AHRS
 * algorithm is being used without a magnetometer.
 * @param ahrs AHRS algorithm structure.
 * @param heading Heading angle in degrees.
 */
void FusionAhrsSetHeading(FusionAhrs *const ahrs, const float heading) {
#define Q ahrs->quaternion.element
    const float yaw = atan2f(Q.w * Q.z + Q.x * Q.y, 0.5f - Q.y * Q.y - Q.z * Q.z);
    const float halfYawMinusHeading = 0.5f * (yaw - FusionDegreesToRadians(heading));
    const FusionQuaternion rotation = {.element = {
            .w = cosf(halfYawMinusHeading),
            .x = 0.0f,
            .y = 0.0f,
            .z = -1.0f * sinf(halfYawMinusHeading),
    }};
    ahrs->quaternion = FusionQuaternionMultiply(rotation, ahrs->quaternion);
#undef Q
}
//------------------------------------------------------------------------------
// End of file
