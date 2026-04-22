#ifndef CONVERT_H
#define CONVERT_H

#include <cjson/cJSON.h>

/**
 * Gets the rate for a specific currency from the API response.
 * Handles both single quote objects and array of quote objects.
 * @param json The cJSON object or array.
 * @param to_currency The ISO 4217 code to look for.
 * @return The rate, or -1.0 on failure.
 */
double get_rate_for_currency(const cJSON* json, const char* to_currency);

#endif /* CONVERT_H */
