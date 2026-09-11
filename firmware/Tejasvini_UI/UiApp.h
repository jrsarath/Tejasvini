#ifndef TEJASVINI_UI_APP_H_
#define TEJASVINI_UI_APP_H_

#include "DisplayManager.h"
#include "ProtocolClient.h"
#include "StatusModel.h"
#include "TouchManager.h"
#include "UiConfig.h"

class UiApp {
public:
  UiApp();

  void setup();
  void loop();

  StatusModel &getStatusModel() { return model_; }
  ProtocolClient &getProtocolClient() { return client_; }

  static UiApp *getInstance();

private:
  StatusModel model_;
  ProtocolClient client_;
  TouchManager touch_;
  DisplayManager display_;

  static UiApp *s_instance;
};

#endif // TEJASVINI_UI_APP_H_
