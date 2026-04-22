#include "currencies.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>

#include "api.h"

/**
 * Supported currencies list (Expanded Frankfurter API list as of April 2026).
 */
const Currency SUPPORTED_CURRENCIES[] = {{"AED", "United Arab Emirates Dirham"},
    {"AFN", "Afghan Afghani"}, {"ALL", "Albanian Lek"},
    {"AMD", "Armenian Dram"}, {"ANG", "Netherlands Antillean Guilder"},
    {"AOA", "Angolan Kwanza"}, {"ARS", "Argentine Peso"},
    {"AUD", "Australian Dollar"}, {"AWG", "Aruban Florin"},
    {"AZN", "Azerbaijani Manat"},
    {"BAM", "Bosnia and Herzegovina Convertible Mark"},
    {"BBD", "Barbadian Dollar"}, {"BDT", "Bangladeshi Taka"},
    {"BGN", "Bulgarian Lev"}, {"BHD", "Bahraini Dinar"},
    {"BIF", "Burundian Franc"}, {"BMD", "Bermudian Dollar"},
    {"BND", "Brunei Dollar"}, {"BOB", "Bolivian Boliviano"},
    {"BRL", "Brazilian Real"}, {"BSD", "Bahamian Dollar"},
    {"BTN", "Bhutanese Ngultrum"}, {"BWP", "Botswana Pula"},
    {"BYN", "Belarusian Ruble"}, {"BZD", "Belize Dollar"},
    {"CAD", "Canadian Dollar"}, {"CDF", "Congolese Franc"},
    {"CHF", "Swiss Franc"}, {"CLP", "Chilean Peso"},
    {"CNH", "Chinese Yuan (Offshore)"}, {"CNY", "Chinese Renminbi"},
    {"COP", "Colombian Peso"}, {"CRC", "Costa Rican Colón"},
    {"CUP", "Cuban Peso"}, {"CVE", "Cape Verdean Escudo"},
    {"CZK", "Czech Koruna"}, {"DJF", "Djiboutian Franc"},
    {"DKK", "Danish Krone"}, {"DOP", "Dominican Peso"},
    {"DZD", "Algerian Dinar"}, {"EGP", "Egyptian Pound"},
    {"ERN", "Eritrean Nakfa"}, {"ETB", "Ethiopian Birr"}, {"EUR", "Euro"},
    {"FJD", "Fijian Dollar"}, {"FKP", "Falkland Islands Pound"},
    {"GBP", "British Pound"}, {"GEL", "Georgian Lari"},
    {"GGP", "Guernsey Pound"}, {"GHS", "Ghanaian Cedi"},
    {"GIP", "Gibraltar Pound"}, {"GMD", "Gambian Dalasi"},
    {"GNF", "Guinean Franc"}, {"GTQ", "Guatemalan Quetzal"},
    {"GYD", "Guyanese Dollar"}, {"HKD", "Hong Kong Dollar"},
    {"HNL", "Honduran Lempira"}, {"HTG", "Haitian Gourde"},
    {"HUF", "Hungarian Forint"}, {"IDR", "Indonesian Rupiah"},
    {"ILS", "Israeli New Shekel"}, {"IMP", "Isle of Man Pound"},
    {"INR", "Indian Rupee"}, {"IQD", "Iraqi Dinar"}, {"IRR", "Iranian Rial"},
    {"ISK", "Icelandic Króna"}, {"JEP", "Jersey Pound"},
    {"JMD", "Jamaican Dollar"}, {"JOD", "Jordanian Dinar"},
    {"JPY", "Japanese Yen"}, {"KES", "Kenyan Shilling"},
    {"KGS", "Kyrgyzstani Som"}, {"KHR", "Cambodian Riel"},
    {"KMF", "Comorian Franc"}, {"KRW", "South Korean Won"},
    {"KWD", "Kuwaiti Dinar"}, {"KYD", "Cayman Islands Dollar"},
    {"KZT", "Kazakhstani Tenge"}, {"LAK", "Lao Kip"}, {"LBP", "Lebanese Pound"},
    {"LKR", "Sri Lankan Rupee"}, {"LRD", "Liberian Dollar"},
    {"LSL", "Lesotho Loti"}, {"LYD", "Libyan Dinar"},
    {"MAD", "Moroccan Dirham"}, {"MDL", "Moldovan Leu"},
    {"MGA", "Malagasy Ariary"}, {"MKD", "Macedonian Denar"},
    {"MMK", "Myanmar Kyat"}, {"MNT", "Mongolian Tögrög"},
    {"MOP", "Macanese Pataca"}, {"MRO", "Mauritanian Ouguiya (Old)"},
    {"MRU", "Mauritanian Ouguiya"}, {"MUR", "Mauritian Rupee"},
    {"MVR", "Maldivian Rufiyaa"}, {"MWK", "Malawian Kwacha"},
    {"MXN", "Mexican Peso"}, {"MYR", "Malaysian Ringgit"},
    {"MZN", "Mozambican Metical"}, {"NAD", "Namibian Dollar"},
    {"NGN", "Nigerian Naira"}, {"NIO", "Nicaraguan Córdoba"},
    {"NOK", "Norwegian Krone"}, {"NPR", "Nepalese Rupee"},
    {"NZD", "New Zealand Dollar"}, {"OMR", "Omani Rial"},
    {"PAB", "Panamanian Balboa"}, {"PEN", "Peruvian Sol"},
    {"PGK", "Papua New Guinean Kina"}, {"PHP", "Philippine Peso"},
    {"PKR", "Pakistani Rupee"}, {"PLN", "Polish Zloty"},
    {"PYG", "Paraguayan Guaraní"}, {"QAR", "Qatari Riyal"},
    {"RON", "Romanian Leu"}, {"RSD", "Serbian Dinar"}, {"RUB", "Russian Ruble"},
    {"RWF", "Rwandan Franc"}, {"SAR", "Saudi Riyal"},
    {"SBD", "Solomon Islands Dollar"}, {"SCR", "Seychellois Rupee"},
    {"SDG", "Sudanese Pound"}, {"SEK", "Swedish Krona"},
    {"SGD", "Singapore Dollar"}, {"SHP", "Saint Helena Pound"},
    {"SLE", "Sierra Leonean Leone"}, {"SOS", "Somali Shilling"},
    {"SRD", "Surinamese Dollar"}, {"SSP", "South Sudanese Pound"},
    {"STN", "São Tomé and Príncipe Dobra"}, {"SVC", "Salvadoran Colón"},
    {"SYP", "Syrian Pound"}, {"SZL", "Swazi Lilangeni"}, {"THB", "Thai Baht"},
    {"TJS", "Tajikistani Somoni"}, {"TMT", "Turkmenistan Manat"},
    {"TND", "Tunisian Dinar"}, {"TOP", "Tongan Paʻanga"},
    {"TRY", "Turkish Lira"}, {"TTD", "Trinidad and Tobago Dollar"},
    {"TWD", "New Taiwan Dollar"}, {"TZS", "Tanzanian Shilling"},
    {"UAH", "Ukrainian Hryvnia"}, {"UGX", "Ugandan Shilling"},
    {"USD", "United States Dollar"}, {"UYU", "Uruguayan Peso"},
    {"UZS", "Uzbekistani Som"}, {"VES", "Venezuelan Bolívar Soberano"},
    {"VND", "Vietnamese Đồng"}, {"VUV", "Vanuatu Vatu"}, {"WST", "Samoan Tālā"},
    {"XAF", "Central African CFA Franc"}, {"XAG", "Silver (troy ounce)"},
    {"XAU", "Gold (troy ounce)"}, {"XCD", "East Caribbean Dollar"},
    {"XCG", "Caribbean Guilder"}, {"XOF", "West African CFA Franc"},
    {"XPD", "Palladium (troy ounce)"}, {"XPF", "CFP Franc"},
    {"XPT", "Platinum (troy ounce)"}, {"YER", "Yemeni Rial"},
    {"ZAR", "South African Rand"}, {"ZMW", "Zambian Kwacha"},
    {"ZWG", "Zimbabwe Gold"}};

const size_t SUPPORTED_CURRENCIES_COUNT =
 sizeof(SUPPORTED_CURRENCIES) / sizeof(SUPPORTED_CURRENCIES[0]);

/**
 * Case-insensitive substring search.
 */
static const char* strcasestr_custom(const char* haystack, const char* needle)
{
	const char* hay_ptr = NULL;
	const char* nee_ptr = NULL;

	if (!*needle) {
		return haystack;
	}
	for (; *haystack; ++haystack) {
		if (tolower((unsigned char) *haystack) ==
		 tolower((unsigned char) *needle)) {
			hay_ptr = haystack;
			nee_ptr = needle;
			while (*hay_ptr && *nee_ptr) {
				if (tolower((unsigned char) *hay_ptr) !=
				 tolower((unsigned char) *nee_ptr)) {
					break;
				}
				++hay_ptr;
				++nee_ptr;
			}
			if (!*nee_ptr) {
				return haystack;
			}
		}
	}
	return NULL;
}

/**
 * Prints all supported currencies.
 */
void list_currencies(void)
{
	size_t idx;

	printf("Supported Currency Codes:\n");
	for (idx = 0; idx < SUPPORTED_CURRENCIES_COUNT; ++idx) {
		printf("%s - %s\n", SUPPORTED_CURRENCIES[idx].code,
		 SUPPORTED_CURRENCIES[idx].name);
	}
}

/**
 * Searches for currencies matching the query.
 */
void search_currencies(const char* query)
{
	int found = 0;
	size_t idx;

	printf("Search Results for '%s':\n", query);
	for (idx = 0; idx < SUPPORTED_CURRENCIES_COUNT; ++idx) {
		if (strcasestr_custom(SUPPORTED_CURRENCIES[idx].code, query) ||
		 strcasestr_custom(SUPPORTED_CURRENCIES[idx].name, query)) {
			printf("%s - %s\n", SUPPORTED_CURRENCIES[idx].code,
			 SUPPORTED_CURRENCIES[idx].name);
			found = 1;
		}
	}
	if (!found) {
		printf("No matching currency found.\n");
	}
}

/**
 * Checks if a currency code is supported.
 */
int is_supported_currency(const char* code)
{
	size_t idx;

	if (code == NULL) {
		return 0;
	}
	for (idx = 0; idx < SUPPORTED_CURRENCIES_COUNT; ++idx) {
		if (strcmp(SUPPORTED_CURRENCIES[idx].code, code) == 0) {
			return 1;
		}
	}
	return 0;
}

/**
 * Tests the internal currency list against the API for a base currency.
 */
void test_currencies_against_api(const char* base_currency)
{
	cJSON* json = NULL;
	const cJSON* item = NULL;
	const cJSON* quote = NULL;
	int missing_from_internal = 0;
	int missing_from_api = 0;
	size_t idx;
	const char* code = NULL;
	int found_in_api = 0;

	json = fetch_rates(base_currency, NULL);
	if (json == NULL) {
		fprintf(stderr, "Error: Failed to fetch rates for test.\n");
		return;
	}

	if (!cJSON_IsArray(json)) {
		fprintf(stderr, "Error: API response is not an array.\n");
		cJSON_Delete(json);
		return;
	}

	printf("Testing internal list against API (Base: %s)...\n", base_currency);

	/* 1. Check what's in API but not in Internal */
	printf("\nCurrencies in API but missing from internal list:\n");
	cJSON_ArrayForEach(item, json)
	{
		quote = cJSON_GetObjectItemCaseSensitive(item, "quote");
		if (cJSON_IsString(quote)) {
			if (!is_supported_currency(quote->valuestring)) {
				printf("  %s\n", quote->valuestring);
				missing_from_internal++;
			}
		}
	}
	if (missing_from_internal == 0) {
		printf("  (None)\n");
	}

	/* 2. Check what's in Internal but not in API */
	printf("\nInternal currencies not found in API response:\n");
	for (idx = 0; idx < SUPPORTED_CURRENCIES_COUNT; idx++) {
		code = SUPPORTED_CURRENCIES[idx].code;
		/* Skip the base currency itself as it won't be in the quotes */
		if (strcmp(code, base_currency) == 0) {
			continue;
		}

		found_in_api = 0;
		cJSON_ArrayForEach(item, json)
		{
			quote = cJSON_GetObjectItemCaseSensitive(item, "quote");
			if (cJSON_IsString(quote) &&
			 strcmp(quote->valuestring, code) == 0) {
				found_in_api = 1;
				break;
			}
		}
		if (!found_in_api) {
			printf("  %s (%s)\n", code, SUPPORTED_CURRENCIES[idx].name);
			missing_from_api++;
		}
	}
	if (missing_from_api == 0) {
		printf("  (None)\n");
	}

	printf("\nSummary:\n");
	printf("  Matches found: %d\n",
	 cJSON_GetArraySize(json) - missing_from_internal);
	printf("  Missing from internal: %d\n", missing_from_internal);
	printf("  Missing from API: %d\n", missing_from_api);

	cJSON_Delete(json);
}
