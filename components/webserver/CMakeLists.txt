idf_component_register(SRCS "webserver.c" "file_manager.c"
                    INCLUDE_DIRS "interface" "."
                    PRIV_REQUIRES hccapx_serializer pcap_serializer esp_http_server wifi_controller main spiffs)