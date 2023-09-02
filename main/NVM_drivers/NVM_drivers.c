/*************************************************************************************************/
/*! @file	NVM_drivers.c
 *	@brief	Se utiliza los drivers de ESP32 para grabar y escribir en la non volatil memory
 *
 *	\b Descripcion: Introducir aquï¿½ descripciï¿½n de las funcionalidades del fichero \n
 *
 *
 *		Compiler  :  \n
 *		Copyright :  \n
 *		Target    :  \n
 *
 *	@author		
 *	@date		25 ago. 2023
 *	@version	1.0
 *
 ****************************************************************************
 *
 *	Log: main.c
 * Revision [Date // Author]:  Modifications \n
 * -----------------------------------------------------------\n
 * V1.0 [ 25 oct. ago // earmijos ]: Original \n
 *
 */
/*************************************************************************************************/


/**********************************INCLUDES ******************************************************/

#include "NVM_drivers.h"

/********************************* (1) PUBLIC METHODS ********************************************/


/*********************************** (2) PUBLIC VARS *********************************************/


/******************************** (3) DEFINES & MACROS *******************************************/

/*********************************** (4) PRIVATE VARS ********************************************/


/**************************** (5) PRIVATE METHODS DEFINITION *************************************/

/************************* (6)  STATIC METHODS IMPLEMENTATION ************************************/

/***************************** (7) PUBLIC METHODS IMPLEMENTATION *********************************/


void NNVM_write_memory_u32(const char * _keynamewrite,uint32_t _writeStorageu32)
{
        esp_err_t err = ESP_OK;
        // Open   
        printf("Opening Non-Volatile Storage (NVS) handle... ");
        nvs_handle_t my_handle;

        err = nvs_open(_keynamewrite, NVS_READWRITE, &my_handle);
        if (err != ESP_OK) {
            printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        } else {

            printf("Done\n");
            err = nvs_set_u32(my_handle, _keynamewrite, _writeStorageu32);
            printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

            printf("Committing updates in NVS ... ");
            err = nvs_commit(my_handle);
            printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
            // Close
            nvs_close(my_handle);
        }

}

void NNVM_read_memory_u32(const char * _keyname,uint32_t *_storageu32)
{
        esp_err_t err = ESP_OK;
        // Open   
        printf("Opening Non-Volatile Storage (NVS) handle... ");
        nvs_handle_t my_handle;

        err = nvs_open(_keyname, NVS_READONLY, &my_handle);
        if (err != ESP_OK) {
            printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        } else {

            // Read
            printf("Reading value from NVS ... ");
            err = nvs_get_u32(my_handle, _keyname, _storageu32);
            switch (err) {
                case ESP_OK:
                    printf("Done\n");
                    printf("Value = %d\n", (int)*_storageu32);
                    break;
                case ESP_ERR_NVS_NOT_FOUND:
                    printf("The value is not initialized yet!\n");
                    break;
                default :
                    printf("Error (%s) reading!\n", esp_err_to_name(err));
            }
            // Close
            nvs_close(my_handle);
        }

}
