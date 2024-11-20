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

        printf("URL: %s\n", url);

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

        res = curl_easy_perform(curl);

        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() falló: %s\n", curl_easy_strerror(res));
            forecast->tempPronostico = 0.0;
            forecast->humedadPronostico = 0.0;
            forecast->probabilidadLluvia = 0;
            strncpy(forecast->condicionClima, "Error", sizeof(forecast->condicionClima) - 1);
            forecast->esHoraDia = 0;
        } else {
            printf("Respuesta: %s\n", chunk.memory);
            
            struct json_object *parsed_json;
            struct json_object *array_item;
            struct json_object *temperature_obj;
            struct json_object *temp_value;

            parsed_json = json_tokener_parse(chunk.memory);
            
            // Obtener el primer elemento del array
            array_item = json_object_array_get_idx(parsed_json, 0);
            
            if(array_item != NULL) {
                // Obtener temperatura
                json_object_object_get_ex(array_item, "Temperature", &temperature_obj);
                if(temperature_obj != NULL) {
                    json_object_object_get_ex(temperature_obj, "Value", &temp_value);
                    forecast->tempPronostico = json_object_get_double(temp_value);
                }

                // Obtener condición del clima
                struct json_object *icon_phrase;
                json_object_object_get_ex(array_item, "IconPhrase", &icon_phrase);
                if(icon_phrase != NULL) {
                    strncpy(forecast->condicionClima, 
                           json_object_get_string(icon_phrase), 
                           sizeof(forecast->condicionClima) - 1);
                }

                // Obtener probabilidad de lluvia
                struct json_object *precip_prob;
                json_object_object_get_ex(array_item, "PrecipitationProbability", &precip_prob);
                if(precip_prob != NULL) {
                    forecast->probabilidadLluvia = json_object_get_int(precip_prob);
                    forecast->humedadPronostico = json_object_get_double(precip_prob);
                }

                // Verificar si es de día
                struct json_object *is_daylight;
                json_object_object_get_ex(array_item, "IsDaylight", &is_daylight);
                if(is_daylight != NULL) {
                    forecast->esHoraDia = json_object_get_boolean(is_daylight);
                }

                printf("\nPronóstico detallado:\n");
                printf("Temperatura: %.2f°C\n", forecast->tempPronostico);
                printf("Condición: %s\n", forecast->condicionClima);
                printf("Probabilidad de lluvia: %d%%\n", forecast->probabilidadLluvia);
                printf("Es hora del día: %s\n", forecast->esHoraDia ? "Sí" : "No");
                
                printf("\nValores extraídos del JSON:\n");
                printf("Temperatura: %.2f°C\n", forecast->tempPronostico);
                printf("Probabilidad de precipitación: %.2f%%\n", forecast->humedadPronostico);
            } else {
                forecast->tempPronostico = 0.0;
                forecast->humedadPronostico = 0.0;
                forecast->probabilidadLluvia = 0;
                strncpy(forecast->condicionClima, "No data", sizeof(forecast->condicionClima) - 1);
                forecast->esHoraDia = 0;
            }

            json_object_put(parsed_json);
        }

        curl_easy_cleanup(curl);
    }

    free(chunk.memory);
}