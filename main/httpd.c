/*
 * httpd.c - This file is part of MagRotator.
 *
 * Copyright (C) 2024, Yannick Seibert. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the author nor the names of its contributors may
 *    be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <esp_log.h>
#include <stdio.h>
#include "httpd.h"

// Tag for logging
static const char *TAG = "httpd.c";
static char buffer[10240];

void http_read_file(const char* file_path, char* buffer, int buflen) {
    FILE* file = fopen(file_path, "r");
    if (file == NULL) {
        ESP_LOGE(TAG, "Failed to open HTML file");
        return;
    }
    
    long length = fsize(file);
    char* data = malloc(length);
    fread(data, 1, length, file);
    fclose(file);
    
    strncpy(buffer, data, buflen);
    free(data);
}

// Handler for the root URL
esp_err_t root_get_handler(httpd_req_t *req) {
    const char resp[] = "Hello, world!";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

// Handler for a specific URL
esp_err_t magloop_status_get_handler(httpd_req_t *req) {
    http_read_file("/spiffs/index.html", buffer, sizeof(buffer));    
    httpd_resp_send(req, buffer, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

// Registering URI handlers
void register_handlers(httpd_handle_t server) {
    httpd_uri_t root_uri = {
        .uri      = "/",
        .method   = HTTP_GET,
        .handler  = root_get_handler,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &root_uri);

    httpd_uri_t hello_uri = {
        .uri      = "/magloop-status",
        .method   = HTTP_GET,
        .handler  = magloop_status_get_handler,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &hello_uri);
}

httpd_handle_t start_webserver(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    if (httpd_start(&server, &config) == ESP_OK)
        register_handlers(server);
    else
        ESP_LOGE(TAG, "Error starting HTTP-Server!");
    
    return server;
}
