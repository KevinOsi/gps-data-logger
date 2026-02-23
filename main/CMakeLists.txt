idf_component_register(SRCS "main.c" "telemetry_task.c" "gps_parser.c" "hw_config.c" "i2c_manager.c" "bme280/bme280.c" "bme280_handler.c" "ssd1306/ssd1306.c" "oled_handler.c" "ui_task.c"
                    INCLUDE_DIRS "." "bme280" "ssd1306")
