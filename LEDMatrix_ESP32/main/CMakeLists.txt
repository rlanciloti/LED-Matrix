idf_component_register(
    SRCS 
        "commands.c"
        "console.c"
        "esp.c"
        "gpio_helper.c"
        "led_helper.c"
        "main.c"
        "wifi_helper.c"
        "../color/color.c"
        "../networking/messages.c"
        "../utils/builtin_animations.c"
    INCLUDE_DIRS 
        "."
        "../color"
        "../networking"
        "../utils"
)
