#pragma once
#define DAP_APP_NAME NODE_NETNAME "-node"

//#ifdef _WIN32
//  #define SYSTEM_PREFIX "%USERPROFILE%/opt/"DAP_APP_NAME
//#else
  #define SYSTEM_PREFIX "/opt/" DAP_APP_NAME
//  #define SYSTEM_PREFIX "opt/"DAP_APP_NAME
//#endif

#define LOCAL_PREFIX "~/." DAP_APP_NAME

#define SYSTEM_CONFIGS_DIR SYSTEM_PREFIX"/etc"
#define LOCAL_CONFIGS_DIR LOCAL_PREFIX"/etc"
#define SYSTEM_LOGS_DIR SYSTEM_PREFIX"/var/log"

#define SYSTEM_CONFIG_GLOBAL_FILENAME SYSTEM_PREFIX"/etc/" DAP_APP_NAME".cfg"
#define LOCAL_CONFIG_GLOBAL LOCAL_PREFIX"/etc/" DAP_APP_NAME".cfg"

#define SYSTEM_PID_FILE_PATH SYSTEM_PREFIX"/run/" DAP_APP_NAME".pid"
#define LOCAL_PID_FILE_PATH SYSTEM_PREFIX"/run/" DAP_APP_NAME".pid"
