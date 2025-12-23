/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_netxduo.c
  * @author  MCD Application Team
  * @brief   NetXDuo applicative file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2020-2021 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* USER CODE END Header */

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/* Includes ------------------------------------------------------------------*/
#include "app_netxduo.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "main.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define PRINT_IP_ADDRESS(addr)           do { \
                                              printf("Device IPv4 Address: %lu.%lu.%lu.%lu \r\n", \
                                              (addr >> 24) & 0xff,                    \
                                              (addr >> 16) & 0xff,                    \
                                              (addr >> 8) & 0xff,                     \
                                              (addr & 0xff));                         \
                                         } while(0)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TX_THREAD      NxAppThread;
NX_PACKET_POOL NxAppPool;
NX_IP          NetXDuoEthIpInstance;
/* USER CODE BEGIN PV */
NX_UDP_SOCKET UDPSocket;
ULONG IpAddress;
ULONG NetMask;
NXD_ADDRESS ipv6_address;

extern UART_HandleTypeDef huart3;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static VOID nx_app_thread_entry (ULONG thread_input);
/* USER CODE BEGIN PFP */
void printIPv6(NXD_ADDRESS ipv6)
{
	printf("Device IPv6 Address: %x:%x:%x:%x:%x:%x:%x:%x\r\n",
			(unsigned int)ipv6.nxd_ip_address.v6[0] >> 16,
			(unsigned int)ipv6.nxd_ip_address.v6[0] & 0xFFFF,
			(unsigned int)ipv6.nxd_ip_address.v6[1] >> 16,
			(unsigned int)ipv6.nxd_ip_address.v6[1] & 0xFFFF,
			(unsigned int)ipv6.nxd_ip_address.v6[2] >> 16,
			(unsigned int)ipv6.nxd_ip_address.v6[2] & 0xFFFF,
			(unsigned int)ipv6.nxd_ip_address.v6[3] >> 16,
			(unsigned int)ipv6.nxd_ip_address.v6[3] & 0xFFFF
	);
}
/* USER CODE END PFP */

/**
  * @brief  Application NetXDuo Initialization.
  * @param memory_ptr: memory pointer
  * @retval int
  */
UINT MX_NetXDuo_Init(VOID *memory_ptr)
{
  UINT ret = NX_SUCCESS;
  TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL*)memory_ptr;
  CHAR *pointer;

  /* USER CODE BEGIN MX_NetXDuo_MEM_POOL */

  /* USER CODE END MX_NetXDuo_MEM_POOL */

  /* USER CODE BEGIN 0 */

  /* USER CODE END 0 */

  /* Initialize the NetXDuo system. */
  nx_system_initialize();

    /* Allocate the memory for packet_pool.  */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, NX_APP_PACKET_POOL_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

  /* Create the Packet pool to be used for packet allocation,
   * If extra NX_PACKET are to be used the NX_APP_PACKET_POOL_SIZE should be increased
   */
  ret = nx_packet_pool_create(&NxAppPool, "NetXDuo App Pool", DEFAULT_PAYLOAD_SIZE, pointer, NX_APP_PACKET_POOL_SIZE);

  if (ret != NX_SUCCESS)
  {
	printf("nx_packet_pool_create failed: 0x%02X\r\n", (unsigned int)ret);
    return NX_POOL_ERROR;
  }

    /* Allocate the memory for Ip_Instance */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, Nx_IP_INSTANCE_THREAD_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

   /* Create the main NX_IP instance */
  ret = nx_ip_create(&NetXDuoEthIpInstance, "NetX Ip instance", NX_APP_DEFAULT_IP_ADDRESS, NX_APP_DEFAULT_NET_MASK, &NxAppPool, nx_stm32_eth_driver,
                     pointer, Nx_IP_INSTANCE_THREAD_SIZE, NX_APP_INSTANCE_PRIORITY);

  if (ret != NX_SUCCESS)
  {
	printf("nx_ip_create failed: 0x%02X\r\n", (unsigned int)ret);
    return NX_NOT_SUCCESSFUL;
  }

    /* Allocate the memory for ARP */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, DEFAULT_ARP_CACHE_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

  /* Enable the ARP protocol and provide the ARP cache size for the IP instance */

  /* USER CODE BEGIN ARP_Protocol_Initialization */

  /* USER CODE END ARP_Protocol_Initialization */

  ret = nx_arp_enable(&NetXDuoEthIpInstance, (VOID *)pointer, DEFAULT_ARP_CACHE_SIZE);

  if (ret != NX_SUCCESS)
  {
	printf("nx_arp_enable failed: 0x%02X\r\n", (unsigned int)ret);
    return NX_NOT_SUCCESSFUL;
  }

  /* Enable the ICMP */

  /* USER CODE BEGIN ICMP_Protocol_Initialization */

  /* USER CODE END ICMP_Protocol_Initialization */

  ret = nx_icmp_enable(&NetXDuoEthIpInstance);
  if (ret != NX_SUCCESS)
  {
    printf("nx_icmp_enable failed: 0x%02X\r\n", (unsigned int)ret);
    return NX_NOT_SUCCESSFUL;
  }

  /* Enable TCP Protocol */

  /* USER CODE BEGIN TCP_Protocol_Initialization */

  /* USER CODE END TCP_Protocol_Initialization */

  ret = nx_tcp_enable(&NetXDuoEthIpInstance);
  if (ret != NX_SUCCESS)
  {
    printf("nx_tcp_enable failed: 0x%02X\r\n", (unsigned int)ret);
    return NX_NOT_SUCCESSFUL;
  }

  /* Enable the UDP protocol required for  DHCP communication */

  /* USER CODE BEGIN UDP_Protocol_Initialization */

  /* USER CODE END UDP_Protocol_Initialization */

  ret = nx_udp_enable(&NetXDuoEthIpInstance);
  if (ret != NX_SUCCESS)
  {
    printf("nx_udp_enable failed: 0x%02X\r\n", (unsigned int)ret);
    return NX_NOT_SUCCESSFUL;
  }

   /* Allocate the memory for main thread   */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, NX_APP_THREAD_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

  /* Create the main thread */
  ret = tx_thread_create(&NxAppThread, "NetXDuo App thread", nx_app_thread_entry , 0, pointer, NX_APP_THREAD_STACK_SIZE,
                         NX_APP_THREAD_PRIORITY, NX_APP_THREAD_PRIORITY, TX_NO_TIME_SLICE, TX_AUTO_START);

  if (ret != TX_SUCCESS)
  {
    printf("tx_thread_create failed: 0x%02X\r\n", (unsigned int)ret);
    return TX_THREAD_ERROR;
  }

  /* USER CODE BEGIN MX_NetXDuo_Init */

  /* Assume ip has been created and IPv4 services (such as ARP,
       ICMP, have been enabled before doing IPv6 init */

    /* Enable IPv6 */
    ret = nxd_ipv6_enable(&NetXDuoEthIpInstance);

    if(ret != NX_SUCCESS)
    {
        /* nxd_ipv6_enable failed. */
    	Error_Handler();
    }

    /* Configure the IPv6 address */

    /*
     * This IPv6 address is specific to my routers available IPv6 subnet addresses
     */
    ipv6_address.nxd_ip_version = NX_IP_VERSION_V6;

    /* Board IPv6: 2600:1700:f3d0:8150:0:0:0:abcd  (same /64 as your PC) */
    ipv6_address.nxd_ip_address.v6[0] = 0x26001700;   // 2600:1700
    ipv6_address.nxd_ip_address.v6[1] = 0xF3D08150;   // f3d0:8150
    ipv6_address.nxd_ip_address.v6[2] = 0x00000000;   // 0000:0000
    ipv6_address.nxd_ip_address.v6[3] = 0x0000ABCD;   // 0000:abcd (pick any unique host ID)

    /* Configure global address of the primary interface. */
    ret = nxd_ipv6_address_set(&NetXDuoEthIpInstance, 0,
                                  &ipv6_address, 64, NX_NULL);

    /* Enable ICMPv6 */
    ret = nxd_icmp_enable(&NetXDuoEthIpInstance);
    if(ret != NX_SUCCESS)
    {
        /* nxd_icmp_enable failed. */
    	Error_Handler();
    }

  /* USER CODE END MX_NetXDuo_Init */

  return ret;
}

/**
* @brief  Main thread entry.
* @param thread_input: ULONG user argument used by the thread entry
* @retval none
*/
static VOID nx_app_thread_entry (ULONG thread_input)
{
  /* USER CODE BEGIN Nx_App_Thread_Entry 0 */
	UINT ret;
	ULONG bytes_read;

    ULONG    ipv6_prefix;
    UINT     ipv6_interface_index;

	UCHAR data_buffer[512];
	NX_PACKET *data_packet;

	NXD_ADDRESS device_address;

	/*
	 * Print IPv4
	 */
	ret = nx_ip_address_get(&NetXDuoEthIpInstance, &IpAddress, &NetMask);
	if (ret != TX_SUCCESS)
	{
	Error_Handler();
	}
	else
	{
		PRINT_IP_ADDRESS(IpAddress);
	}


	/*
	 * Print IPv6
	 */
	nxd_ipv6_address_get( &NetXDuoEthIpInstance, 0, &device_address, &ipv6_prefix, &ipv6_interface_index );
	printIPv6(device_address);


	/*
	 * Create a UDP Socket and bind to it
	 */

	ret = nx_udp_socket_create(&NetXDuoEthIpInstance, &UDPSocket, "UDP Server Socket", NX_IP_NORMAL, NX_FRAGMENT_OKAY, NX_IP_TIME_TO_LIVE, 512);
	if (ret != NX_SUCCESS)
	{
		Error_Handler();
	}

  /* Bind to port 6000 */
	ret = nx_udp_socket_bind(&UDPSocket, 6000, TX_WAIT_FOREVER);
	if (ret != NX_SUCCESS)
	{
		Error_Handler();
	}
	else
	{
		printf("UDP Server listening on PORT 6000.. \r\n");
	}

	/*
	 * Main Task Loop
	 * Waits 1 second (100 centiseconds) for each UDP packet. If received, print out message
	 */
	printf("Entering UDP receive loop...\r\n");

	/* ------------------------------
	     * TEST: Ping PC from STM32
	     * ------------------------------ */

	    /* Replace with your PC IPv4 address (one of the 192.168.1.xxx you saw) */
	    ULONG pc_ip = IP_ADDRESS(192, 168, 1, 135);   // example: 192.168.1.187

	    NX_PACKET *ping_response = NX_NULL;

	    ret = nx_icmp_ping(&NetXDuoEthIpInstance,
	                       pc_ip,
	                       "abc", 3,              /* ping payload, size */
	                       &ping_response,
	                       NX_IP_PERIODIC_RATE);  /* timeout: 1 second */

	    if (ret == NX_SUCCESS)
	    {
	        printf("Ping to PC (192.168.1.135) OK\r\n");
	        nx_packet_release(ping_response);
	    }
	    else
	    {
	        printf("Ping to PC failed, ret=0x%02X\r\n", (unsigned int)ret);
	    }

//	while (1)
//	    {
//	        TX_MEMSET(data_buffer, 0, sizeof(data_buffer));
//
//	        /* Wait FOREVER for a UDP packet (for debugging) */
//	        ret = nx_udp_socket_receive(&UDPSocket, &data_packet, NX_WAIT_FOREVER);
//
//	        if (ret == NX_SUCCESS)
//	        {
//	            nx_packet_data_retrieve(data_packet, data_buffer, &bytes_read);
//
//	            printf("RX IPv4 %lu bytes: ", bytes_read);
//	            HAL_UART_Transmit(&huart3, (uint8_t *)data_buffer, bytes_read, 0xFFFF);
//	            printf("\r\n");
//
//	            /* IMPORTANT: release the packet back to the pool */
//	            nx_packet_release(data_packet);
//	        }
//	        else
//	        {
//	            /* This should not normally happen with NX_WAIT_FOREVER,
//	               but we print it in case of errors. */
//	            printf("nx_udp_socket_receive error: 0x%02X\r\n", (unsigned int)ret);
//	        }
//	    }

		while (1)
		{
		    TX_MEMSET(data_buffer, '\0', sizeof(data_buffer));

		    /* wait for data for 1 sec */
		    ret = nx_udp_socket_receive(&UDPSocket, &data_packet, 100);

		    if (ret == NX_SUCCESS)
		    {
		    	nx_packet_data_retrieve(data_packet, data_buffer, &bytes_read);

	        /* Print our received data on UART com port*/
				  HAL_UART_Transmit(&huart3, (uint8_t *)data_buffer, bytes_read, 0xFFFF);
				  printf("\r\n");	// new line to make print out more readable

			/* FIX! ->>>> release packet back to pool */
				  nx_packet_release(data_packet);
		    }

		}

  /* USER CODE END Nx_App_Thread_Entry 0 */

}
/* USER CODE BEGIN 2 */

/* USER CODE END 2 */
