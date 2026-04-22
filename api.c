#include "api.h"

#include <curl/curl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define URL_MAX_SIZE 256

/**
 * Data structure to store the API response.
 */
struct MemoryStruct {
	char* memory;
	size_t size;
};

/**
 * Callback function for libcurl to store response data.
 */
static size_t
WriteMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
	size_t realsize = size * nmemb;
	struct MemoryStruct* mem = (struct MemoryStruct*) userp;

	char* ptr = realloc(mem->memory, mem->size + realsize + 1);
	if (ptr == NULL) {
		return 0;
	}

	mem->memory = ptr;
	memcpy(&(mem->memory[mem->size]), contents, realsize);
	mem->size += realsize;
	mem->memory[mem->size] = 0;

	return realsize;
}

/**
 * Helper function to configure CURL handle.
 */
static CURLcode perform_request(CURL* curl_handle, const char* url,
 struct MemoryStruct*
  chunk) // NOLINT(readability-function-cognitive-complexity)
{
	curl_easy_setopt(curl_handle, CURLOPT_URL, url);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void*) chunk);
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

	return curl_easy_perform(curl_handle);
}

/**
 * Fetches the rate(s) from Frankfurter API.
 */
cJSON* fetch_rates(const char* from_currency, const char* to_currency)
{
	CURL* curl_handle = NULL;
	CURLcode res;
	struct MemoryStruct chunk;
	cJSON* json = NULL;
	char url[URL_MAX_SIZE];
	int snprintf_res;

	chunk.memory = malloc(1);
	if (chunk.memory == NULL) {
		return NULL;
	}
	chunk.size = 0;

	curl_global_init(CURL_GLOBAL_ALL);
	curl_handle = curl_easy_init();

	if (curl_handle) {
		if (to_currency == NULL) {
			/* Fetch all rates for the base currency */
			snprintf_res = snprintf(url, sizeof(url),
			 "https://api.frankfurter.dev/v2/rates?base=%s", from_currency);
		} else {
			/* Fetch single rate */
			snprintf_res = snprintf(url, sizeof(url),
			 "https://api.frankfurter.dev/v2/rate/%s/%s", from_currency,
			 to_currency);
		}

		if (snprintf_res >= 0 && (size_t) snprintf_res < sizeof(url)) {
			res = perform_request(curl_handle, url, &chunk);
			if (res == CURLE_OK) {
				if (chunk.size > 0) {
					json = cJSON_Parse(chunk.memory);
				}
			} else {
				fprintf(stderr, "curl_easy_perform() failed: %s\n",
				 curl_easy_strerror(res));
			}
		}
		curl_easy_cleanup(curl_handle);
	}

	free(chunk.memory);
	curl_global_cleanup();

	return json;
}
