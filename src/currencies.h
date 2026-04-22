#ifndef CURRENCIES_H
#define CURRENCIES_H

#include <stddef.h>

/**
 * Structure representing a currency with its ISO 4217 code and full name.
 */
typedef struct {
	const char* code;
	const char* name;
} Currency;

/**
 * Array of supported currencies by the Frankfurter API.
 */
extern const Currency SUPPORTED_CURRENCIES[];

/**
 * Number of supported currencies.
 */
extern const size_t SUPPORTED_CURRENCIES_COUNT;

/**
 * Prints all supported currencies to stdout.
 */
void list_currencies(void);

/**
 * Searches for currencies matching the query in either code or name.
 * @param query The search string.
 */
void search_currencies(const char* query);

/**
 * Checks if a currency code is supported.
 * @param code The ISO 4217 code.
 * @return 1 if supported, 0 otherwise.
 */
int is_supported_currency(const char* code);

/**
 * Tests the internal currency list against the API for a base currency.
 * @param base_currency The ISO 4217 code to use as base.
 */
void test_currencies_against_api(const char* base_currency);

#endif /* CURRENCIES_H */
