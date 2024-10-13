idf_component_register(SRCS "wifi.c" "main.c"
                    INCLUDE_DIRS ".")

spiffs_create_partition_image(spiffs ../spiffs FLASH_IN_PROJECT)