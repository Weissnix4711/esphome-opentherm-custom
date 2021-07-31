#include "esphome.h"
#include "esphome/components/sensor/sensor.h"
#include "OpenTherm.h"
#include "opentherm_climate.h"

// Pins to OpenTherm Adapter
int inPin = D2; 
int outPin = D1;
OpenTherm ot(inPin, outPin, false);

ICACHE_RAM_ATTR void handleInterrupt() {
    ot.handleInterrupt();
}

class OpenthermComponent: public PollingComponent {
private:
  const char *TAG = "opentherm_component";
public:
  OpenthermClimate *hotWaterClimate = new OpenthermClimate();
  OpenthermClimate *flowWaterClimate = new OpenthermClimate();
  
  // Set 3 sec. to give time to read all sensors (and not appear in HA as not available)
  OpenthermComponent(): PollingComponent(5000) {
  }

  void setup() override {
    // This will be called once to set up the component
    // think of it as the setup() call in Arduino
      ESP_LOGD("opentherm_component", "Setup");

      ot.begin(handleInterrupt);

      hotWaterClimate->setup();
      flowWaterClimate->setup();
  }
  
  float getHotWaterTemperature() {
      unsigned long response = ot.sendRequest(ot.buildRequest(OpenThermRequestType::READ, OpenThermMessageID::Tdhw, 0));
      return ot.isValidResponse(response) ? ot.getFloat(response) : -1;
  }

  bool setHotWaterTemperature(float temperature) {
        unsigned int data = ot.temperatureToData(temperature);
      unsigned long request = ot.buildRequest(OpenThermRequestType::WRITE, OpenThermMessageID::TdhwSet, data);
      unsigned long response = ot.sendRequest(request);
      return ot.isValidResponse(response);
  }

  void update() override {

    ESP_LOGD("opentherm_component", "update flowWaterClimate: %f", flowWaterClimate->target_temperature);
    ESP_LOGD("opentherm_component", "update hotWaterClimate: %f", hotWaterClimate->target_temperature);

    bool enableHotWater = hotWaterClimate->mode == ClimateMode::CLIMATE_MODE_HEAT;
    bool enableCooling = false; // this boiler is for heating only

    
    //Set/Get Boiler Status
    auto response = ot.setBoilerStatus((flowWaterClimate->mode == ClimateMode::CLIMATE_MODE_HEAT), enableHotWater, enableCooling);
    bool isFlameOn = ot.isFlameOn(response);
    bool isCentralHeatingActive = ot.isCentralHeatingActive(response);
    bool isHotWaterActive = ot.isHotWaterActive(response);
    float hotWater_temperature = getHotWaterTemperature();

    // Set temperature depending on room thermostat
    float heating_target_temperature = flowWaterClimate->target_temperature;
    ESP_LOGD("opentherm_component", "setBoilerTemperature  at %f °C (from flow water climate)", flowWaterClimate->target_temperature);
    ot.setBoilerTemperature(heating_target_temperature);

    // Set hot water temperature
    setHotWaterTemperature(hotWaterClimate->target_temperature);

    float boilerTemperature = ot.getBoilerTemperature();

    // Publish status of thermostat that controls hot water
    hotWaterClimate->current_temperature = hotWater_temperature;
    hotWaterClimate->action = isHotWaterActive ? ClimateAction::CLIMATE_ACTION_HEATING : ClimateAction::CLIMATE_ACTION_OFF;
    hotWaterClimate->publish_state();
    
    // Publish status of thermostat that controls heating
    flowWaterClimate->current_temperature = boilerTemperature;
    flowWaterClimate->action = isCentralHeatingActive && isFlameOn && !isHotWaterActive ? ClimateAction::CLIMATE_ACTION_HEATING : ClimateAction::CLIMATE_ACTION_OFF;
    flowWaterClimate->publish_state();
  }
};