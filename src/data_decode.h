#include "stdint-gcc.h"

typedef struct 
{
    float TotalVehicleHours;
    uint32_t Calendar;
    uint32_t TotalVehicleDistance;
    float vehicle_speed;
    float AmbientAirTemp;
    float Blow_by_Press;
    float AccelPedalPos;
    float Engine1Speed;
    float Engine_OilTemperature;
    uint32_t Engine_OilPressure;
    uint32_t CoolantTemperature;
    uint32_t E1FuelInjection;
    float FuelRate;
    uint32_t BoostPressure1;
    float EXHTemp_LF;
    float EXHTemp_LR;
    float EXHTemp_RF;
    float EXHTemp_RR;
    uint32_t TCLockupEngaged;
    float TMOilTemperature;
    uint32_t TMCurrentRange;
    float Front_Brake_Press;
    float Rear_Brake_Press;
    uint32_t FootBrakePosition;
    uint32_t RetarderOilTempF;
    uint32_t RetarderOilTempRR;
    uint32_t RetarderPosition;
    float PitchInclinometer;
    uint32_t BodySeating;
    float LiveWeight;
}data_to_write;

uint16_t id_decode( uint8_t *id);
uint32_t data_reverse(uint8_t *data, uint8_t data_lenth_code);
void update_struct ( data_to_write * datatowrite, uint16_t id, uint32_t data);