#include "blecomm.h"
#include <NimBLEDevice.h>

static constexpr uint32_t scan_time = 30 * 1000;

static DeviceCallback cb = nullptr;

class ScanCallbacks : public NimBLEScanCallbacks {
    void onDiscovered(const NimBLEAdvertisedDevice* device) override {
        if(cb != nullptr && device->getName() == DEVICE_NAME) cb(device->getAddress().getVal(), device->getManufacturerData(), device->getRSSI());
    }
    void onResult(const NimBLEAdvertisedDevice* device) override {
        if(cb != nullptr && device->getName() == DEVICE_NAME) cb(device->getAddress().getVal(), device->getManufacturerData(), device->getRSSI());
    }
    void onScanEnd(const NimBLEScanResults& _results, int _reason) override {
        NimBLEDevice::getScan()->start(scan_time, false, true);
    }
} ScanCallbacks;

static void ble_init_adv(NimBLEAdvertising* advertising, bool minimize_intervals) {
    advertising->reset();
    advertising->setMinInterval(minimize_intervals ? 100 : 1500);
    advertising->setMaxInterval(minimize_intervals ? 200 : 2500);
    advertising->setName(DEVICE_NAME);
}
void ble_init() {
    NimBLEDevice::init("yii");
    NimBLEAdvertising* advertising = NimBLEDevice::getAdvertising();
    ble_init_adv(advertising, false);
    advertising->start();
    Serial.println("adv start");

    NimBLEScan* scan = NimBLEDevice::getScan();
    scan->setScanCallbacks(&ScanCallbacks, true);
    scan->setActiveScan(true);
    scan->setMaxResults(0);
    scan->start(scan_time, false, true);
}
void ble_set_data(std::vector<uint8_t> vec, bool minimize_intervals) {
    NimBLEAdvertising* advertising = NimBLEDevice::getAdvertising();
    // advertising->stop();
    ble_init_adv(advertising, minimize_intervals);
    advertising->setManufacturerData(vec);
    advertising->start();
}
void ble_set_cb(DeviceCallback acb) {
    cb = acb;
}