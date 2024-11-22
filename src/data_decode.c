#include "data_decode.h"

uint16_t id_decode( uint8_t *id){
    uint16_t ID = id[1];
    uint16_t ID = ID<<8|id[0];
    return ID;
}

uint32_t data_reverse(uint8_t *data, uint8_t data_lenth_code){
    uint32_t reversed_data = 0;
    for (uint8_t i = 0; i < data_lenth_code-2; i++){uint32_t reversed_data = reversed_data<<8|data[3+i];}
    return reversed_data;
}


void update_struct ( data_to_write * datatowrite, uint16_t id, uint32_t data){
    switch (id)
    {
    case 101:  // Total Vehicle Hours
        datatowrite->TotalVehicleHours = data * 0.05;
        break;
    case 120:  // Calendar
        datatowrite->Calendar = data;
        break;
    case 140:  // Total Vehicle Distance
        datatowrite->TotalVehicleDistance = data * 5;
        break;
    case 150:  // Vehicle Speed
        datatowrite->vehicle_speed = data * 0.00390625;
        break;
    case 182:  // Ambient Air Temperature
        datatowrite->AmbientAirTemp = data * 0.03125 - 273; 
        break;
    case 202:  // Blow-by Pressure
        datatowrite->Blow_by_Press = data * 0.05; 
        break;
    case 203:  // Accel Pedal Position
        datatowrite->AccelPedalPos = data * 0.4;
        break;
    case 209:  // Engine 1 Speed
        datatowrite->Engine1Speed = data * 0.125; 
        break;
    case 211:  // Engine Oil Temperature
        datatowrite->Engine_OilTemperature = data * 0.03125 - 273; 
        break;
    case 213:  // Engine Oil Pressure
        datatowrite->Engine_OilPressure = data * 4; 
        break;
    case 222:  // Coolant Temperature
        datatowrite->CoolantTemperature = data - 40; 
        break;
    case 230:  // Engine Fuel Injection
        datatowrite->E1FuelInjection = data;
        break;
    case 231:  // Engine Fuel Rate
        datatowrite->FuelRate = data * 0.05;
        break;
    case 233:  // Boost Pressure 1
        datatowrite->BoostPressure1 = data * 2; 
        break;
    case 246:  // Exhaust Gas Temp 1 (Port 1)
        datatowrite->EXHTemp_LF = data * 0.03125 - 273; 
        break;
    case 247:  // Exhaust Gas Temp 2 (Port 2)
        datatowrite->EXHTemp_LR = data * 0.03125 - 273; 
        break;
    case 248:  // Exhaust Gas Temp 3 (Port 3)
        datatowrite->EXHTemp_RF = data * 0.03125 - 273; 
        break;
    case 249:  // Exhaust Gas Temp 4 (Port 4)
        datatowrite->EXHTemp_RR = data * 0.03125 - 273; 
        break;
    case 607:  // Torque Converter Lockup Engaged
        datatowrite->TCLockupEngaged = data; // No scaling provided
        break;
    case 612:
        datatowrite->TMOilTemperature = data * 0.03125 - 273;
        break;
    case 620:  // Transmission Current Range
        datatowrite->TMCurrentRange = data ; // Assuming resolution
        break;
    case 803:  // Front Brake Pressure
        datatowrite->Front_Brake_Press = data * 0.00390625; // Assuming resolution
        break;
    case 804:  // Rear Brake Pressure
        datatowrite->Rear_Brake_Press = data * 0.00390625; // Assuming resolution
        break;
    case 807:  // Foot Brake Position
        datatowrite->FootBrakePosition = data; // No scaling provided
        break;
    case 808:  // Retarder Oil Temperature Front
        datatowrite->RetarderOilTempF = data -40 ; // Assuming resolution and offset
        break;
    case 809:  // Retarder Oil Temperature Rear Right
        datatowrite->RetarderOilTempRR = data -40 ; // Assuming resolution and offset
        break;
    case 813:  // Retarder Position
        datatowrite->RetarderPosition = data; // No scaling provided
        break;
    case 2401:  // Pitch Inclinometer
        datatowrite->PitchInclinometer = data * 0.0078125 -200; // Assuming resolution
        break;
    case 2404: // Body Seating
        datatowrite->BodySeating = data; // No scaling provided
        break;
    case 2627: // Live Weight
        datatowrite->LiveWeight = data * 0.1; // Assuming resolution
        break;
    default:
        // Handle unknown id if necessary
        break;
    }
}
