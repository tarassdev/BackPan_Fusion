/**
 * @file FusionConvention.h
 * @author Serhii Filimoshyn
 * @brief Earth axes convention.
 */

#ifndef FUSION_CONVENTION_H
#define FUSION_CONVENTION_H

//------------------------------------------------------------------------------
// Definitions

/**
 * @brief Earth axes convention.
 */
typedef enum {
    FusionConventionNwu, /* North-West-Up */
    FusionConventionEnu, /* East-North-Up */
    FusionConventionNed, /* North-East-Down */
} FusionConvention;

#endif

//------------------------------------------------------------------------------
// End of file
