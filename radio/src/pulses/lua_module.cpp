/*
 * Copyright (C) EdgeTX
 *
 * License GPLv2: http://www.gnu.org/licenses/gpl-2.0.html
 */

#include "lua_module.h"
#include "lua/lua_api.h"
#include "hal/module_driver.h"
#include "hal/module_port.h"

#if defined(LUA)

#if !defined(SIMU)

static void* luaModuleInit(uint8_t module)
{
  etx_serial_init params = {
    .baudrate = 115200,
    .encoding = ETX_Encoding_8N1,
    .direction = ETX_Dir_TX_RX,
    .polarity = ETX_Pol_Normal,
  };

  auto mod_st = modulePortInitSerial(module, ETX_MOD_PORT_UART, &params, false);
  if (mod_st) {
    auto drv = modulePortGetSerialDrv(mod_st->tx);
    auto ctx = modulePortGetCtx(mod_st->tx);
    if (drv && ctx) {
      luaSetSendCb(ctx, drv->sendByte);
      luaSetGetSerialByte(ctx, drv->getByte);
    }
  }

  return (void*)mod_st;
}

static void luaModuleDeInit(void* ctx)
{
  luaSetSendCb(nullptr, nullptr);
  luaSetGetSerialByte(nullptr, nullptr);

  auto mod_st = (etx_module_state_t*)ctx;
  if (mod_st) {
    modulePortDeInit(mod_st);
  }
}

#else // SIMU

static void* luaModuleInit(uint8_t module) { return (void*)1; }
static void luaModuleDeInit(void* ctx) {}

#endif // !SIMU

const etx_proto_driver_t LuaModuleDriver = {
  .protocol = PROTOCOL_CHANNELS_LUA,
  .init = luaModuleInit,
  .deinit = luaModuleDeInit,
  .sendPulses = nullptr,
  .processData = nullptr,
  .processFrame = nullptr,
  .onConfigChange = nullptr,
  .txCompleted = nullptr,
};

#endif // LUA
