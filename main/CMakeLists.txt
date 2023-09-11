set(sources "C:/Repos/Animal_Feeder/FTF_display/jpeg_decoder.c")
set(includes "C:/Repos/Animal_Feeder/FTF_display")

# Compile only when cannot use ROM code
if(NOT CONFIG_JD_USE_ROM)
    list(APPEND sources "C:/Repos/Animal_Feeder/FTF_display/tjpgd.c")
    list(APPEND includes "C:/Repos/Animal_Feeder/FTF_display")
endif()

idf_component_register(SRCS "main.c" "C:/Repos/Animal_Feeder/RCT/i2cdev/i2cdev.c"
                            "C:/Repos/Animal_Feeder/RCT/ds3231/ds3231.c"
                            "C:/Repos/Animal_Feeder/main/Main_Screen/Main_Screen.c"
                            "C:/Repos/Animal_Feeder/main/Alarm_menu/Alarm_menu.c"
                            "C:/Repos/Animal_Feeder/main/Button_Handler/Button_Handler.c"
                            "C:/Repos/Animal_Feeder/FTF_display/simple_gui.c" 
                            "C:/Repos/Animal_Feeder/FTF_display/spi_config.c" 
                            "C:/Repos/Animal_Feeder/FTF_display/spi_lcd.c"
                            "C:/Repos/Animal_Feeder/FTF_display/pretty_effect.c" 
                            "C:/Repos/Animal_Feeder/FTF_display/decode_image.c"
                            
                            
                            "C:/Repos/Animal_Feeder/main/Pwm_motor/Pwm_motor.c"               
                    INCLUDE_DIRS "." "C:/Repos/Animal_Feeder/RCT/ds3231"
                    "C:/Repos/Animal_Feeder/RCT/i2cdev"
                    "C:/Repos/Animal_Feeder/RCT/esp_idf_lib_helpers"
                    "C:/Repos/Animal_Feeder/main/Main_Screen"
                    "C:/Repos/Animal_Feeder/main/Alarm_menu"
                    "C:/Repos/Animal_Feeder/main/Button_Handler"
                    "C:/Repos/Animal_Feeder/FTF_display"
                    "C:/Repos/Animal_Feeder/main/Pwm_motor"
                    EMBED_FILES image.jpg )
