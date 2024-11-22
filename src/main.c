#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_task_wdt.h"
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

QueueHandle_t writequeue;
FILE *log_CAN;


void vTaskCANReceive(void* pvParameter){
    twai_message_t message;
    uint8_t data[8]={0};
    for(;;){
         if (twai_receive(&message, pdMS_TO_TICKS(1000)) == ESP_OK) {
            ESP_LOGI(CANTAG,"%lx : ",message.identifier);
            for (int i = 0; i < message.data_length_code; i++) {
                printf(" %x",message.data[i]);
            }
            memcpy(data,message.data,sizeof(message.data));
            if (xQueueSendToBack(writequeue,(void *)&data,pdMS_TO_TICKS(1000)) != pdPASS)
            {
                printf("Queue not sent \n");
            }
            memset(&data,0,sizeof(data));          
        } else {
            ESP_LOGI(CANTAG,"Failed to receive message");
        }
    }
}

void vTask_Data_Decode(void *pvParameter){
    uint8_t data[8]={0};
    for (;;)
    {
        if(writequeue != NULL ){
            if(xQueueReceive(writequeue,(void*)&data,pdMS_TO_TICKS(1000))==pdTRUE){
                
            }
        }else
        {
            printf("Queue is NULL \n");
            vTaskDelay(pdMS_TO_TICKS(100));
        }
    }
    
}

void vWriteToSD(void* pvParameter){
    uint8_t data[8]={0};
    const char *file_log = "/test1/LOG.txt";
    TickType_t currentTick;
    uint32_t tickinms;
    for(;;){
        if (writequeue != NULL)
        {
            if (xQueueReceive(writequeue,(void*)&data,pdMS_TO_TICKS(1000))==pdTRUE)
            {
                currentTick = xTaskGetTickCount();
                tickinms = currentTick *portTICK_PERIOD_MS;
                log_CAN = fopen(file_log,"a");
                fprintf(log_CAN,"%ld : %02x %02x %02x %02x %02x %02x %02x %02x \n", tickinms, data[0], data[1], data[2], data[3], data[4], data[5], data[6], data[7]);
                fclose(log_CAN);
                memset(&data,0,sizeof(data));
                vTaskDelay(10);
            }
        }else
        {
            printf("Queue is null \n");
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
    }
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

    uint8_t data[8];
    writequeue = xQueueCreate(10,sizeof(data));
    xTaskCreatePinnedToCore(vTaskCANReceive,"CAN Receiver Task",2048,NULL,0,NULL,0);
    xTaskCreatePinnedToCore(vWriteToSD,"Writing to SD CARD",2048,NULL,0,NULL,1);
     
}
