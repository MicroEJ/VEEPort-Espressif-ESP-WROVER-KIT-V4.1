
/*
 * C
 * Copyright 2014-2020 MicroEJ Corp. All rights reserved.
 * This library is provided in source code for use, modification and test, subject to license terms.
 * Any modification of the source code will break MicroEJ Corp. warranties on the whole library.
 */

#define FS_ADAPT_PRINT_ERROR
//#define FS_ADAPT_DEBUG

#include <string.h>
#include <sni.h>
#include <stdint.h>
#include "diskio.h"
#include "pool.h"
#include "fs_helper_os.h"
#include "fs_helper.h"
#include "LLFS_impl.h"
#include "sdmmc_cmd.h"
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"

#ifdef __cplusplus
	extern "C" {
#endif

#define FS_DELEGATE_STACK_SIZE         (7 * 1024)
#define FS_DELEGATE_TASK_PRIORITY      ( 12 ) /** Should be > tskIDLE_PRIORITY & < configTIMER_TASK_PRIORITY */
#define FS_DELEGATE_TASK_STACK_SIZE     FS_DELEGATE_STACK_SIZE/4

/** @brief add SD card GPIO configuration */
#define SD_CLK_GPIO GPIO_NUM_14 // Shared with JTAG
#define SD_CMD_GPIO GPIO_NUM_15 // Shared with JTAG
#define SD_D0_GPIO GPIO_NUM_2 // Shared with LED/Camera
#define SD_D3_GPIO GPIO_NUM_13 // Shared with JTAG/ECOM-COMM (SPI)

/** @ brief private pool file */
static FIL gpst_pool_file[FS_MAX_NUMBER_OF_FILE_IN_POOL];
static POOL_item_status_t gpst_pool_file_item_status[FS_MAX_NUMBER_OF_FILE_IN_POOL];
static POOL_ctx_t gst_pool_file_ctx =
{
	gpst_pool_file,
	gpst_pool_file_item_status,
	sizeof(FIL),
	sizeof(gpst_pool_file)/sizeof(FIL)
};

/** @brief private pool directory */
static FF_DIR gpst_pool_dir[FS_MAX_NUMBER_OF_DIR_IN_POOL];
static POOL_item_status_t gpst_pool_dir_item_status[FS_MAX_NUMBER_OF_DIR_IN_POOL];
static POOL_ctx_t gst_pool_dir_ctx =
{
	gpst_pool_dir,
	gpst_pool_dir_item_status,
	sizeof(FF_DIR),
	sizeof(gpst_pool_dir)/sizeof(FF_DIR)
};

/** @brief SD card logical drive path */
FATFS SDFatFs;  /* File system object for SD card logical drive */
sdmmc_card_t s_card;
char SDPath[4];

WCHAR ff_convert(WCHAR wch, UINT dir)
{
	WCHAR c_return;
	
	/* test if wch is in simple ascii table */
	if (wch < 0x80)
	{	
		c_return = wch;
	}
	else
	{
		c_return = 0;
	}
	
	return(c_return);
}

/**
 * @brief private function to set or clear readOnly attribute of a file.
 *
 * @param[in] _pc_path_in pointer on path file. This method assumes this pointer is not null.
 * @param[in] _uc_enable enable read only or not
 * @param[out] _puc_bool_out pointer on boolean value (1 error / 0 ok)
 */
void ff_set_read_only(char* _pc_path_in, unsigned char _uc_enable, unsigned char * _puc_bool_out)
{
	FRESULT fs_err;
	FILINFO fno;
	BYTE attr;

	/* FatFs does not manage correctly readOnly attribute for directories. */
	if(_uc_enable)
	{
		/* Test if the file is a directory */
		fs_err = f_stat(_pc_path_in, &fno);
		/* If an error occurs or the file is directory returns error, otherwise continue */
		if (fs_err != FR_OK || (fno.fattrib & AM_DIR))
		{
			* _puc_bool_out = 1; // Error
			return;
		}
	}

	attr = _uc_enable ? AM_RDO : 0;
	fs_err = f_chmod((TCHAR*) _pc_path_in, attr, AM_RDO);

	if (fs_err == FR_OK)
	{
		* _puc_bool_out = 0; // OK
	}
	else
	{
		* _puc_bool_out = 1; // Error
	}
}

FS_ADAPT_status_t FS_ADAPT_init_f(void)
{
	FS_ADAPT_status_t e_return = FS_ADAPT_ERROR;

	/* Connect SDMMC driver to FATFS */
	BYTE pdrv = 0xFF;
	if (ff_diskio_get_drive(&pdrv) == ESP_OK && pdrv != 0xFF)
	{
		/* Link SDMMC driver */
		ff_diskio_register_sdmmc(pdrv, &s_card);

		/* Set SD path */
		SDPath[0] = (char)('0' + pdrv);
		SDPath[1] = ':';
		SDPath[2] = 0;

		/* Try to mount partition */
		FRESULT res = f_mount(&SDFatFs, (TCHAR const *)SDPath, 0);
		if (res == FR_OK)
		{
			e_return = FS_ADAPT_NO_ERROR;
		}
		else
		{
#ifdef FS_ADAPT_PRINT_ERROR
			printf("FS_ADAPT_ERROR [%s:%u] : failed to mount card (error %d)\n", __MODULE__, __LINE__, res);
#endif
		}
	}
	else
	{
#ifdef FS_ADAPT_PRINT_ERROR
		printf("FS_ADAPT_ERROR [%s:%u] : the maximum count of volumes is already mounted\n", __MODULE__, __LINE__);
#endif
	}

	/* init private pool */
	
	memset(gpst_pool_file, 0, sizeof(gpst_pool_file));
	memset(gpst_pool_file_item_status, POOL_FREE, sizeof(gpst_pool_file_item_status));
	
	memset(gpst_pool_dir, 0, sizeof(gpst_pool_dir));
	memset(gpst_pool_dir_item_status, POOL_FREE, sizeof(gpst_pool_dir_item_status));
	
	return(e_return);
}

FS_ADAPT_status_t FS_ADAPT_init_media(BYTE pdrv)
{
	esp_err_t err = ESP_OK;

    /*
     * Use 1-line mode, so ecom-comm (SPI) can be used at the same time, as the 4-line mode uses GPIO12 and GPIO13 which are
     * also used by the ecom-comm (SPI)
     */
    sdmmc_host_t host_config = SDMMC_HOST_DEFAULT();
    host_config.flags = SDMMC_HOST_FLAG_1BIT;
    host_config.max_freq_khz = SDMMC_FREQ_HIGHSPEED;

    /* This initializes the slot without card detect (CD) and write protect (WP) signals and in 1-line mode. */
    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    slot_config.width = 1;

    /*
     * The below GPIOs should have external 10k pull-ups.
     * Internal pull-ups are not sufficient, but enabling them makes a difference on some boards
     */
    gpio_set_pull_mode(SD_CMD_GPIO, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(SD_D0_GPIO , GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(SD_D3_GPIO , GPIO_PULLUP_ONLY); // Not used in 1-line SD mode, but we need to enable the internal pull-up

    /* Init the SD host */
    err = host_config.init();
	#ifdef FS_ADAPT_PRINT_ERROR
		if (err != ESP_OK) {
			printf("FS_ADAPT_PRINT_ERROR [%s:%u] : host init returned rc=0x%x\n", __MODULE__, __LINE__, err);
			goto fail;
		}
	#endif

    /* Configure SD slot */
    if (host_config.flags == SDMMC_HOST_FLAG_SPI) {
        err = sdspi_host_init_slot(host_config.slot,
                (const sdspi_slot_config_t*) &slot_config);
    } else {
        err = sdmmc_host_init_slot(host_config.slot,
                (const sdmmc_slot_config_t*) &slot_config);
    }
	#ifdef FS_ADAPT_PRINT_ERROR
		if (err != ESP_OK) {
			printf("FS_ADAPT_PRINT_ERROR [%s:%u] : slot_config returned rc=0x%x\n", __MODULE__, __LINE__, err);
			goto fail;
		}
	#endif

    /* Probe and initialize card */
    err = sdmmc_card_init(&host_config, &s_card);
	#ifdef FS_ADAPT_PRINT_ERROR
		if (err != ESP_OK) {
			printf("FS_ADAPT_PRINT_ERROR [%s:%u] : sdmmc_card_init failed 0x(%x)\n", __MODULE__, __LINE__, err);
			goto fail;
		}
	#endif

    return FS_ADAPT_NO_ERROR;

fail:
    host_config.deinit();
    return FS_ADAPT_ERROR;
}

static void xFsDelegateTaskFunction(void * pvParameters)
{
	// Caunch event consumer
	FS_run_f();
}

FS_ADAPT_status_t FS_ADAPT_start_f(void)
{
	BaseType_t created = xTaskCreate( xFsDelegateTaskFunction, "FS Delegate", FS_DELEGATE_STACK_SIZE, NULL, FS_DELEGATE_TASK_PRIORITY, NULL );
	return created == pdPASS ? FS_ADAPT_NO_ERROR : FS_ADAPT_ERROR;
}

FS_ADAPT_status_t FS_ADAPT_get_java_thread(unsigned long * _pul_java_thread)
{
	FS_ADAPT_status_t e_return;
	int32_t i32_java_thread;
	i32_java_thread = SNI_getCurrentJavaThreadID();

	if ((_pul_java_thread != NULL) && (i32_java_thread != SNI_ERROR))
	{
		* _pul_java_thread = (unsigned long)i32_java_thread;
		e_return = FS_ADAPT_NO_ERROR;
	}
	else
	{
		#ifdef FS_ADAPT_PRINT_ERROR
			printf("FS_ADAPT_ERROR [%s:%u] : cannot get java thread (error %d)\n", __MODULE__, __LINE__, i32_java_thread);
		#endif
		e_return = FS_ADAPT_ERROR;
	}
	
	return (e_return);
}

FS_ADAPT_status_t FS_ADAPT_suspend_java_thread (void)
{
	FS_ADAPT_status_t e_return;
	int32_t i32_result;
	
	i32_result = SNI_suspendCurrentJavaThread(0);
	
	if (i32_result != SNI_OK)
{
		e_return = FS_ADAPT_ERROR;
		#ifdef FS_ADAPT_PRINT_ERROR
			printf("FS_ADAPT_ERROR [%s:%u] : cannot suspend java thread (error %d)\n", __MODULE__, __LINE__, i32_result);
		#endif
	}
	else
	{
		e_return = FS_ADAPT_NO_ERROR;
	}
	
	return (e_return);
}

FS_ADAPT_status_t FS_ADAPT_resume_java_thread(unsigned long const _ul_java_thread)
{
	FS_ADAPT_status_t e_return;
	int32_t i32_result;

	i32_result = SNI_resumeJavaThread(_ul_java_thread);
	
	if (i32_result != SNI_OK)
	{
		#ifdef FS_ADAPT_PRINT_ERROR
			printf("FS_ADAPT_ERROR [%s:%u] : cannot resume java thread (error %d, thread %lu)\n", __MODULE__, __LINE__, i32_result, _ul_java_thread);
		#endif
		e_return = FS_ADAPT_ERROR;
	}
	else
	{
		e_return = FS_ADAPT_NO_ERROR;
	}

	return (e_return);
}

FS_ADAPT_status_t FS_ADAPT_post_event_action_f(void * _pv_mailbox, void* _pv_event, unsigned long _ul_timeout)
{
	FS_ADAPT_status_t e_return;
	BaseType_t e_result;

	/* test if mailbox is full */
	if (uxQueueSpacesAvailable(*(QueueHandle_t*)(_pv_mailbox)) != 0)
	{
		/* send event without timeout */
		e_result = xQueueSend(*(QueueHandle_t*)(_pv_mailbox), &_pv_event, 0);

		/* test if send is ok */
		if (e_result == pdPASS)
		{
			e_return = FS_ADAPT_NO_ERROR;
		}
		else
		{
			e_return = FS_ADAPT_ERROR;
			#ifdef FS_ADAPT_PRINT_ERROR
				printf("FS_ADAPT_ERROR [%s:%u] : sending error (err %u)\n", __MODULE__, __LINE__, e_result);
			#endif
		}
	}
	else
	{
		#ifdef FS_ADAPT_PRINT_ERROR
			printf("FS_ADAPT_ERROR [%s:%u] : mail box appear full\n", __MODULE__, __LINE__);
		#endif
		e_return = FS_ADAPT_ERROR;
	}
	
	return (e_return);
}


FS_ADAPT_status_t FS_ADAPT_wait_event(void * _pv_mailbox, void** _ppv_event, unsigned long _ul_timeout)
{
	FS_ADAPT_status_t e_return;
	BaseType_t e_result;
	
	e_result = xQueueReceive(*(QueueHandle_t*)(_pv_mailbox), _ppv_event, _ul_timeout);

	/* test received function */
	if (e_result != pdTRUE)
	{
		e_return = FS_ADAPT_ERROR;
		#ifdef FS_ADAPT_PRINT_ERROR
			printf("FS_ADAPT_ERROR [%s:%u] : task was wake up not properly\n", __MODULE__, __LINE__);
		#endif
	}
	else
	{
		e_return = FS_ADAPT_NO_ERROR;
	}
	
	return (e_return);
}

/***** All private function FS_specific *******/

void FS_ADAPT_close_file_f(unsigned int ui_fd_in,
		                 unsigned char * puc_bool_out)
{
	FRESULT fs_err;
	
	fs_err = f_close((FIL*) ui_fd_in);

	if (fs_err != FR_OK)
	{
		*puc_bool_out = 1;
	}
	else
	{
		*puc_bool_out = 0;
	}
	
	/* free file pool */
	POOL_free_f(&gst_pool_file_ctx, (void*) ui_fd_in);

#ifdef FS_ADAPT_DEBUG
	printf("FS_ADAPT_DEBUG [%s:%u] : close file %d (status %d, err %d)\n", __MODULE__, __LINE__, ui_fd_in, *puc_bool_out, fs_err);
#endif
}

void FS_ADAPT_open_file_f(char* _pc_path_in,
		                      unsigned char _uc_mode_in,
		                      int * _pi_fd_out)
{
	FRESULT fs_err;
	BYTE b_internal_mode;
	FIL * m_file;
	POOL_status_t e_result;

	#ifdef FS_ADAPT_DEBUG
		printf("FS_ADAPT_DEBUG [%s:%u] : open file %s in %c mode\n", __MODULE__, __LINE__, _pc_path_in, _uc_mode_in);
	#endif
	/* test entry function */
	if (_pc_path_in != NULL)
	{
		/* test read or write mode */
		if (_uc_mode_in == 'A')
		{
			/* append so write existing file or create file if needed */
			b_internal_mode = FA_WRITE | FA_OPEN_ALWAYS;
		}
		else if (_uc_mode_in == 'R')
		{
			/* read so read on existing file */
			b_internal_mode = FA_READ | FA_OPEN_EXISTING;
		}
		else if (_uc_mode_in == 'W')
		{
			/* write create file if needed or overwrite existing */
			b_internal_mode = FA_WRITE | FA_CREATE_ALWAYS;
		}
		else if (_uc_mode_in == ('R' | 'W'))
		{
			/* write create file if needed or overwrite existing plus read permission */
			b_internal_mode = FA_READ | FA_WRITE | FA_CREATE_ALWAYS;
		}
		else if (_uc_mode_in == ('R' | 'A'))
		{
			/* append so write existing file or create file if needed plus read permission */
			b_internal_mode = FA_READ | FA_WRITE | FA_OPEN_ALWAYS;
		}
		else
		{
			/* nothing to do */
			b_internal_mode = 0xFF;
		}

		/* allocate in pool */
		e_result = POOL_reserve_f(&gst_pool_file_ctx, (void**)&m_file);

		/* test if alloc is ok */
		if (e_result == POOL_NO_ERROR)
		{

			/* open file according to internal mode */
			fs_err = f_open(m_file, (TCHAR*)_pc_path_in, b_internal_mode);

			/* test return function */
			if (fs_err != FR_OK)
			{
				POOL_free_f(&gst_pool_file_ctx, (void*) m_file);
				*_pi_fd_out = LLFS_NOK;  // error
			}
			else
			{
				/* test if append mode */
				if ((_uc_mode_in == 'A') || (_uc_mode_in == ('R' | 'A')))
				{
					/* seek at the end of file */
					fs_err = f_lseek(m_file, f_size(m_file));

					/* test return function */
					if (fs_err != FR_OK)
					{
						POOL_free_f(&gst_pool_file_ctx, (void*) m_file);
						*_pi_fd_out = LLFS_NOK;  // error
					}
					else
					{
						*_pi_fd_out = (int)m_file;  // no error
					}
				}
				else
				{
					*_pi_fd_out = (int)m_file;  // no error
				}
			}
		}
		else
		{
			*_pi_fd_out = LLFS_NOK;  // error
		}
	}
	else
	{
		*_pi_fd_out = LLFS_NOK;  // error
	}
	
}

void FS_ADAPT_read_file_f(unsigned int _ui_fd_in,
						              char* _pc_output_buffer_in,
						              unsigned int _ui_data_length_in,
						              int * _pi_error_out)
{
	unsigned int bytesread;
	FRESULT fs_err = FR_INT_ERR;

	/* Test entry function */
	if (_pc_output_buffer_in != NULL)
	{
		/* read */
		fs_err = f_read((FIL*)_ui_fd_in,
						_pc_output_buffer_in ,
						_ui_data_length_in,
						&bytesread);

		/* test return function */
		if (fs_err != FR_OK)
		{
			*_pi_error_out = LLFS_NOK;  // error
		}
		else
		{
			/* if read byte == 0 return EOF (see sun implementation) */
			*_pi_error_out = (bytesread == 0) ? LLFS_EOF : (int) bytesread;
		}
	}
	else
	{
		*_pi_error_out = LLFS_NOK;  // error
	}
	
		#ifdef FS_ADAPT_DEBUG
				printf("FS_ADAPT_DEBUG [%s:%u] read file content %s - read %d (err %d)\n",  __MODULE__, __LINE__, _pc_output_buffer_in, * _pi_error_out, fs_err);
		#endif
}


void FS_ADAPT_write_file_f(unsigned int _ui_fd_in,
						               char* _pc_intput_buffer_in,
						               unsigned int _ui_data_length_in,
						               int * _pi_error_out)
{
	unsigned int byteswritten;
	FRESULT fs_err = FR_INT_ERR;

	/* Test entry function */
	if (_pc_intput_buffer_in != NULL)
	{
		/* read */
		fs_err = f_write((FIL*) _ui_fd_in, _pc_intput_buffer_in , _ui_data_length_in, &byteswritten);

		/* test return function */
		if (fs_err != FR_OK || byteswritten != _ui_data_length_in)
		{
			* _pi_error_out = LLFS_NOK;  // error
		}
		else
		{
			* _pi_error_out = (int) byteswritten;
		}
	}
	else
	{
		* _pi_error_out = LLFS_NOK;  // error
	}
	
	#ifdef FS_ADAPT_DEBUG
				printf("FS_ADAPT_DEBUG [%s:%u] write file content %s - written %d (err %d)\n",  __MODULE__, __LINE__, _pc_intput_buffer_in, * _pi_error_out, fs_err);
	#endif
}

void FS_ADAPT_write_byte_file_f(unsigned int _ui_fd_in,
										int _i_data_in,
										int * _pi_error_out)
{
	unsigned int byteswritten;
	FRESULT fs_err;

	/* read */
	fs_err = f_write((FIL*) _ui_fd_in, &_i_data_in , 1, &byteswritten);

	/* test return function */
	if (fs_err != FR_OK || byteswritten != 1)
	{
		* _pi_error_out = LLFS_NOK; // error
	}
	else
	{
		* _pi_error_out = LLFS_OK; // OK
	}


	#ifdef FS_ADAPT_DEBUG
				printf("FS_ADAPT_DEBUG [%s:%u] write byte file content %d - written %d (err %d)\n",  __MODULE__, __LINE__, _i_data_in, * _pi_error_out, fs_err);
	#endif
}


void FS_ADAPT_read_byte_file_f(unsigned int _ui_fd_in,
										int * _pi_error_out)
{
	unsigned int bytesread;
	unsigned char data;
	FRESULT fs_err;

	/* read */
	fs_err = f_read((FIL*)_ui_fd_in,
					&data ,
					1,
					&bytesread);

	/* test return function */
	if (fs_err != FR_OK)
	{
		*_pi_error_out = LLFS_NOK;  // error
	}
	else if (bytesread == 0){
		*_pi_error_out = LLFS_EOF;  // EOF
	}
	else
	{
		*_pi_error_out = data;  // OK
	}

	#ifdef FS_ADAPT_DEBUG
			printf("FS_ADAPT_DEBUG [%s:%u] read byte file content %d - result %d (err %d)\n",  __MODULE__, __LINE__, data, * _pi_error_out, fs_err);
	#endif
}


void FS_ADAPT_skip_file_f(unsigned int _ui_fd_in,
								   long long int _l_n_in,
								   long long int * _pl_error_out)
{
	FRESULT fs_err = FR_INT_ERR;
	FIL* fd = (FIL*)_ui_fd_in;

	DWORD currentPtr = f_tell(fd);
	DWORD size = f_size(fd);
	long long int newPtr = currentPtr + _l_n_in;

	// Check for overflow when computing newPtr
	if(_l_n_in > 0)
	{
		if(newPtr < currentPtr)
		{
			// an overflow occurs: saturate the newPtr value
			newPtr = INT64_MAX;
		}
	}
	else
	{
		if(newPtr > currentPtr)
		{
			// an overflow occurs: saturate the newPtr value
			newPtr = INT64_MIN;
		}
	}

	if(newPtr < 0)
	{
		*_pl_error_out = LLFS_NOK; // error
	}
	else
	{
		// limit newPtr value to the size of the file
		if(newPtr > size)
		{
			newPtr = size;
		}

		// Info: cast of newPtr is safe here because 0 <= newPtr <= size
		fs_err = f_lseek(fd, (DWORD)newPtr);
		if(fs_err != FR_OK)
		{
			*_pl_error_out = LLFS_NOK; // error
		}
		else
		{
			*_pl_error_out = newPtr - currentPtr;
		}
	}

	#ifdef FS_ADAPT_DEBUG
		printf("FS_ADAPT_DEBUG [%s:%u] skip file : n=%lld skipped=%lld (err %d)\n",  __MODULE__, __LINE__, _l_n_in, * _pl_error_out, fs_err);
	#endif
}


void FS_ADAPT_available_file_f(unsigned int _ui_fd_in,
										int * _pi_error_out)
{
	FIL* fd = (FIL*)_ui_fd_in;

	DWORD currentPtr = f_tell(fd);
	DWORD size = f_size(fd);

	*_pi_error_out = size - currentPtr;

	#ifdef FS_ADAPT_DEBUG
		printf("FS_ADAPT_DEBUG [%s:%u] available file : %d\n",  __MODULE__, __LINE__, * _pi_error_out);
	#endif
}


void FS_ADAPT_exist_file_f(char* _pc_path_in, unsigned char * _puc_bool_out)
{
	FRESULT fs_err = FR_INT_ERR;

	/* test entry function */
	if (_pc_path_in != NULL)
	{
		fs_err = f_stat(_pc_path_in, NULL);

		/* test return function */
		if (fs_err != FR_OK)
		{
			* _puc_bool_out = 1;
		}
		else
		{
			* _puc_bool_out = 0;
		}
	}
	else
	{
		* _puc_bool_out = 1;
	}
	#ifdef FS_ADAPT_DEBUG
				printf("FS_ADAPT_DEBUG [%s:%u] exist file : %u (err %d)\n",  __MODULE__, __LINE__, * _puc_bool_out, fs_err);
	#endif
}


void FS_ADAPT_get_free_space_f(char* _pc_path_in, unsigned long * _pul_size_out)
{
	FATFS *fs;
	DWORD dw_free_cluster;
	FRESULT fs_err = FR_INT_ERR;

	/* test entry function */
	if (_pc_path_in != NULL)
	{
		fs_err = f_getfree((TCHAR*)_pc_path_in, &dw_free_cluster, &fs);

		/* test return function */
		if (fs_err != FR_OK)
		{
			* _pul_size_out = 0; //error
		}
		else
		{
#if FF_MAX_SS != FF_MIN_SS
			WORD ss;
			if (disk_ioctl(fs->pdrv, GET_SECTOR_SIZE, &ss) != RES_OK)
			{
				* _pul_size_out = 0; //error;
			}
			else
			{
				* _pul_size_out = (dw_free_cluster * fs->csize) * ss;
			}
#else
			* _pul_size_out = (dw_free_cluster * fs->csize) * FF_MAX_SS;
#endif
		}
	}
	else
	{
		* _pul_size_out = 0; //error
	}
	
	#ifdef FS_ADAPT_DEBUG
		printf("FS_ADAPT_DEBUG [%s:%u] : get free size on %s : %lu (err %d)\n", __MODULE__, __LINE__,_pc_path_in, *_pul_size_out,fs_err);
	#endif
}


void FS_ADAPT_get_total_space_f(char* _pc_path_in, unsigned long * _pul_size_out)
{
	FATFS *fs;
	DWORD dw_free_cluster;
	FRESULT fs_err = FR_INT_ERR;

	/* test entry function */
	if (_pc_path_in != NULL)
	{
		fs_err = f_getfree((TCHAR*)_pc_path_in, &dw_free_cluster, &fs);

		/* test return function */
		if (fs_err != FR_OK)
		{
			* _pul_size_out = 0; //error
		}
		else
		{
#if FF_MAX_SS != FF_MIN_SS
			WORD ss;
			if (disk_ioctl(fs->pdrv, GET_SECTOR_SIZE, &ss) != RES_OK)
			{
				* _pul_size_out = 0; //error;
			}
			else
			{
				/* minus 2 -> one for boot and one reserved */
				* _pul_size_out = ((fs->n_fatent - 2) * fs->csize) * ss;
			}
#else
			/* minus 2 -> one for boot and one reserved */
			* _pul_size_out = ((fs->n_fatent - 2) * fs->csize) * FF_MAX_SS;
#endif
		}
	}
	else
	{
		* _pul_size_out = 0 ; //error
	}
	
	#ifdef FS_ADAPT_DEBUG
		printf("FS_ADAPT_DEBUG [%s:%u] : get total size on %s : %lu (err %d)\n", __MODULE__, __LINE__,_pc_path_in, *_pul_size_out,fs_err);
	#endif 
}

void FS_ADAPT_get_usable_space_f(char* _pc_path_in, unsigned long * _pul_size_out)
{
	FS_ADAPT_get_free_space_f(_pc_path_in, _pul_size_out);
}

void FS_ADAPT_is_directory_f(char* _pc_path_in, unsigned char * _puc_bool_out)
{
	FILINFO fno;
	FRESULT fs_err;

	/* test entry function */
	if (_pc_path_in != NULL)
	{
		fs_err = f_stat(_pc_path_in, &fno);
		/* test return function */
		if (fs_err != FR_OK)
		{
			* _puc_bool_out = 1;
		}
		else
		{
			* _puc_bool_out = (fno.fattrib & AM_DIR) ? 0 : 1;
		}
	}
	else
	{
		* _puc_bool_out = 1;
	}
}

void FS_ADAPT_is_file_f(char* _pc_path_in, unsigned char * _puc_bool_out)
{
	FILINFO fno;
	FRESULT fs_err;

	/* test entry function */
	if (_pc_path_in != NULL)
	{
		fs_err = f_stat(_pc_path_in, &fno);

		/* test return function */
		if (fs_err != FR_OK)
		{
			* _puc_bool_out = 1;
		}
		else
		{
			* _puc_bool_out = (fno.fattrib & AM_DIR) ? 1 : 0;
		}
	}
	else
	{
		* _puc_bool_out = 1;
	}
}

void FS_ADAPT_is_hidden_f(char* _pc_path_in, unsigned char * _puc_bool_out)
{
	FILINFO fno;
	FRESULT fs_err;

	/* test entry function */
	if (_pc_path_in != NULL)
	{
		fs_err = f_stat((TCHAR*)_pc_path_in, &fno);

		/* test return function */
		if (fs_err != FR_OK)
		{
			* _puc_bool_out = 1;
		}
		else
		{
			* _puc_bool_out = (fno.fattrib & AM_HID) ? 0 : 1;
		}
	}
	else
	{
		* _puc_bool_out = 1;
	}
}

void FS_ADAPT_get_last_modified_f(char* _pc_path_in, int * _pi_data_out, unsigned char * _puc_bool_out)
{
	FILINFO fno;
	FRESULT fs_err;

	/* test entry function */
	if ((_pc_path_in != NULL) && (_pi_data_out != NULL))
	{
		fs_err = f_stat((TCHAR*)_pc_path_in, &fno);

		/* test return function */
		if (fs_err != FR_OK)
		{
			* _puc_bool_out = 1;
		}
		else
		{
			_pi_data_out[5] = (int) ((fno.ftime & 31) * 2);
			_pi_data_out[4] = (int) ((fno.ftime >> 5) & 63);
			_pi_data_out[3] = (int) (fno.ftime >> 11);
			_pi_data_out[2] = (int) (fno.fdate & 31);
			_pi_data_out[1] = (int) ((fno.fdate >> 5) & 15);
			_pi_data_out[0] = (int) ((fno.fdate >> 9) + 1980);
			* _puc_bool_out = 0;
			#ifdef FS_ADAPT_DEBUG
				printf("FS_ADAPT_DEBUG [%s:%u] timestamp get : %u/%02u/%02u, %02u:%02u:%02u\n",  __MODULE__, __LINE__, _pi_data_out[0], _pi_data_out[1], _pi_data_out[2], _pi_data_out[3], _pi_data_out[4], _pi_data_out[5]);
			#endif
		}
	}
	else
	{
		* _puc_bool_out = 1;
	}
}

void FS_ADAPT_mk_dir_f(const char* _pc_path_in, unsigned char * _puc_bool_out)
{
	FRESULT fs_err = FR_INT_ERR;
	
	#ifdef FS_ADAPT_DEBUG
	printf("FS_ADAPT_DEBUG [%s:%u] : create dir adaptation layer enter %p %s\n", __MODULE__, __LINE__, _pc_path_in,_pc_path_in);
	#endif 
	
	/* test entry function */
	if ((_pc_path_in != NULL) && (_puc_bool_out != NULL))
	{		
		fs_err = f_mkdir((TCHAR*)_pc_path_in);
		
		/* test return function */
		if (fs_err != FR_OK)
		{
			* _puc_bool_out = 1;
		}
		else
		{
			* _puc_bool_out = 0;
		}
	}
	else
	{
		* _puc_bool_out = 1;
	}
	
	#ifdef FS_ADAPT_DEBUG
	printf("FS_ADAPT_DEBUG [%s:%u] : create dir adaptation layer %s (fs_err %d)\n", __MODULE__, __LINE__, _pc_path_in, fs_err );
	#endif 
}
void FS_ADAPT_open_dir_f(char* _pc_path_in, int * _pi_fd_out)
{
	FRESULT fs_err = FR_INT_ERR;
	FF_DIR * dir;
	POOL_status_t e_result;

	/* test entry function */
	if (_pc_path_in != NULL)
	{
		e_result = POOL_reserve_f(&gst_pool_dir_ctx, (void**)&dir);

		/* test if alloc is ok */
		if (e_result == POOL_NO_ERROR)
		{
			fs_err = f_opendir(dir, (TCHAR*)_pc_path_in);

			/* test return function */
			if (fs_err != FR_OK)
			{
				/* free directry pool */
				POOL_free_f(&gst_pool_dir_ctx, (void*) dir);
				* _pi_fd_out = LLFS_NOK;
			}
			else
			{
				* _pi_fd_out = (int) dir;
			}
		}
		else
		{
			*_pi_fd_out = LLFS_NOK;  // error
		}
	}
	else
	{
		* _pi_fd_out = LLFS_NOK;
	}
	
	#ifdef FS_ADAPT_DEBUG
		printf("FS_ADAPT_DEBUG [%s:%u] open dir %s (err %d)\n", __MODULE__, __LINE__, _pc_path_in, fs_err);
	#endif
}

void FS_ADAPT_close_dir_f(unsigned int _ui_fd_in, unsigned char * _puc_bool_out)
{
	FRESULT fs_err;

	if (_ui_fd_in != UINT32_MAX)
	{
		fs_err = f_closedir((FF_DIR*) _ui_fd_in);

		if (fs_err != FR_OK)
		{
			* _puc_bool_out = 1;
		}
		else
		{
			* _puc_bool_out = 0;
		}
	}
	
	/* free directry pool */
	POOL_free_f(&gst_pool_dir_ctx, (void*) _ui_fd_in);
	
}

void FS_ADAPT_read_dir_f(unsigned int _ui_fd_in,
						char* _pc_filename_in,
						unsigned char * _puc_bool_out)
{
	FRESULT fs_err = FR_INT_ERR;
	FILINFO fno;

	/* test entry function */
	if ((_pc_filename_in != NULL) && (_ui_fd_in != UINT32_MAX))
	{
		fs_err = f_readdir((FF_DIR*)_ui_fd_in, &fno);

		/* test return function or end of dir */
		if ((fs_err != FR_OK) || fno.fname[0] == 0)
		{
			* _puc_bool_out = 1;
		}
		else
		{
			strcpy((char*)_pc_filename_in, fno.fname);
			* _puc_bool_out = 0;
		}
	}
	else
	{
		* _puc_bool_out = 1;
	}
	
	#ifdef FS_ADAPT_DEBUG
		printf("FS_ADAPT_DEBUG [%s:%u] read dir return %s status %u (err %d)\n", __MODULE__, __LINE__, _pc_filename_in, * _puc_bool_out, fs_err);
	#endif
}

void FS_ADAPT_rename_f(char* _pc_old_name_in,
					  char* _pc_new_name_in,
					  unsigned char * _puc_bool_out)
{
	FRESULT fs_err;
	char* puc_temp;

	/* remove drive number and ":" in new name (due to fatfs limitation) perhaps solved in new release */
	puc_temp = strchr(_pc_new_name_in, ':');
	
	if (puc_temp != NULL)
	{
		_pc_new_name_in = puc_temp + 1;
	}
	
	#ifdef FS_ADAPT_DEBUG
			printf("FS_ADAPT_DEBUG [%s:%u] rename : old name %s, new name %s\n", __MODULE__, __LINE__, _pc_old_name_in, _pc_new_name_in);
	#endif
	fs_err = f_rename((TCHAR*) _pc_old_name_in, (TCHAR*) _pc_new_name_in);

	if (fs_err != FR_OK)
	{
		* _puc_bool_out = 1;
	}
	else
	{
		* _puc_bool_out = 0;
	}
}

void FS_ADAPT_set_last_modified_f(char* _pc_path_in,
		                             int _i_years_in,
		                             int _i_month_in,
		                             int _i_day_in,
		                             int _i_hour_in,
		                             int _i_minute_in,
		                             int _i_second_in,
		                             unsigned char * _puc_bool_out)
{
	FRESULT fs_err;
	FILINFO fno;

	if (_pc_path_in != NULL)
	{
		#ifdef FS_ADAPT_DEBUG
			printf("FS_ADAPT_DEBUG [%s:%u] timestamp set : %u/%02u/%02u, %02u:%02u:%02u\n", __MODULE__, __LINE__, _i_years_in, _i_month_in, _i_day_in, _i_hour_in, _i_minute_in, _i_second_in);
		#endif
		fno.fdate = (WORD)(((_i_years_in - 1980) * 512U) | _i_month_in * 32U | _i_day_in);
		fno.ftime = (WORD)(_i_hour_in * 2048U | _i_minute_in * 32U | _i_second_in / 2U);

		fs_err = f_utime((TCHAR*) _pc_path_in, &fno);

		if (fs_err != FR_OK)
		{
			* _puc_bool_out = 1;
		}
		else
		{
			* _puc_bool_out = 0;
		}
	}
	else
	{
		* _puc_bool_out = 1;
	}
}

void FS_ADAPT_set_read_only_f(char* _pc_path_in, unsigned char * _puc_bool_out)
{
	if (_pc_path_in != NULL)
	{
		ff_set_read_only(_pc_path_in, 1, _puc_bool_out);
	}
	else
	{
		* _puc_bool_out = 1;
	}
	
	#ifdef FS_ADAPT_DEBUG
			printf("FS_ADAPT_DEBUG [%s:%u] readonly set on %s status %u\n", __MODULE__, __LINE__, _pc_path_in, * _puc_bool_out);
	#endif
}

void FS_ADAPT_get_length_f(char* _pc_path_in, unsigned long * _pul_size_out)
{
	FILINFO fno;
	FRESULT fs_err = FR_INT_ERR;


	/* test entry function */
	if (_pc_path_in != NULL)
	{
		fs_err = f_stat((TCHAR*)_pc_path_in, &fno);

		/* test return function */
		if (fs_err != FR_OK)
		{
			* _pul_size_out = 0; //error
		}
		else
		{
			/* paste size */
			* _pul_size_out = fno.fsize;
		}
	}
	else
	{
		* _pul_size_out = 0; //error
	}
	
	#ifdef FS_ADAPT_DEBUG
		printf("FS_ADAPT_DEBUG [%s:%u] length of %s : %lu (err %d)\n", __MODULE__, __LINE__, _pc_path_in, * _pul_size_out, fs_err);
	#endif
	
}

void FS_ADAPT_create_f(char* _pc_path_in, FS_status_t * _pstatus_error_out)
{
	FRESULT fs_err;
	FIL fp;

  /* test entry function */
  if (_pc_path_in != NULL)
	{
		/* create the file */
		fs_err = f_open(&fp, (TCHAR*)_pc_path_in, FA_CREATE_NEW);

		/* test return function */
		if (fs_err == FR_OK)
		{
			/* close file */
			fs_err = f_close(&fp);

			/* test return function */
			if (fs_err != FR_OK)
			{
				* _pstatus_error_out = FS_IO_ERROR;  // error
			}
			else
			{
				* _pstatus_error_out = FS_NO_ERROR;
			}
		}
		else if (fs_err == FR_EXIST)
		{
			* _pstatus_error_out = FS_FILE_NOT_CREATED;  // file already exists
		}
		else
		{
			* _pstatus_error_out = FS_IO_ERROR;  // error
		}
	}
	else
	{
		* _pstatus_error_out = FS_IO_ERROR;  // error
	}
	
	#ifdef FS_ADAPT_DEBUG
	printf("FS_ADAPT_DEBUG [%s:%u] : create item adaptation layer %s (status %d)\n", __MODULE__, __LINE__, _pc_path_in, * _pstatus_error_out );
	#endif 
}

void FS_ADAPT_delete_f(char* _pc_path_in, unsigned char * _puc_bool_out)
{
	FRESULT fs_err;

	/* test entry function */
	if (_pc_path_in != NULL)
	{
		/* open file according to internal mode */
		fs_err = f_unlink((TCHAR*)_pc_path_in);

		/* test return function */
		if (fs_err != FR_OK)
		{
			* _puc_bool_out = 1;  // error
		}
		else
		{
			* _puc_bool_out = 0;
		}
	}
	else
	{
		* _puc_bool_out = 1;  // error
	}
}

void FS_ADAPT_check_access_f(char* _pc_path_in, FS_access_t _access_in, unsigned char * _puc_bool_out)
{
	FILINFO fno;
	FRESULT fs_err;

	/* Returns false by default */
	* _puc_bool_out = 1;

	/* test entry function */
	if (_pc_path_in != NULL)
	{
		/** FatFs only manages write permissions */
		if(_access_in == FS_ACCESS_WRITE)
		{
			fs_err = f_stat(_pc_path_in, &fno);
			/* test return function */
			if (fs_err == FR_OK)
			{
				/* If the file is in readonly -> writable is false.
				 * If the file is not in readonly -> writable is true. */
				* _puc_bool_out = (fno.fattrib & AM_RDO) ? 1 : 0;
			}
		}
		else
		{
			/* fatfs doesn't support other permissions so return always ok */
			* _puc_bool_out = 0;  // OK
		}
	}
}


void FS_ADAPT_set_permission_f(char* _pc_path_in,
							   FS_access_t _access_in,
		                       unsigned char _uc_enable,
		                       unsigned char _uc_owner_only,
		                       unsigned char * _puc_bool_out)
{
	/* Returns error by default */
	* _puc_bool_out = 1;

	if (_pc_path_in != NULL)
	{
		/* fatfs only support write permission */
		if(_access_in == FS_ACCESS_WRITE)
		{
			ff_set_read_only(_pc_path_in, !_uc_enable, _puc_bool_out);
		}
		else
		{	/* fatfs doesn't support other permissions so return OK only when enabling a permission */
			if(_uc_enable)
			{
				* _puc_bool_out = 0; // OK
			}
		}
	}
}

#ifdef __cplusplus
	}
#endif
