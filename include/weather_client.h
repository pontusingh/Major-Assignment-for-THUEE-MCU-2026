#ifndef WEATHER_CLIENT_H
#define WEATHER_CLIENT_H

#include "pet_state.h"

void weatherUpdate(PetState& s, unsigned long nowMs);
void weatherHandleStatusCode(int httpCode);

#endif
