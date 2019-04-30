/*
 * Authors:
 * Dmitriy A. Gearasimov <kahovski@gmail.com>
 * DeM Labs Inc.   https://demlabs.net
 * DeM Labs Open source community https://github.com/demlabsinc
 * Copyright  (c) 2017-2019
 * All rights reserved.

 This file is part of DAP (Deus Applications Prototypes) the open source project

    DAP (Deus Applicaions Prototypes) is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    DAP is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with any DAP based project.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <unistd.h>
#include <sys/types.h>
#include <getopt.h>
#include <signal.h>

#define LOG_TAG "main"

#include "sig_unix_handler.h"
#include "dap_config.h"
#include "dap_server.h"
#include "dap_http.h"
#include "dap_http_folder.h"
#include "dap_events.h"
#include "dap_enc.h"
#include "dap_enc_ks.h"
#include "dap_enc_http.h"
#include "dap_chain.h"
#include "dap_chain_wallet.h"

#include "dap_chain_cs_dag.h"
#include "dap_chain_cs_dag_poa.h"
#include "dap_chain_cs_dag_pos.h"

#include "dap_chain_net.h"
#include "dap_chain_net_srv.h"
#include "dap_chain_net_srv_app.h"
#include "dap_chain_net_srv_app_db.h"
#include "dap_chain_net_srv_datum.h"
#include "dap_chain_net_srv_datum_pool.h"
#include "dap_chain_net_srv_vpn.h"
#include "dap_chain_global_db.h"
#include "dap_chain_mempool.h"
#include "dap_chain_node_cli.h"

#include "dap_stream_session.h"
#include "dap_stream.h"
#include "dap_stream_ctl.h"
#include "dap_stream_ch_vpn.h"
#include "dap_stream_ch_chain.h"
#include "dap_stream_ch_chain_net.h"
#include "dap_stream_ch_chain_net_srv.h"

#include "dap_common.h"
#include "dap_client_remote.h"
#include "dap_http_simple.h"
#include "dap_process_manager.h"
#include "dap_traffic_track.h"

#define DAP_APP_NAME NODE_NETNAME"-node"
#define SYSTEM_PREFIX "/opt/"DAP_APP_NAME
#define LOCAL_PREFIX "~/."DAP_APP_NAME

#define SYSTEM_CONFIGS_DIR SYSTEM_PREFIX"/etc"
#define LOCAL_CONFIGS_DIR LOCAL_PREFIX"/etc"

#define SYSTEM_CONFIG_GLOBAL_FILENAME SYSTEM_PREFIX"/etc/"DAP_APP_NAME".cfg"
#define LOCAL_CONFIG_GLOBAL LOCAL_PREFIX"/etc/"DAP_APP_NAME".cfg"

#define SYSTEM_PID_FILE_PATH SYSTEM_PREFIX"/run/"DAP_APP_NAME".pid"
#define LOCAL_PID_FILE_PATH SYSTEM_PREFIX"/run/"DAP_APP_NAME".pid"

#define ENC_HTTP_URL "/enc_init"
#define STREAM_CTL_URL "/stream_ctl"

#define STREAM_URL "/stream"
#define SLIST_URL "/nodelist"
#define MEMPOOL_URL "/mempool"
#define MAIN_URL "/"

void parse_args(int argc, const char * argv[]);
void exit_if_server_already_running(void);

int main(int argc, const char * argv[])
{
    dap_server_t * l_server = NULL; // DAP Server instance
    int rc;

    dap_config_init(SYSTEM_CONFIGS_DIR);
    if((g_config = dap_config_open(DAP_APP_NAME) ) == NULL) {
        log_it(L_CRITICAL,"Can't init general configurations");
        return -1;
    }

    if(dap_common_init(DAP_APP_NAME"_logs.txt")!=0){
        log_it(L_CRITICAL,"Can't init common functions module");
        return -2;
    }

    parse_args(argc, argv);

    // change to dap_config_get_item_int_default when it's will be possible
    size_t l_thread_cnt;
    const char *s_thrd_cnt = dap_config_get_item_str(g_config, "resources", "threads_cnt");
    if(s_thrd_cnt != NULL)
        l_thread_cnt = (size_t)atoi(s_thrd_cnt);

    if (l_thread_cnt == 0 )
        l_thread_cnt = (size_t)sysconf(_SC_NPROCESSORS_ONLN);

    if(dap_server_init(l_thread_cnt)!=0){
        log_it(L_CRITICAL,"Can't init socket server module");
        return -4;
    }
    if(dap_http_init()!=0){
        log_it(L_CRITICAL,"Can't init http server module");
        return -5;
    }
    if(dap_http_folder_init()!=0){
        log_it(L_CRITICAL,"Can't init http server module");
        return -55;
    }

    if(dap_enc_init() !=0){
        log_it(L_CRITICAL,"Can't init encryption module");
        return -56;
    }

    if(dap_enc_ks_init(false, 60*60*2 ) !=0){
        log_it(L_CRITICAL,"Can't init encryption key storage module");
        return -57;
    }
    
    if(dap_chain_global_db_init(g_config)) {
        log_it(L_CRITICAL, "Can't init global db module");
        return -58;
    }

    if(dap_datum_mempool_init()) {
        log_it(L_CRITICAL, "Can't init mempool module");
        return -59;
    }

    if( dap_chain_init() !=0){
        log_it(L_CRITICAL,"Can't init dap chain modules");
        return -60;
    }


    if( dap_chain_wallet_init() !=0){
        log_it(L_CRITICAL,"Can't init dap chain wallet module");
        return -61;
    }


    if( dap_chain_cs_dag_init() !=0){
        log_it(L_CRITICAL,"Can't init dap chain dag consensus module");
        return -6;
    }

    if( dap_chain_cs_dag_poa_init() !=0){
        log_it(L_CRITICAL,"Can't init dap chain dag consensus PoA module");
        return -6;
    }

    if( dap_chain_cs_dag_pos_init() !=0){
        log_it(L_CRITICAL,"Can't init dap chain dag consensus PoA module");
        return -6;
    }

    if( dap_chain_net_init() !=0){
        log_it(L_CRITICAL,"Can't init dap chain network module");
        return -6;
    }

    if( dap_chain_net_srv_init() !=0){
        log_it(L_CRITICAL,"Can't init dap chain network service module");
        return -6;
    }

    if( dap_chain_net_srv_app_init() !=0){
        log_it(L_CRITICAL,"Can't init dap chain network service applications module");
        return -6;
    }


    if( dap_chain_net_srv_datum_init() !=0){
        log_it(L_CRITICAL,"Can't init dap chain network service datum module");
        return -6;
    }
    if( dap_chain_net_srv_datum_pool_init() !=0){
        log_it(L_CRITICAL,"Can't init dap chain network service datum pool module");
        return -6;
    }
    if( dap_chain_net_srv_vpn_init(g_config) !=0){
        log_it(L_CRITICAL,"Can't init dap chain network service vpn module");
        return -6;
    }

    if(enc_http_init() !=0){
        log_it(L_CRITICAL,"Can't init encryption http session storage module");
        return -81;
    }

    if(dap_stream_init() != 0 ){
        log_it(L_CRITICAL,"Can't init stream server module");
        return -82;
    }

    if (dap_stream_ctl_init(DAP_ENC_KEY_TYPE_OAES, 32) != 0 ){
        log_it(L_CRITICAL,"Can't init stream control module");
        return -83;
    }

    if ( dap_http_simple_module_init() != 0 ) {
        log_it(L_CRITICAL,"Can't init http simple module");
        return -9;
    }

    if(dap_chain_node_cli_init(g_config)) {
            log_it(L_CRITICAL, "Can't init server for console");
        return -11;
    }

    if (sig_unix_handler_init(dap_config_get_item_str_default(g_config,
                                                              "resources",
                                                              "pid_path",
                                                              SYSTEM_PID_FILE_PATH)) != 0) {
        log_it(L_CRITICAL,"Can't init sig unix handler module");
        return -9;
    }

    save_process_pid_in_file(dap_config_get_item_str_default(g_config,
                                                             "resources",
                                                             "pid_path",
                                                             SYSTEM_PREFIX"/run/kelvin-node.pid"));

    log_it (L_DEBUG,"config server->enabled = \"%s\" ",dap_config_get_item_str_default(
                g_config,"server","enabled",false
                ) );
    if( dap_config_get_item_bool_default(g_config,"server","enabled",false) ) {
        int32_t l_port = dap_config_get_item_int32_default(g_config, "server", "listen_port_tcp",-1); // TODO Default listen port

        if( l_port > 0 ) {
            l_server = dap_server_listen((dap_config_get_item_str_default(g_config,
                                                                    "server",
                                                                    "listen_address",
                                                                    "0.0.0.0")),
                                   l_port,
                                   DAP_SERVER_TCP);
        }else
            log_it(L_WARNING, "Server is enabled but no port is defined");

    }

    if(l_server) { // If listener server is initialized
        bool is_traffick_track_enable = dap_config_get_item_bool_default(g_config, "traffic_track", "enable", false);

        if(is_traffick_track_enable) {
            time_t timeout = // TODO add default timeout (get_item_int32_default)
                    dap_config_get_item_int32(g_config, "traffic_track", "callback_timeout");

            dap_traffic_track_init(l_server, timeout);
           // dap_traffic_callback_set(db_auth_traffic_track_callback);
        }

        // TCP-specific things
        if ( dap_config_get_item_int32_default(g_config, "server", "listen_port_tcp",-1) > 0) {
            // Init HTTP-specific values
            dap_http_new(l_server,DAP_APP_NAME);

            // Handshake URL
            enc_http_add_proc(DAP_HTTP(l_server), ENC_HTTP_URL);

            // Streaming URLs
            dap_stream_add_proc_http(DAP_HTTP(l_server), STREAM_URL);
            dap_stream_ctl_add_proc(DAP_HTTP(l_server), STREAM_CTL_URL);

            const char *str_start_mempool = dap_config_get_item_str(g_config, "mempool", "accept");
            if(str_start_mempool && !strcmp(str_start_mempool, "true")) {
                dap_chain_mempool_add_proc(DAP_HTTP(l_server), MEMPOOL_URL);
            }

            // Built in WWW server

            if (  dap_config_get_item_bool_default(g_config,"www","enabled",false)  ){
                dap_http_folder_add(DAP_HTTP(l_server), "/",
                                dap_config_get_item_str(g_config,
                                                                "resources",
                                                                "www_root"));
            }

        }
    }else
        log_it(L_INFO, "No enabled server, working in client mode only");
    // VPN channel
    if (dap_config_get_item_bool_default(g_config,"vpn","enabled",false)){
        dap_stream_ch_vpn_init(dap_config_get_item_str_default(g_config, "vpn", "network_address", NULL),
                   dap_config_get_item_str_default(g_config, "vpn", "network_mask", NULL));

    }

    // Chain Network init

    dap_stream_ch_chain_init();
    dap_stream_ch_chain_net_init();
    dap_stream_ch_chain_net_srv_init();

    // New event loop init
    dap_events_init(0,0);
    dap_events_t *l_events = dap_events_new();
    dap_events_start (l_events);
    if (dap_config_get_item_bool_default(g_config,"vpn","enabled",false))
        dap_stream_ch_vpn_deinit();

    // Endless loop for server's requests processing
    rc = dap_server_loop(l_server);
    // After loop exit actions
    log_it(rc?L_CRITICAL:L_NOTICE,"Server loop stopped with return code %d",rc);

    // Deinit modules

    dap_stream_deinit();
    dap_stream_ctl_deinit();
    dap_http_folder_deinit();
    dap_http_deinit();
    dap_server_deinit();
    dap_enc_ks_deinit();
    dap_common_deinit();
    return rc*10;

}

void parse_args(int argc, const char * argv[]) {
    int opt, option_index = 0, is_daemon = 0;
    struct option long_options[] = {
        {"stop", 0, NULL, 0},
        {0, 0, NULL, 0} // mast be a last element
    };

    while ((opt = getopt_long(argc, (char *const *)argv, "D0",
                              long_options, &option_index)) != -1) {
        switch (opt) {
        case 0: { // --stop
            pid_t pid = get_pid_from_file(dap_config_get_item_str_default(g_config,
                                                                          "resources",
                                                                          "pid_path",
                                                                          SYSTEM_PID_FILE_PATH));
            if (pid == 0) {
                log_it(L_ERROR, "Can't read pid from file");
                exit(-20);
            } else if(kill_process(pid)) {
                log_it(L_INFO, "Server successfully stopped");
                exit(0);
            }
            log_it(L_WARNING, "Server not stopped. Maybe he is not running now?");
            exit(-21);
        }
        case 'D':
            log_it(L_INFO, "Daemonize server starting...");
            exit_if_server_already_running();
            is_daemon = 1;
            daemonize_process();
            break;
        default:
            log_it(L_WARNING, "Unknown option from command line");
        }
    }

    if(!is_daemon)
        exit_if_server_already_running();
}

void exit_if_server_already_running(void) {
    pid_t pid = get_pid_from_file(dap_config_get_item_str(g_config, "resources", "pid_path"));
    if (pid != 0 && is_process_running(pid)) {
        log_it(L_WARNING, "Proccess %d is running, don't allow "
                          "to run more than one copy of DapServer, exiting...", pid);
        exit(-2);
    }
}

