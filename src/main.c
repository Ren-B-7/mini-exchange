#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "api.h"
#include "convert.h"
#include "currencies.h"
#include "include/minicli.h"

#define CURRENCY_CODE_LEN 8
#define TARGET_CURRENCIES_MAX_LEN 256

static int list_currencies_cb(int argc, char** argv, void* user_data)
{
	(void) argc;
	(void) argv;
	(void) user_data;
	list_currencies();
	exit(EXIT_SUCCESS);
	return 0;
}

static int search_currencies_cb(int argc, char** argv, void* user_data)
{
	(void) user_data;
	if (argc < 1) {
		fprintf(stderr, "Error: Search query missing.\n");
		exit(EXIT_FAILURE);
	}
	search_currencies(argv[0]);
	exit(EXIT_SUCCESS);
	return 1;
}

static int test_currencies_cb(int argc, char** argv, void* user_data)
{
	(void) user_data;
	if (argc < 1) {
		fprintf(stderr, "Error: ISO code missing for test.\n");
		exit(EXIT_FAILURE);
	}
	char test_base[CURRENCY_CODE_LEN];
	snprintf(test_base, sizeof(test_base), "%s", argv[0]);
	for (char* ptr = test_base; *ptr != '\0'; ++ptr) {
		*ptr = (char) toupper((unsigned char) *ptr);
	}
	if (!is_supported_currency(test_base)) {
		fprintf(stderr, "Error: Unsupported current currency code '%s'.\n",
		 test_base);
		exit(EXIT_FAILURE);
	}
	test_currencies_against_api(test_base);
	exit(EXIT_SUCCESS);
	return 1;
}

static void uppercase_string(char* str)
{
	if (str == NULL) {
		return;
	}
	for (char* ptr = str; *ptr != '\0'; ++ptr) {
		*ptr = (char) toupper((unsigned char) *ptr);
	}
}

int main(int argc, char* argv[])
{
	CliParser parser;
	CliInitParams params = {"exchange", "Currency exchange tool"};
	cli_init(&parser, params);

	CliArgument args[] = {
	    {"--list", "-l", "List all supported currency codes and names",
	        list_currencies_cb, NULL},
	    {"--search", "-s", "Search for a currency code or name",
	        search_currencies_cb, NULL},
	    {"--test", "-t", "Test internal currency list against API",
	        test_currencies_cb, NULL}};

	for (size_t i = 0; i < sizeof(args) / sizeof(CliArgument); ++i) {
		cli_add_argument(&parser, args[i]);
	}

	int consumed = cli_parse(&parser, argc, argv);

	if (argc - consumed < 3) {
		fprintf(stderr, "Error: Invalid number of arguments.\n");
		printf("Usage: %s [options] <value> <base_currency> "
		       "<target_currencies>\n",
		 argv[0]);
		return EXIT_FAILURE;
	}

	char* endptr;
	double value = strtod(argv[consumed], &endptr);
	if (*endptr != '\0') {
		fprintf(stderr, "Error: Invalid value '%s'\n", argv[consumed]);
		return EXIT_FAILURE;
	}

	char current_currency[CURRENCY_CODE_LEN];
	snprintf(current_currency, sizeof(current_currency), "%s",
	 argv[consumed + 1]);
	uppercase_string(current_currency);

	if (!is_supported_currency(current_currency)) {
		fprintf(stderr, "Error: Unsupported current currency code '%s'.\n",
		 current_currency);
		return EXIT_FAILURE;
	}

	char target_currencies_str[TARGET_CURRENCIES_MAX_LEN];
	snprintf(target_currencies_str, sizeof(target_currencies_str), "%s",
	 argv[consumed + 2]);

	cJSON* json = NULL;
	if (strchr(target_currencies_str, ',') != NULL) {
		json = fetch_rates(current_currency, NULL);
	} else {
		uppercase_string(target_currencies_str);
		if (!is_supported_currency(target_currencies_str)) {
			fprintf(stderr, "Error: Unsupported current currency code '%s'.\n",
			 target_currencies_str);
			return EXIT_FAILURE;
		}
		json = fetch_rates(current_currency, target_currencies_str);
	}

	if (json == NULL) {
		fprintf(stderr, "Error: Failed to fetch exchange rates.\n");
		return EXIT_FAILURE;
	}

	char* current_pos = target_currencies_str;
	while (current_pos != NULL && *current_pos != '\0') {
		char* next_comma = strchr(current_pos, ',');
		if (next_comma != NULL) {
			*next_comma = '\0';
		}

		if (*current_pos != '\0') {
			uppercase_string(current_pos);
			if (!is_supported_currency(current_pos)) {
				fprintf(stderr,
				 "Error: Unsupported target currency code '%s'.\n",
				 current_pos);
			} else {
				double rate = get_rate_for_currency(json, current_pos);
				if (rate >= 0) {
					printf("%.2f %s = %.2f %s\n", value, current_currency,
					 value * rate, current_pos);
				} else {
					fprintf(stderr,
					 "Error: Failed to get rate for '%s' from API.\n",
					 current_pos);
				}
			}
		}
		current_pos = (next_comma != NULL) ? next_comma + 1 : NULL;
	}

	cJSON_Delete(json);
	cli_destroy(&parser);
	return EXIT_SUCCESS;
}
