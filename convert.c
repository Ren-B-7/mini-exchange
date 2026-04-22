#include "convert.h"

#include <string.h>

/**
 * Gets the rate from a single quote object.
 */
static double get_rate_from_object(const cJSON* json, const char* to_currency)
{
	const cJSON* quote = cJSON_GetObjectItemCaseSensitive(json, "quote");
	const cJSON* rate = NULL; /* Declare rate here */

	if (cJSON_IsString(quote) &&
	 (strcmp(quote->valuestring, to_currency) == 0)) {
		rate = cJSON_GetObjectItemCaseSensitive(json, "rate");
		if (cJSON_IsNumber(rate)) {
			return rate->valuedouble;
		}
	}
	return -1.0;
}

/**
 * Gets the rate for a specific currency from the API response.
 * Handles both single quote objects and array of quote objects.
 * @param json The cJSON object or array.
 * @param to_currency The ISO 4217 code to look for.
 * @return The rate, or -1.0 on failure.
 */
double get_rate_for_currency(const cJSON* json, const char* to_currency)
{
	const cJSON* item = NULL; /* Declare item here */
	double rate;              /* Declare rate here */

	if (json == NULL) {
		return -1.0;
	}

	/* If it's an array of quote objects */
	if (cJSON_IsArray(json)) {
		cJSON_ArrayForEach(item, json)
		{
			rate = get_rate_from_object(item, to_currency);
			if (rate >= 0) {
				return rate;
			}
		}
	} else if (cJSON_IsObject(json)) {
		/* If it's a single quote object */
		return get_rate_from_object(json, to_currency);
	}

	return -1.0;
}
