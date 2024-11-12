#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <json-c/json.h>
#include "prediccion_clima.h"

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if(!ptr) {
        printf("Error: sin memoria suficiente\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

void obtener_pronostico(WeatherForecast *forecast) {
    CURL *curl;
    CURLcode res;
    struct MemoryStruct chunk;
    
    chunk.memory = malloc(1);
    chunk.size = 0;

    curl = curl_easy_init();
    if(curl) {
        char url[256];
        snprintf(url, sizeof(url), 
                "http://dataservice.accuweather.com/forecasts/v1/hourly/1hour/%s?apikey=%s&metric=true",
                LOCATION_KEY, API_KEY);

        // Print de la URL
        printf("URL: %s\n", url);

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

        res = curl_easy_perform(curl);

        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() falló: %s\n", curl_easy_strerror(res));
            forecast->tempPronostico = 0.0;
            forecast->humedadPronostico = 0.0;
        } else {
            // Print de la respuesta
            printf("Respuesta: %s\n", chunk.memory);
            
            struct json_object *parsed_json;
            struct json_object *temperature;
            struct json_object *relative_humidity;

            parsed_json = json_tokener_parse(chunk.memory);
            
            struct json_object *forecast_data = json_object_array_get_idx(parsed_json, 0);
            
            if(forecast_data != NULL) {
                json_object_object_get_ex(forecast_data, "Temperature", &temperature);
                json_object_object_get_ex(forecast_data, "RelativeHumidity", &relative_humidity);

                if(temperature != NULL && relative_humidity != NULL) {
                    struct json_object *temp_value;
                    json_object_object_get_ex(temperature, "Value", &temp_value);
                    forecast->tempPronostico = json_object_get_double(temp_value);
                    forecast->humedadPronostico = json_object_get_double(relative_humidity);
                } else {
                    forecast->tempPronostico = 0.0;
                    forecast->humedadPronostico = 0.0;
                }
            } else {
                forecast->tempPronostico = 0.0;
                forecast->humedadPronostico = 0.0;
            }

            json_object_put(parsed_json);
        }

        curl_easy_cleanup(curl);
    }

    free(chunk.memory);
}