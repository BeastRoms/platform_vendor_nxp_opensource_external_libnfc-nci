/******************************************************************************
 *
 *  Copyright (C) 2011-2012 Broadcom Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at:
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
/******************************************************************************
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
*  Copyright 2019 NXP
*
******************************************************************************/
#pragma once
#include <pthread.h>

#include "config.h"
#include "nfc_hal_api.h"
#include "nfc_target.h"

#include <utils/RefBase.h>

namespace android {
namespace hardware {
namespace nfc {
namespace V1_0 {
struct INfc;
struct INfcClientCallback;
}
namespace V1_1 {
struct INfc;
struct INfcClientCallback;
} } } }

/*
 * Uncomment define ENABLE_ESE_CLIENT to
 * enable eSE client
 */
//#define ENABLE_ESE_CLIENT TRUE

namespace vendor {
namespace nxp {
namespace hardware {
namespace nfc {
namespace V1_0 {
struct INqNfc;
} } } } }

class NfcDeathRecipient;
class ThreadMutex {
 public:
  ThreadMutex();
  virtual ~ThreadMutex();
  void lock();
  void unlock();
  operator pthread_mutex_t*() { return &mMutex; }

 private:
  pthread_mutex_t mMutex;
};

class ThreadCondVar : public ThreadMutex {
 public:
  ThreadCondVar();
  virtual ~ThreadCondVar();
  void signal();
  void wait();
  operator pthread_cond_t*() { return &mCondVar; }
  operator pthread_mutex_t*() {
    return ThreadMutex::operator pthread_mutex_t*();
  }

 private:
  pthread_cond_t mCondVar;
};

class AutoThreadMutex {
 public:
  AutoThreadMutex(ThreadMutex& m);
  virtual ~AutoThreadMutex();
  operator ThreadMutex&() { return mm; }
  operator pthread_mutex_t*() { return (pthread_mutex_t*)mm; }

 private:
  ThreadMutex& mm;
};

class NfcAdaptation {
 public:
  virtual ~NfcAdaptation();
  void Initialize();
  void Finalize();
  void FactoryReset();
  void DeviceShutdown();
  static NfcAdaptation& GetInstance();
  tHAL_NFC_ENTRY* GetHalEntryFuncs();
  void DownloadFirmware();
  void GetVendorConfigs(std::map<std::string, ConfigValue>& configMap);
#if (NXP_EXTNS == TRUE)
  void GetNxpConfigs(std::map<std::string, ConfigValue>& configMap);
#endif
  void Dump(int fd);
#if (NXP_EXTNS == TRUE)
  nfc_nci_IoctlInOutData_t* mCurrentIoctlData;
#endif
 private:
  NfcAdaptation();
  void signal();
  static NfcAdaptation* mpInstance;
  static ThreadMutex sLock;
#if (NXP_EXTNS == TRUE)
  static ThreadMutex sIoctlLock;
#endif
  ThreadCondVar mCondVar;
  tHAL_NFC_ENTRY mHalEntryFuncs;  // function pointers for HAL entry points
  static android::sp<android::hardware::nfc::V1_0::INfc> mHal;
  static android::sp<android::hardware::nfc::V1_1::INfc> mHal_1_1;
  static android::sp<vendor::nxp::hardware::nfc::V1_0::INqNfc> mNqHal;
  static android::hardware::nfc::V1_1::INfcClientCallback* mCallback;
  static tHAL_NFC_CBACK* mHalCallback;
  static tHAL_NFC_DATA_CBACK* mHalDataCallback;
  static ThreadCondVar mHalOpenCompletedEvent;
#if (NXP_EXTNS == FALSE)
  static ThreadCondVar mHalCloseCompletedEvent;
#endif
  static android::sp<NfcDeathRecipient> mDeathRecipient;

  static uint32_t NFCA_TASK(uint32_t arg);
  static uint32_t Thread(uint32_t arg);
  void InitializeHalDeviceContext();
  static void HalDeviceContextCallback(nfc_event_t event,
                                       nfc_status_t event_status);
  static void HalDeviceContextDataCallback(uint16_t data_len, uint8_t* p_data);

  static void HalInitialize();
  static void HalTerminate();
  static void HalOpen(tHAL_NFC_CBACK* p_hal_cback,
                      tHAL_NFC_DATA_CBACK* p_data_cback);
  static void HalClose();
  static void HalCoreInitialized(uint16_t data_len,
                                 uint8_t* p_core_init_rsp_params);
  static void HalWrite(uint16_t data_len, uint8_t* p_data);
#if (NXP_EXTNS == TRUE)
  static int HalIoctl(long arg, void* p_data);
#endif
  static bool HalPrediscover();
  static void HalControlGranted();
  static void HalPowerCycle();
  static uint8_t HalGetMaxNfcee();
  static void HalDownloadFirmwareCallback(nfc_event_t event,
                                          nfc_status_t event_status);
  static void HalDownloadFirmwareDataCallback(uint16_t data_len,
                                              uint8_t* p_data);
};
