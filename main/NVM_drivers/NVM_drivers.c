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


void NNVM_write_memory_u16(const char * _keynamewrite,uint16_t _writeStorageu16)
{
        esp_err_t err = nvs_flash_init();
        if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
            // NVS partition was truncated and needs to be erased
            // Retry nvs_flash_init
            ESP_ERROR_CHECK(nvs_flash_erase());
            err = nvs_flash_init();
        }
        ESP_ERROR_CHECK( err );

        // Open   
        printf("Opening Non-Volatile Storage (NVS) handle... ");
        nvs_handle_t my_handle;

        err = nvs_open(_keynamewrite, NVS_READWRITE, &my_handle);
        if (err != ESP_OK) {
            printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        } else {

            printf("Done\n");
            err = nvs_set_u16(my_handle, "restart_counter", _writeStorageu16);
            printf((err != ESP_OK) ? "Failed!\n" : "Done\n");

            printf("Committing updates in NVS ... ");
            err = nvs_commit(my_handle);
            printf((err != ESP_OK) ? "Failed!\n" : "Done\n");
            // Close
            nvs_close(my_handle);
        }

}

void NNVM_read_memory_u16(const char * _keyname,uint16_t *_storageu16)
{
        esp_err_t err = nvs_flash_init();
        if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
            // NVS partition was truncated and needs to be erased
            // Retry nvs_flash_init
            ESP_ERROR_CHECK(nvs_flash_erase());
            err = nvs_flash_init();
        }
        ESP_ERROR_CHECK( err );
        // Open   
        printf("Opening Non-Volatile Storage (NVS) handle... ");
        nvs_handle_t my_handle;

            err = nvs_open(_keyname, NVS_READONLY, &my_handle);
        if (err != ESP_OK) {
            printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        } else {

            // Read
            printf("Reading restart counter from NVS ... ");
            err = nvs_get_u16(my_handle, _keyname, _storageu16);
            switch (err) {
                case ESP_OK:
                    printf("Done\n");
                    printf("Restart counter = %d\n", (int)*_storageu16);
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
