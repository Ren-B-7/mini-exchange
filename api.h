#ifndef API_H
#define API_H

#include <cjson/cJSON.h>

/**
 * Fetches exchange rate(s) from the Frankfurter API.
 * @param from_currency The ISO 4217 code for the base currency.
 * @param to_currency The ISO 4217 code for the desired currency, or NULL for
 * all.
 * @return A cJSON object (array or object) containing the API response, or
 * NULL.
 */
cJSON* fetch_rates(const char* from_currency, const char* to_currency);

#endif /* API_H */
