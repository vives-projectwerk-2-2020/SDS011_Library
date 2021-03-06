#pragma once

#include "mbed.h"

namespace Particula
{
    enum ErrorCodes {
        NO_HEADER,
        DEVICE_NOT_READABLE,
        READ_SUCCESSFULL,
        READ_NOT_SUCCESSFULL,
        SLEEP_SUCCESSFULL,
        SLEEP_NOT_SUCCESSFULL,
        WAKEUP_SUCCESSFULL,
        WAKEUP_NOT_SUCCESSFULL,
        WORKING_PERIODE_SET,
        WORKING_PERIODE_NOT_SET,
        
    };

    class SDS011
    {    

        public:
            SDS011(PinName pinTXDevice, PinName pinRXDevice);
            
        public:    
            double getPM25Value();
            double getPM10Value();
            int getIdByte();        
            void sendDataToPc();           
            ErrorCodes sleep();
            ErrorCodes wakeUp();
            ErrorCodes read();
            ErrorCodes setWorkingPeriode(uint8_t);
            void printfbuffer(); 
            
                
        private:
            int calculateChecksum(int,int,uint8_t[]);
            bool correctChecksum();
        private:
            static const unsigned int PACKET_SIZE = 10;
            
            const int MAX_TRIES = 20;
            const double MAX_TIMOUT = 2.0;    
            

            uint8_t buffer[PACKET_SIZE];
            const uint8_t sleep_command[20] = {0xAA, 0xB4, 0x06, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x05, 0xAB};
            const uint8_t wakeup_command[20] = {0xAA, 0xB4, 0x06, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x06, 0xAB};
            uint8_t periode_command[20] = {0xAA, 0xB4, 0x08, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0xAB};
     
            double PM25Value;
            double PM10Value;
            int idByte;
            Serial device;
    };
};