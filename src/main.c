#include "string.h"
#include "sys/unistd.h"
#include "sys/stat.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"
#include "driver/gpio.h"
#include "driver/twai.h"

//LOGGGING DEF 
static const char *SDCARDTAG = "SD_CARD";
static const char *CANTAG = "CAN_TAG";

FILE *log_CAN;

int print_to_SD(const char * something, va_list args){
    if (log_CAN == NULL)
    {
        printf("%s() ABORT, FIle pointer is not yet loaded \n", __FUNCTION__);
        return -1;
    }
    vfprintf(log_CAN,something,args);
    return vprintf(something,args);
}

void app_main()
{
    //Initialize CAN BUS Config
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(GPIO_NUM_33, GPIO_NUM_32, TWAI_MODE_NORMAL); //Change to listen mode only
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_250KBITS();
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
    // twai_filter_config_t f_config_HD785 ={
    //     .acceptance_code = (0x18ff368f << 3),
    //     .acceptance_mask = 0x00,
    //     .single_filter = 1
    // };

    // twai_filter_config_t f_config_allbc ={
    //     .acceptance_code = (0x00ff0000 << 3),
    //     .acceptance_mask = 0xF807FFFF,
    //     .single_filter = 1
    // };

    // twai_filter_config_t f_config_bcsa = {
    //     .acceptance_code = (0x00FF008F<<3),
    //     .acceptance_mask = 0xF807F807,
    //     .single_filter = 1
    // };

    //Install TWAI driver
    if (twai_driver_install(&g_config, &t_config, &f_config) == ESP_OK) {
        ESP_LOGI(CANTAG,"Driver installed");
    } else {
        ESP_LOGI(CANTAG,"Failed to install driver");
        return;
    }

    //Start TWAI driver
    if (twai_start() == ESP_OK) {
        ESP_LOGI(CANTAG,"Driver started");
        } else {
        ESP_LOGI(CANTAG,"Failed to start driver");
        return;
    }


    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024
    };
    sdmmc_card_t *card;
    ESP_LOGI(SDCARDTAG,"Initializing SD CARD");
    ESP_LOGI(SDCARDTAG,"Using SDMMC Peripheral");
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    sdmmc_slot_config_t slot_config  = SDMMC_SLOT_CONFIG_DEFAULT();
    slot_config.width = 4;
    slot_config.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;
    
    ESP_LOGI(SDCARDTAG,"Mounting Filesystem");
    esp_err_t mounterr = esp_vfs_fat_sdmmc_mount("/test1",&host,&slot_config,&mount_config, &card);

     if (mounterr != ESP_OK) {
        if (mounterr == ESP_FAIL) {
            ESP_LOGE(SDCARDTAG, "Failed to mount filesystem. "
                     "If you want the card to be formatted, set the EXAMPLE_FORMAT_IF_MOUNT_FAILED menuconfig option.");
        } else {
            ESP_LOGE(SDCARDTAG, "Failed to initialize the card (%s). "
                     "Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(mounterr));    
        }
        return;
     }
    ESP_LOGI(SDCARDTAG,"FIlesystem mounted");
    sdmmc_card_print_info(stdout,card);
    ESP_LOGI(SDCARDTAG,"DONE");

    //const char *file_baru = "/test1/sesuatu.txt";
    const char *file_log = "/test1/LOG.txt";

    // char data[]= "The swaying shades of acacias \n"
    //              "In flower arrangements suggest \n"
    //              "That it's time for me to be a man \n"
    //              "Or else with what regrets \n"
    //              "Would I be left to live forever \n";

    // ESP_LOGI(SDCARDTAG, "Opening file %s", file_baru);
    // FILE *f = fopen(file_baru, "w");
    // if (f == NULL) {
    //     ESP_LOGE(SDCARDTAG, "Failed to open file f  or writing");
    // }
    // fprintf(f, data);
    // fclose(f);
    // ESP_LOGI(SDCARDTAG, "File written...");

    ESP_LOGI(SDCARDTAG,"SWITCHING TO PRINT LOGS INTO SDCARD");
    esp_log_set_vprintf(&print_to_SD);
    
    while (1)
    {
        log_CAN = fopen(file_log,"a");
        twai_message_t message;
        if (twai_receive(&message, pdMS_TO_TICKS(10000)) == ESP_OK) {
            char* CAN_data = calloc(500,sizeof(char));
            char* temp = CAN_data;
            CAN_data += sprintf(CAN_data,"ID is %lx : ",message.identifier);
            CAN_data += sprintf(CAN_data,"Data :");
            for (int i = 0; i < message.data_length_code; i++) {
                CAN_data += sprintf(CAN_data," %x",message.data[i]);
            }
            ESP_LOGI(SDCARDTAG,"%s",temp);
            free(temp);
        } else {
            ESP_LOGI(SDCARDTAG,"Failed to receive message");
        }
        fclose(log_CAN);
        //vTaskDelay(3000/portTICK_PERIOD_MS);
    }       
}
