/******************************************************************************
 *
 *  Copyright 2018 NXP
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 ******************************************************************************/
#ifndef ANDROID_HARDWARE_HAL_NXPNFC_V1_0_H
#define ANDROID_HARDWARE_HAL_NXPNFC_V1_0_H

#define NFC_NCI_NXP_PN54X_HARDWARE_MODULE_ID "nfc_nci.pn54x"
#define MAX_IOCTL_TRANSCEIVE_CMD_LEN  256
#define MAX_IOCTL_TRANSCEIVE_RESP_LEN 256
#define MAX_ATR_INFO_LEN              128

enum {
    HAL_NFC_IOCTL_NCI_TRANSCEIVE = 0xF1,
    HAL_NFC_IOCTL_SEND_FLASH_UPDATE,
    HAL_NFC_IOCTL_NFC_JCOP_DWNLD
};

enum {
  HAL_NFC_IOCTL_P61_IDLE_MODE = 0,
  HAL_NFC_IOCTL_P61_WIRED_MODE,
  HAL_NFC_IOCTL_P61_PWR_MODE,
  HAL_NFC_IOCTL_P61_DISABLE_MODE,
  HAL_NFC_IOCTL_P61_ENABLE_MODE,
  HAL_NFC_IOCTL_SET_BOOT_MODE,
  HAL_NFC_IOCTL_GET_CONFIG_INFO,
  HAL_NFC_IOCTL_CHECK_FLASH_REQ,
  HAL_NFC_IOCTL_FW_DWNLD,
  HAL_NFC_IOCTL_FW_MW_VER_CHECK,
  HAL_NFC_IOCTL_DISABLE_HAL_LOG,
  HAL_NFC_IOCTL_P61_GET_ACCESS,
  HAL_NFC_IOCTL_P61_REL_ACCESS,
  HAL_NFC_IOCTL_ESE_CHIP_RST,
  HAL_NFC_IOCTL_REL_SVDD_WAIT,
  HAL_NFC_IOCTL_SET_JCP_DWNLD_ENABLE,
  HAL_NFC_IOCTL_SET_JCP_DWNLD_DISABLE,
  HAL_NFC_IOCTL_SET_NFC_SERVICE_PID,
  HAL_NFC_IOCTL_REL_DWP_WAIT,
  HAL_NFC_IOCTL_GET_FEATURE_LIST,
  HAL_NFC_IOCTL_SPI_DWP_SYNC,
  HAL_NFC_IOCTL_RF_STATUS_UPDATE,
  HAL_NFC_SET_SPM_PWR,
  HAL_NFC_SET_POWER_SCHEME,
  HAL_NFC_GET_SPM_STATUS,
  HAL_NFC_GET_ESE_ACCESS,
  HAL_NFC_SET_DWNLD_STATUS,
  HAL_NFC_INHIBIT_PWR_CNTRL,
  HAL_NFC_IOCTL_ESE_JCOP_DWNLD,
  HAL_NFC_IOCTL_ESE_UPDATE_COMPLETE
#if (NXP_EXTNS == TRUE)
 ,HAL_NFC_IOCTL_SET_TRANSIT_CONFIG
#endif
};

enum {
    //HAL_NFC_ENABLE_I2C_FRAGMENTATION_EVT = 0x07,
    HAL_NFC_POST_MIN_INIT_CPLT_EVT       = 0x08
};
/*
 * Data structures provided below are used of Hal Ioctl calls
 */
/*
 * nfc_nci_ExtnCmd_t shall contain data for commands used for transceive command in ioctl
 */
typedef struct
{
    uint16_t cmd_len;
    uint8_t  p_cmd[MAX_IOCTL_TRANSCEIVE_CMD_LEN];
} nfc_nci_ExtnCmd_t;

/*
 * nfc_nci_ExtnRsp_t shall contain response for command sent in transceive command
 */
typedef struct
{
    uint16_t rsp_len;
    uint8_t  p_rsp[MAX_IOCTL_TRANSCEIVE_RESP_LEN];
} nfc_nci_ExtnRsp_t;
/*
 * TransitConfig_t shall contain transit config value and transit
 * Configuration length
 */
typedef struct
{
    long len;
    char *val;
} TransitConfig_t;
/*
 * InputData_t :ioctl has multiple subcommands
 * Each command has corresponding input data which needs to be populated in this
 */
typedef union {
    uint16_t          bootMode;
    uint8_t           halType;
    nfc_nci_ExtnCmd_t nciCmd;
    uint32_t          timeoutMilliSec;
    long              nfcServicePid;
    TransitConfig_t transitConfig;
}InputData_t;
/*
 * nfc_nci_ExtnInputData_t :Apart from InputData_t, there are context data
 * which is required during callback from stub to proxy.
 * To avoid additional copy of data while propagating from libnfc to Adaptation
 * and Nfcstub to ncihal, common structure is used. As a sideeffect, context data
 * is exposed to libnfc (Not encapsulated).
 */
typedef struct {
    /*context to be used/updated only by users of proxy & stub of Nfc.hal
    * i.e, NfcAdaptation & hardware/interface/Nfc.
    */
    void*       context;
    InputData_t data;
    uint8_t data_source;
    long level;
}nfc_nci_ExtnInputData_t;

/*
 * outputData_t :ioctl has multiple commands/responses
 * This contains the output types for each ioctl.
 */
typedef union{
    uint32_t            status;
    nfc_nci_ExtnRsp_t   nciRsp;
    uint8_t             nxpNciAtrInfo[MAX_ATR_INFO_LEN];
    uint32_t            p61CurrentState;
    uint16_t            fwUpdateInf;
    uint16_t            fwDwnldStatus;
    uint16_t            fwMwVerStatus;
    uint8_t             chipType;
}outputData_t;

/*
 * nfc_nci_ExtnOutputData_t :Apart from outputData_t, there are other information
 * which is required during callback from stub to proxy.
 * For ex (context, result of the operation , type of ioctl which was completed).
 * To avoid additional copy of data while propagating from libnfc to Adaptation
 * and Nfcstub to ncihal, common structure is used. As a sideeffect, these data
 * is exposed(Not encapsulated).
 */
typedef struct {
    /*ioctlType, result & context to be used/updated only by users of
     * proxy & stub of Nfc.hal.
     * i.e, NfcAdaptation & hardware/interface/Nfc
     * These fields shall not be used by libnfc or halimplementation*/
    uint64_t     ioctlType;
    uint32_t     result;
    void*        context;
    outputData_t data;
}nfc_nci_ExtnOutputData_t;

/*
 * nfc_nci_IoctlInOutData_t :data structure for input & output
 * to be sent for ioctl command. input is populated by client/proxy side
 * output is provided from server/stub to client/proxy
 */
typedef struct {
    nfc_nci_ExtnInputData_t  inp;
    nfc_nci_ExtnOutputData_t out;
}nfc_nci_IoctlInOutData_t;

enum NxpNfcHalStatus {
    /** In case of an error, HCI network needs to be re-initialized */
    HAL_NFC_STATUS_RESTART = 0x30,
};

typedef union {
    uint8_t ese_jcop_download_state;
} nfcIoctlData_t;
extern nfcIoctlData_t  nfcioctldata;

/*
 * nxpnfc_nci_device_t :data structure for nxp's extended nfc_nci_device
 * Extra features added are
 * -ioctl(manage sync between  and DWP & SPI)
 * -check request for fw download
 */
typedef struct nxpnfc_nci_device{
    //nfc_nci_device_t nci_device;
    /*
    * (*ioctl)() For P61 power management synchronization
    * between NFC Wired and SPI.
    */
    int (*ioctl)(const struct nxpnfc_nci_device *p_dev, long arg, void *p_data);
    /*
    * (*check_fw_dwnld_flag)() Is called to get FW downlaod request.
    */
    int (*check_fw_dwnld_flag)(const struct nxpnfc_nci_device *p_dev, uint8_t* param1);
}nxpnfc_nci_device_t;

#endif  // ANDROID_HARDWARE_HAL_NXPNFC_V1_0_H
