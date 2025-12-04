idf_component_register(SRCS "attack_dos.c" "attack_method.c" "main.c" "attack_handshake.c" "attack_pmkid.c" "attack.c"
                    INCLUDE_DIRS .
                    REQUIRES nvs_flash esp_wifi esp_event esp_timer wifi_controller frame_analyzer webserver
                    PRIV_REQUIRES wsl_bypasser pcap_serializer hccapx_serializer)
