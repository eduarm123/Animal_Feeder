#ifndef __NVM_DRIVERS_H__
#define __NVM_DRIVERS_H__

#ifdef	__cplusplus
extern "C" {
#endif



/**********************************INCLUDES ******************************************************/

#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"

/********************************* (1) PUBLIC METHODS ********************************************/


/*********************************** (2) PUBLIC VARS *********************************************/



/******************************** (3) DEFINES & MACROS *******************************************/


/*********************************** (4) PRIVATE VARS ********************************************/


/**************************** (5) PRIVATE METHODS DEFINITION *************************************/

/************************* (6)  STATIC METHODS IMPLEMENTATION ************************************/

/***************************** (7) PUBLIC METHODS IMPLEMENTATION *********************************/

void NNVM_write_memory_u16(const char * _keynamewrite,uint16_t _writeStorageu16);

void NNVM_read_memory_u16(const char * _keyname, uint16_t *_storageu16);




#ifdef	__cplusplus
}
#endif

#endif  /* __NVM_DRIVERS_H__ */