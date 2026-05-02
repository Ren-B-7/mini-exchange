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

static void print_help_cb(int argc, char** argv, void* user_data)
{
	(void) argc;
	(void) argv;
	const char* prog_name = (const char*) user_data;
	printf("Usage: %s [options] <value> <Current currency> <Desired "
	       "currency(ies)>\n",
	 prog_name);
	printf("\nOptions:\n");
	printf("  -h, --help        Show this help message\n");
	printf("  -l, --list        List all supported currency codes and names\n");
	printf("  -s, --search <q>  Search for a currency code or name\n");
	printf("  -t, --test <iso>  Test internal currency list against API for "
	       "base <iso>\n");
	printf("\nPositional Arguments:\n");
	printf("  <Desired currency(ies)> can be a single code (e.g., USD) or a "
	       "comma-separated list (e.g., EUR,ZAR,XCD)\n");
	printf("\nExample:\n");
	printf("  %s 100 USD EUR,ZAR,XCD\n", prog_name);
	exit(EXIT_SUCCESS);
}

static void list_currencies_cb(int argc, char** argv, void* user_data)
{
	(void) argc;
	(void) argv;
	(void) user_data;
	list_currencies();
	exit(EXIT_SUCCESS);
}

static void search_currencies_cb(int argc, char** argv, void* user_data)
{
	(void) user_data;
	if (argc < 1) {
		fprintf(stderr, "Error: Search query missing.\n");
		exit(EXIT_FAILURE);
	}
	search_currencies(argv[0]);
	exit(EXIT_SUCCESS);
}

static void test_currencies_cb(int argc, char** argv, void* user_data)
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
	    {"--help", "-h", "Show this help message", print_help_cb, argv[0]},
	    {"--list", "-l", "List all supported currency codes and names",
	        list_currencies_cb, NULL},
	    {"--search", "-s", "Search for a currency code or name",
	        search_currencies_cb, NULL},
	    {"--test", "-t", "Test internal currency list against API",
	        test_currencies_cb, NULL}};

	for (size_t i = 0; i < sizeof(args) / sizeof(CliArgument); ++i) {
		if (cli_add_argument(&parser, args[i]) != 0) {
			fprintf(stderr, "Error: Failed to add CLI argument.\n");
			cli_destroy(&parser);
			return EXIT_FAILURE;
		}
	}

	cli_parse(&parser, argc, argv);

	if (argc < 4) {
		fprintf(stderr, "Error: Invalid number of arguments.\n");
		print_help_cb(0, NULL, argv[0]);
		return EXIT_FAILURE;
	}

	char* endptr;
	double value = strtod(argv[1], &endptr);
	if (*endptr != '\0') {
		fprintf(stderr, "Error: Invalid value '%s'\n", argv[1]);
		return EXIT_FAILURE;
	}

	char current_currency[CURRENCY_CODE_LEN];
	snprintf(current_currency, sizeof(current_currency), "%s", argv[2]);
	uppercase_string(current_currency);

	if (!is_supported_currency(current_currency)) {
		fprintf(stderr, "Error: Unsupported current currency code '%s'.\n",
		 current_currency);
		return EXIT_FAILURE;
	}

	char target_currencies_str[TARGET_CURRENCIES_MAX_LEN];
	snprintf(target_currencies_str, sizeof(target_currencies_str), "%s",
	 argv[3]);

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
