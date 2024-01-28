#include "CAN/Can_Interface.hpp"
#include <stdio.h>
#include <string.h>
#include <freertos/task.h>

uint8_t electric_module_frame_number = 0;
uint8_t data_module_frame_number = 0;
struct ElectricModule ElectricModuleData;
struct DataModule DataModuleData;

void CanInit(gpio_num_t tx_pin, gpio_num_t rx_pin, uint32_t acceptance_code, uint32_t acceptance_mask)
{
    //Initialize configuration structures using macro initializers
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(tx_pin, rx_pin, TWAI_MODE_NORMAL);
    g_config.tx_queue_len = TX_QUEUE_LEN;
    g_config.rx_queue_len = RX_QUEUE_LEN;
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_1MBITS();
    twai_filter_config_t f_config = {.acceptance_code = acceptance_code,
                                     .acceptance_mask = acceptance_mask, 
                                     .single_filter = true};

    //Install TWAI driver
    ESP_LOGI(CAN_TAG, "Installing CAN");
    ESP_ERROR_CHECK( twai_driver_install(&g_config, &t_config, &f_config));

    //Start TWAI driver
    ESP_LOGI(CAN_TAG, "Installing CAN");
    ESP_ERROR_CHECK(twai_start());
}


uint8_t CheckStateCAN(uint8_t func)
{
    //get TWAI driver status
    twai_status_info_t twai_status;
    ESP_ERROR_CHECK_WITHOUT_ABORT(twai_get_status_info(&twai_status));
    switch (twai_status.state)
    {
    case TWAI_STATE_STOPPED:
        ESP_ERROR_CHECK_WITHOUT_ABORT(twai_start());
        ESP_LOGI(CAN_TAG, "Wrong twai driver state \n");
        return CAN_FAIL;
    
    case TWAI_STATE_BUS_OFF:
        twai_initiate_recovery();
        ESP_LOGI(CAN_TAG, "CAN transmiter not working \n");
        return CAN_FAIL;
    
    case TWAI_STATE_RECOVERING:
        ESP_LOGI(CAN_TAG, "CAN recovering \n");
        return CAN_FAIL;

    default:
        
        if (!(twai_status.msgs_to_tx < TX_QUEUE_LEN) && func == TRANSMITION)
        {
            ESP_LOGI(CAN_TAG, "CAN tx buffer is full");
            return CAN_FAIL;
        }
        else if (!(twai_status.msgs_to_tx < RX_QUEUE_LEN) && func == RECEIVE)
        {
            ESP_LOGI(CAN_TAG, "CAN rx buffer is full");
            return CAN_FAIL;
        }
        
        return CAN_SUCCESS;
    }
}


uint8_t CanSendMessage(uint32_t module_identifier, uint8_t* data, uint8_t data_lenght)
{
    if (CheckStateCAN(TRANSMITION) != CAN_SUCCESS)
    {
        return CAN_FAIL;
    }
    
    //Configure message to transmit
    twai_message_t message;
    message.identifier = module_identifier;
    message.extd = 0;
    message.rtr = 0;
    message.self = 0;
    message.ss = 0;
    
    message.data_length_code = data_lenght;
    for (int i = 0; i < data_lenght; i++) 
    {
        message.data[i] = data[i];
    }
    
    //Queue message for transmission
    esp_err_t error_tw = twai_transmit(&message, pdMS_TO_TICKS(1000));

    if (error_tw != ESP_OK) 
    {
        printf("Failed to queue message for transmission: %d\n", error_tw);
        for (int i = 0; i < data_lenght; i++) 
        {
            printf("%u \n", data[i]);
        }
        return CAN_FAIL;
    }

    return CAN_SUCCESS;
}


uint8_t receiveMessage(twai_message_t *rx_message, uint16_t ticks_to_wait)
{
    if (CheckStateCAN(RECEIVE) != CAN_SUCCESS)
    {
        return CAN_FAIL;
    }
    
    // set messege parameters
    rx_message -> extd = 0;
    rx_message -> rtr = 0;

    if (twai_receive(rx_message, pdMS_TO_TICKS(ticks_to_wait)) != ESP_OK) 
    {
        printf("Failed to receive message\n");
        return CAN_FAIL;
    }

    //Process received message
    if (rx_message -> extd) {
        printf("Message is in Extended Format\n");
    } else {
        printf("Message is in Standard Format\n");
    }
    printf("ID is %ld\n", rx_message -> identifier);
    if (!(rx_message -> rtr)) {
        for (int i = 0; i < rx_message -> data_length_code; i++) {
            printf("Data byte %d = %d\n", i, (int)rx_message -> data[i]);
        }
    }
    return CAN_SUCCESS;
}


uint8_t initConversationMasterSlaveCAN(uint8_t module_id)
{   
    twai_message_t rx_message;
    
    //send init message to module and wait for the answer
    uint8_t init_conversation_data[] = {INIT_CONVERSATION_MASTER, module_id};
    uint8_t data_lenght = 2;
    uint8_t new_init_message_iterator = 0;

    do{
        if (new_init_message_iterator == 0)
        {
            CanSendMessage(COMMUNICATION_ID, init_conversation_data, data_lenght);
            new_init_message_iterator = 4;
        }
        receiveMessage(&rx_message, 500);

        new_init_message_iterator--;
    }while (rx_message.data[0] != INIT_CONVERSATION_SLAVE);

    // set number of messurements for each module
    if(rx_message.identifier == ELECTRIC_ID)
    {
        electric_module_frame_number = rx_message.data[1];
    }
    else if (rx_message.identifier == DATA_ID)
    {
        data_module_frame_number = rx_message.data[1];
    }
    else
    {
        return CAN_FAIL;
    }

    return CAN_SUCCESS;
}


uint8_t initConversationMasterCAN()
{
    // init CAN drive
    CanInit(TX_CAN_PIN, RX_CAN_PIN);

    // Init slaves
    if(initConversationMasterSlaveCAN(ELECTRIC_ID) != CAN_SUCCESS)
    {
        ESP_LOGI(CAN_TAG, "Failed to init Electric module");
        return ESP_FAIL;  //initialization failed
    }
    ESP_LOGI(CAN_TAG, "Electric unit success");

    if(initConversationMasterSlaveCAN(DATA_ID) != CAN_SUCCESS)
    {
        ESP_LOGI(CAN_TAG, "Failed to init Data module");
        return ESP_FAIL;
    }
    ESP_LOGI(CAN_TAG, "data unit success");


    return ESP_OK;
}


uint8_t saveCanDataFrame(twai_message_t rx_message)
{
    switch (rx_message.data[0])
    {
    case TEMPERATURE:
    {
        uint16_t temp = (rx_message.data[2] << 8) | rx_message.data[3];
        DataModuleData.temperature[rx_message.data[1]] = temp;
        break;
    }
    case HYDROGEN_USAGE:
    {
        uint16_t measurement = (rx_message.data[1] << 8) | rx_message.data[2];
        DataModuleData.hydrogen_usage = measurement ;
        break;
    }
    case HUMIDITY:
        DataModuleData.humidity = rx_message.data[1];
        break;
    
    case ERROR:
        DataModuleData.error = rx_message.data[1];
        break;
    
    case SPEED:
        DataModuleData.speed = rx_message.data[1];
        break;
    
    case DATA_MESSURMENTS_TIME:
        DataModuleData.time = rx_message.data[1];
        break;

    default:
        return CAN_FAIL;
    }
    return CAN_SUCCESS;
}


uint8_t saveCanElecticFrame(twai_message_t rx_message)
{
    switch (rx_message.data[0])
    {
    case VOLTAGE_VALUE:
    {
        uint16_t voltage = (rx_message.data[2] << 8) | rx_message.data[3];
        ElectricModuleData.voltage[rx_message.data[1]] = voltage;
        break;
    }
    case CURRENT_VALUE:
    {
        uint16_t current = (rx_message.data[2] << 8) | rx_message.data[3];
        ElectricModuleData.current[rx_message.data[1]] = current;
        break;
    }
    case AMBIENT_TEMPERATURE:
        ElectricModuleData.ambient_temperature = rx_message.data[1];
        break;
    
    case ERROR:
        ElectricModuleData.error = rx_message.data[1];
        break;
    
    case H2_LEAK:
        ElectricModuleData.h2_leak = rx_message.data[1];
        break;
    
    case CELL_TEMP:
        ElectricModuleData.cell_temp = rx_message.data[1];
        break;
    
    case ELECTRIC_MESSURMENTS_TIME:
        ElectricModuleData.time = rx_message.data[1];
        break;

    default:
        return CAN_FAIL;
    }
    return CAN_SUCCESS;
}


uint8_t getDataFromSingleModule(uint8_t module_id)
{
    twai_message_t rx_message;
    
    //send message to selected module and wait for the answer
    uint8_t collect_conversation_data[] = {COLLECT_DATA, module_id};
    uint8_t data_lenght = 2;
    CanSendMessage(COMMUNICATION_ID, collect_conversation_data, data_lenght);
    
    uint8_t messurement_number = 0;

    // checks how much data should be received
    if(module_id == ELECTRIC_ID)
    {
        messurement_number = electric_module_frame_number;
    } else if (module_id == DATA_ID)
    {
        messurement_number = data_module_frame_number;
    }

    // collect all data frames, if all results are not received, 
    // whole operation failes
    for(uint8_t i = 0; i < messurement_number; i++)
    {
        if(receiveMessage(&rx_message, 50) == CAN_FAIL) { return CAN_FAIL; }
        
        if(module_id == DATA_ID)
        {
            if(saveCanDataFrame(rx_message) == CAN_FAIL)
            {
                vTaskDelay(pdMS_TO_TICKS(10));
                twai_clear_receive_queue();
                return CAN_FAIL; 
            }

        } else if (module_id == ELECTRIC_ID)
        {
            if(saveCanElecticFrame(rx_message) == CAN_FAIL)
            {
                vTaskDelay(pdMS_TO_TICKS(10));
                twai_clear_receive_queue();
                return CAN_FAIL; 
            }
        }
    }
    return CAN_SUCCESS;
}


uint8_t collectData()
{
    uint8_t result_data;
    uint8_t result_electric;
    uint8_t index = 3;

    do{
        result_electric = getDataFromSingleModule(ELECTRIC_ID);
        index--;
    }while (result_electric != CAN_SUCCESS && index != 0);
    
    index = 3;
    do{
        result_data = getDataFromSingleModule(DATA_ID);
        index--;
    }while (result_data != CAN_SUCCESS && index != 0);


    if(result_data == CAN_FAIL)
    {
        ESP_LOGI(CAN_TAG, "Couldn't communicate with data module");
        memset(&DataModuleData, 0, sizeof(struct DataModule)); //reset data
        DataModuleData.error = COULDNT_COLLECT_DATA;
    }
    else if(result_electric == CAN_FAIL)
    {
        ESP_LOGI(CAN_TAG, "Couldn't communicate with Electric module");
        memset(&ElectricModuleData, 0, sizeof(struct ElectricModule)); //reset data
        ElectricModuleData.error = COULDNT_COLLECT_DATA;
    }

    return CAN_SUCCESS;
}


void initConversationSlaveCAN(uint8_t number_of_messurements, uint8_t module_id)
{
    twai_message_t rx_message;

    uint8_t init_conversation[] = {INIT_CONVERSATION_SLAVE, number_of_messurements};
    uint8_t data_lenght = sizeof(init_conversation)/sizeof(init_conversation[0]);

    do{
        receiveMessage(&rx_message, 2000);
    }while (rx_message.data[0] != INIT_CONVERSATION_MASTER);
    CanSendMessage(module_id, init_conversation, data_lenght);
    
    ESP_LOGI(CAN_TAG, "Can connected");
}