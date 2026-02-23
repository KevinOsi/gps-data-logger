idf_component_register(SRCS "main.c" "telemetry_task.c" "gps_parser.c" "hw_config.c" "i2c_manager.c" "bme280/bme280.c" "bme280_handler.c"
                    INCLUDE_DIRS "." "bme280")
