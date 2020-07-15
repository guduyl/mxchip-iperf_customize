// Copyright (c) 2015-2019 The HomeKit ADK Contributors
//
// Licensed under the Apache License, Version 2.0 (the “License”);
// you may not use this file except in compliance with the License.
// See [CONTRIBUTORS.md] for the list of HomeKit ADK project authors.

// This file contains a set of example verifier codes that are intended FOR DEVELOPMENT PURPOSES ONLY.
// See the manufacturing guidelines in the HAP specification for more information.

#ifndef ACCESSORY_SETUP_H
#define ACCESSORY_SETUP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "HAPPlatform.h"
#include "HAPPlatform+Init.h"

/**
 * Setup info. Contains the SRP salt and the SRP verifier.
 */
extern const HAPSetupInfo *_Nullable SETUP_INFO;

/**
 * Setup Code. Only allowed when NFC is used and display is not used.
 */
extern const HAPSetupCode *_Nullable SETUP_CODE;

/**
 * Setup ID. Optional.
 */
extern const HAPSetupID *_Nullable SETUP_ID;

#ifdef __cplusplus
}
#endif

#endif
