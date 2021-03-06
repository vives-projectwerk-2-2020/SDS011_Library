#include "SDS011.h"
#include <time.h>

/* Functions */
namespace Particula {

    SDS011::SDS011(PinName pinTXdevice, PinName pinRXdevice)
        :device(pinTXdevice,pinRXdevice,9600){
        device.set_blocking(false);
    }

    ErrorCodes SDS011::read(void){
        bool succesfulRead = false;
        int headData;
        int counter = 0;      
        
        while(!succesfulRead && (counter < MAX_TRIES)){
            time_t beginLoop,endloop;
            time(&beginLoop);
            while (!device.readable()){
                time(&endloop);
                if (difftime(endloop,beginLoop)>MAX_TIMOUT){
                    return DEVICE_NOT_READABLE;
                }    
            }
            headData = device.getc();
            if(headData == 0xAA){            
                buffer[0] = headData;          
                for(uint8_t t = 1; t<PACKET_SIZE; t++){
                    buffer[t] = device.getc();
                }   
                if(buffer[9] == 0xAB){
                    succesfulRead = true;
                    PM25Value = ((buffer[3] * 256) + buffer[2])/10.0;
                    PM10Value = ((buffer[5] *256) + buffer[4])/10.0;
                    idByte = buffer[6] + buffer[7]*256;
                }             
            } 
            
            counter++;
            if(counter == (MAX_TRIES-1)){
                return NO_HEADER;
            }
        }        
        if(correctChecksum()){
            return READ_SUCCESSFULL;
        } else {
            return READ_NOT_SUCCESSFULL;
        }  

        return READ_NOT_SUCCESSFULL;    
    }
       
    ErrorCodes SDS011::sleep(void){
        for(uint8_t i =0; i<19; i++){
            device.putc(sleep_command[i]);
        }  
        if (read() == READ_SUCCESSFULL){
            if(buffer[4] == 0x00){
                return SLEEP_SUCCESSFULL;
            }
            return SLEEP_NOT_SUCCESSFULL;
        } 
        return SLEEP_NOT_SUCCESSFULL;
    }

    ErrorCodes SDS011::wakeUp(void){              /* !! DO NOT TOUCH !! */
        for(int t=0; t<2 ;t++){             // is necessary to keep this onholy peace of code working 
            ThisThread::sleep_for(500);     
            for(uint8_t i =0; i<19; i++){
                device.putc(wakeup_command[i]);
            } 
        }
        if(read() == READ_SUCCESSFULL){
            if(buffer[4] == 0x01){
                return WAKEUP_SUCCESSFULL;
            }
            return WAKEUP_NOT_SUCCESSFULL;
        }
        return WAKEUP_NOT_SUCCESSFULL;
    }

    ErrorCodes SDS011::setWorkingPeriode(uint8_t periode){
        /* if periode=0, work continuously */
        /* else work 30sec and sleep for periode*60 -30sec */
        periode_command[4] = periode;
        periode_command[17] = calculateChecksum(2,17,periode_command);

        for(uint8_t i =0; i<19; i++){
            device.putc(periode_command[i]);
            printf("\r\n %X \r\n",periode_command[i]);
        }  
        if( read() == READ_SUCCESSFULL){
            if(buffer[4] == periode){
                return WORKING_PERIODE_SET;
            }
            return WORKING_PERIODE_NOT_SET;
        }
       
    }

    int SDS011::calculateChecksum(int beginData, int endData,uint8_t Package[]){
        int checksum=0;
        for(int t=beginData; t<=endData; t++ ){
            checksum += Package[t];
        }
        return checksum%256;  
    }

    double SDS011::getPM25Value(){
        return PM25Value;
    }

    double SDS011::getPM10Value(){
        return PM10Value;
    }

    int SDS011::getIdByte(){
        return idByte;
    }

    bool SDS011::correctChecksum(){       
        if(calculateChecksum(2,7,buffer) == buffer[8]){
            return true;
        }
        return false;
    }

    void SDS011::sendDataToPc(){
        
        printf("\r\nThe sensorID is %X \r\n", idByte);
        printf("The air contains %.1lf µg/m³ of PM2.5 \r\n", PM25Value);
        printf("The air contains %.1lf µg/m³ of PM10 \r\n", PM10Value);
        if(correctChecksum() == true){
            printf("The checksum is true, data is correct \r\n");
        } else {
            printf("The checksum is false, data is corrupt \r\n");
        }
    } 

    void SDS011::printfbuffer(){
        printf("\r\nThis is the buffer:");
        for(uint8_t t=0;t<PACKET_SIZE;t++){
            printf(" %x", buffer[t]);
        }
        printf("\r\n");
    }   


}