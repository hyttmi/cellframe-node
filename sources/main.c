/*
 * Authors:
 * Dmitriy A. Gerasimov <kahovski@gmail.com>
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

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <getopt.h>
#include <signal.h>

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#include <mswsock.h>
#include <ws2tcpip.h>
#include <io.h>
#include <pthread.h>
#include "userenv.h"

#endif

#define LOG_TAG "main"

#ifndef _WIN32
  #include "sig_unix_handler.h"
#else
    #include "sig_win32_handler.h"
    #include "registry.h"
    void  S_SetExceptionFilter( void );
#endif
#include "dap_common.h"
#include "dap_config.h"
#include "dap_server.h"
#include "dap_http.h"
#include "dap_http_folder.h"
#include "dap_dns_server.h"
#include "dap_modules_dynamic_cdb.h"


#include "dap_events.h"
#include "dap_enc.h"
#include "dap_enc_ks.h"
#include "dap_enc_http.h"

#include "dap_chain.h"
#include "dap_chain_wallet.h"

#include "dap_chain_cs_dag.h"
#include "dap_chain_cs_dag_poa.h"
#include "dap_chain_cs_dag_pos.h"
#include "dap_chain_cs_none.h"

#include "dap_chain_net.h"
#include "dap_chain_net_srv.h"
#include "dap_chain_net_srv_app.h"
#include "dap_chain_net_srv_app_db.h"
#include "dap_chain_net_srv_datum.h"
#include "dap_chain_net_srv_geoip.h"

#ifdef DAP_OS_LINUX
#include "dap_chain_net_srv_vpn.h"
#include "dap_chain_net_srv_vpn_cdb.h"
#include "dap_chain_net_srv_vpn_cdb_server_list.h"
#include "dap_chain_net_vpn_client.h"
#endif

#include "dap_chain_global_db.h"
#include "dap_chain_mempool.h"
#include "dap_chain_node.h"
#include "dap_chain_node_cli.h"

#include "dap_stream_session.h"
#include "dap_stream.h"
#include "dap_stream_ctl.h"
#include "dap_stream_ch_chain.h"
#include "dap_stream_ch_chain_net.h"
#include "dap_stream_ch_chain_net_srv.h"
#include "dap_chain_net_srv_xchange.h"
#include "dap_chain_net_srv_stake.h"

#include "dap_common.h"
#include "dap_events_socket.h"
#include "dap_client.h"
#include "dap_http_client.h"
//#include "dap_http_client_simple.h"
#include "dap_http_simple.h"
#include "dap_process_manager.h"

#include "dap_defines.h"
#include "dap_file_utils.h"

#ifdef DAP_SUPPORT_PYTHON_PLUGINS
    #include "dap_chain_plugins.h"
#endif


#define ENC_HTTP_URL "/enc_init"
#define STREAM_CTL_URL "/stream_ctl"

#define STREAM_URL "/stream"
#define MEMPOOL_URL "/mempool"
#define MAIN_URL "/"

#ifdef __ANDROID__
    #include "cellframe_node.h"
#endif

void parse_args( int argc, const char **argv );
void exit_if_server_already_running( void );

static const char *s_pid_file_path = NULL;

bool dap_chain_net_srv_pay_verificator(dap_chain_tx_out_cond_t *a_cond, dap_chain_datum_tx_t *a_tx) { return true; }

#ifdef __ANDROID__
int cellframe_node_Main(int argc, const char **argv)
#else
int main( int argc, const char **argv )
#endif
{
	dap_server_t *l_server = NULL; // DAP Server instance
    bool l_debug_mode = true;
	bool bServerEnabled = false;
	int rc = 0;

    dap_set_appname("cellframe-node");
	#if defined(_WIN32) && defined(NDEBUG)
		S_SetExceptionFilter( );
	#endif

#ifdef _WIN32
    g_sys_dir_path = dap_strdup_printf("%s/%s", regGetUsrPath(), dap_get_appname());
#elif DAP_OS_MAC
    g_sys_dir_path = dap_strdup_printf("/Applications/%s.app/Contents/Resources", dap_get_appname());
#elif DAP_OS_ANDROID
    g_sys_dir_path = dap_strdup_printf("/storage/emulated/0/opt/%s",dap_get_appname());
#elif DAP_OS_UNIX
    g_sys_dir_path = dap_strdup_printf("/opt/%s", dap_get_appname());
#endif

    {
        char l_log_path[MAX_PATH] = {'\0'};
        int l_pos = dap_sprintf(l_log_path, "%s/var/log", g_sys_dir_path);
        dap_mkdir_with_parents(l_log_path);
        dap_sprintf(l_log_path + l_pos, "/%s.log", dap_get_appname());
        if (dap_common_init(dap_get_appname(), l_log_path) != 0) {
            printf("Fatal Error: Can't init common functions module");
            return -2;
        }
    }

    {
        char l_config_dir[MAX_PATH] = {'\0'};
        dap_sprintf(l_config_dir, "%s/etc", g_sys_dir_path);
        dap_config_init(l_config_dir);
    }

    if ((g_config = dap_config_open(dap_get_appname())) == NULL ) {
        log_it( L_CRITICAL,"Can't init general configurations" );
        return -1;
    }

    s_pid_file_path = dap_config_get_item_str_default( g_config,  "resources", "pid_path","/tmp") ;

    log_it(L_DEBUG, "Parsing command line args");
    parse_args( argc, argv );
    #ifdef _WIN32
        CreateMutexW( NULL, FALSE, (WCHAR *) L"DAP_CELLFRAME_NODE_74E9201D33F7F7F684D2FEF1982799A79B6BF94B568446A8D1DE947B00E3C75060F3FD5BF277592D02F77D7E50935E56" );
	#endif

      l_debug_mode = dap_config_get_item_bool_default( g_config,"general","debug_mode", false );
    //  bDebugMode = true;//dap_config_get_item_bool_default( g_config,"general","debug_mode", false );

    if ( l_debug_mode )
	    log_it( L_ATT, "*** DEBUG MODE ***" );
	else
 	   log_it( L_ATT, "*** NORMAL MODE ***" );

    dap_log_level_set( l_debug_mode ? L_DEBUG : L_NOTICE );

    log_it( L_DAP, "*** CellFrame Node version: %s ***", DAP_VERSION );

	// change to dap_config_get_item_int_default when it's will be possible
	size_t l_thread_cnt = 0;

	const char *s_thrd_cnt = dap_config_get_item_str( g_config, "resources", "threads_cnt" );
	if ( s_thrd_cnt != NULL )
	    l_thread_cnt = (size_t)atoi( s_thrd_cnt );

	if ( !l_thread_cnt ) {
    	#ifndef _WIN32
      		l_thread_cnt = (size_t)sysconf(_SC_NPROCESSORS_ONLN);
    	#else
      		SYSTEM_INFO si;
      		GetSystemInfo( &si );
      		l_thread_cnt = si.dwNumberOfProcessors;
    	#endif
  	}
    if ( dap_enc_init() != 0 ){
        log_it( L_CRITICAL, "Can't init encryption module" );
        return -56;
    }

    // New event loop init
    dap_events_init( 0, 0 );
    dap_events_t *l_events = dap_events_new( );
    dap_events_start( l_events );

    bServerEnabled = dap_config_get_item_bool_default( g_config, "server", "enabled", false );

    log_it ( L_DEBUG,"config server->enabled = \"%u\" ", bServerEnabled );

    if ( bServerEnabled && dap_server_init() != 0 ) {
        log_it( L_CRITICAL, "Can't init socket server module" );
        return -4;
    }


	if ( dap_http_init() != 0 ) {
    	log_it( L_CRITICAL, "Can't init http server module" );
	    return -5;
	}

	if ( dap_http_folder_init() != 0 ){
	    log_it( L_CRITICAL, "Can't init http server module" );
	    return -55;
	}
    if ( dap_http_simple_module_init() != 0 ) {
        log_it(L_CRITICAL,"Can't init http simple module");
        return -9;
    }

    if ( enc_http_init() != 0 ) {
        log_it( L_CRITICAL, "Can't init encryption http session storage module" );
        return -81;
    }

    if ( dap_stream_init(dap_config_get_item_bool_default(g_config,"general","debug_dump_stream_headers",false)) != 0 ) {
        log_it( L_CRITICAL, "Can't init stream server module" );
        return -82;
    }

    if ( dap_stream_ctl_init(DAP_ENC_KEY_TYPE_OAES, 32) != 0 ){
        log_it( L_CRITICAL, "Can't init stream control module" );
        return -83;
    }

    dap_client_init();


	if ( dap_chain_global_db_init(g_config) ) {
	    log_it( L_CRITICAL, "Can't init global db module" );
	    return -58;
	}



	//dap_http_client_simple_init( );

	if ( dap_datum_mempool_init() ) {
	    log_it( L_CRITICAL, "Can't init mempool module" );
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
        return -62;
    }

    if( dap_chain_cs_dag_poa_init() !=0){
        log_it(L_CRITICAL,"Can't init dap chain dag consensus PoA module");
        return -63;
    }

    if( dap_chain_cs_dag_pos_init() !=0){
        log_it(L_CRITICAL,"Can't init dap chain dag consensus PoA module");
        return -64;
    }

    if(dap_chain_gdb_init() != 0) {
        log_it(L_CRITICAL, "Can't init dap chain gdb module");
        return -71;
    }

    dap_chain_ledger_verificator_add(DAP_CHAIN_TX_OUT_COND_SUBTYPE_SRV_XCHANGE, dap_chain_net_srv_xchange_verificator);
    dap_chain_ledger_verificator_add(DAP_CHAIN_TX_OUT_COND_SUBTYPE_SRV_PAY, dap_chain_net_srv_pay_verificator);
    dap_chain_ledger_verificator_add(DAP_CHAIN_TX_OUT_COND_SUBTYPE_SRV_STAKE, dap_chain_net_srv_stake_verificator);

    if( dap_chain_net_init() !=0){
        log_it(L_CRITICAL,"Can't init dap chain network module");
        return -65;
    }

    if( dap_chain_net_srv_init(g_config) !=0){
        log_it(L_CRITICAL,"Can't init dap chain network service module");
        return -66;
    }

    if( dap_chain_net_srv_app_init() !=0){
        log_it(L_CRITICAL,"Can't init dap chain network service applications module");
        return -67;
    }

    if( dap_chain_net_srv_datum_init() !=0){
        log_it(L_CRITICAL,"Can't init dap chain network service datum module");
        return -68;
    }

#ifndef _WIN32
    // vpn server
    if(dap_config_get_item_bool_default(g_config, "srv_vpn", "enabled", false)) {
        if(dap_chain_net_srv_vpn_init(g_config) != 0) {
            log_it(L_ERROR, "Can't init dap chain network service vpn module");
            return -70;
        }
    }
    // vpn client
    if(dap_chain_net_vpn_client_init(g_config) != 0) {
        log_it(L_ERROR, "Can't init dap chain network service vpn client");
        return -72;
    }
#endif

    if(dap_config_get_item_bool_default(g_config, "srv_vpn", "geoip_enabled", false)) {
        if(chain_net_geoip_init(g_config) != 0) {
            log_it(L_CRITICAL, "Can't init geoip module");
            return -73;
        }
    }

	if ( dap_chain_node_cli_init(g_config) ) {
	    log_it( L_CRITICAL, "Can't init server for console" );
	    return -11;
	}

#ifndef _WIN32
    if (sig_unix_handler_init(dap_config_get_item_str_default(g_config,
                                                              "resources",
                                                              "pid_path",
                                                              "/tmp")) != 0) {
        log_it(L_CRITICAL,"Can't init sig unix handler module");
        return -12;
    }
#else
    if ( sig_win32_handler_init( NULL ) ) {
        log_it( L_CRITICAL,"Can't init sig win32 handler module" );
        return -12;
    }
#endif

    if (dap_chain_node_mempool_init()) {
        log_it( L_CRITICAL, "Can't init automatic mempool processing" );
        return -13;
    }

    save_process_pid_in_file(s_pid_file_path);

	if ( bServerEnabled ) {

        int32_t l_port = dap_config_get_item_int32(g_config, "server", "listen_port_tcp");

        if( l_port > 0 ) {
            l_server = dap_server_new(l_events,  (dap_config_get_item_str(g_config, "server", "listen_address")),
                                   (uint16_t) l_port,
                                   DAP_SERVER_TCP );
        } else
            log_it( L_WARNING, "Server is enabled but no port is defined" );

    }

    if ( l_server ) { // If listener server is initialized
        // TCP-specific things
		if ( dap_config_get_item_int32_default(g_config, "server", "listen_port_tcp",-1) > 0) {
            // Init HTTP-specific values
            dap_http_new( l_server, dap_get_appname() );

            if( dap_config_get_item_bool_default(g_config,"cdb","enabled",false) ) {
                if(dap_modules_dynamic_load_cdb(DAP_HTTP( l_server ))){
                    log_it(L_CRITICAL,"Can't init CDB module");
                    return -3;
                }else{
                    log_it(L_NOTICE, "Central DataBase (CDB) is initialized");
                }
            }

	        // Handshake URL
	        enc_http_add_proc( DAP_HTTP(l_server), ENC_HTTP_URL );

	        // Streaming URLs
	        dap_stream_add_proc_http( DAP_HTTP(l_server), STREAM_URL );
	        dap_stream_ctl_add_proc( DAP_HTTP(l_server), STREAM_CTL_URL );

	        const char *str_start_mempool = dap_config_get_item_str( g_config, "mempool", "accept" );
	        if ( str_start_mempool && !strcmp(str_start_mempool, "true")) {
	                dap_chain_mempool_add_proc(DAP_HTTP(l_server), MEMPOOL_URL);
	        }

	        // Built in WWW server

	        if (  dap_config_get_item_bool_default(g_config,"www","enabled",false)  ){
	                dap_http_folder_add( DAP_HTTP(l_server), "/",
	                                dap_config_get_item_str(g_config,
                                                                "resources",
                                                                "www_root") );
	        }

		}
    } else
        log_it( L_INFO, "No enabled server, working in client mode only" );


    // DNS server start
    bool bootstrap_balancer_enabled = dap_config_get_item_bool_default(g_config, "dns_server", "bootstrap_balancer", false);
    log_it(L_DEBUG, "config dns_server->bootstrap_balancer = \"%u\" ", bootstrap_balancer_enabled);
    if (bootstrap_balancer_enabled) {
        dap_dns_server_start();
    }

    // Chain Network init

	dap_stream_ch_chain_init( );
	dap_stream_ch_chain_net_init( );

    dap_stream_ch_chain_net_srv_init();

    if (!dap_chain_net_srv_xchange_init()) {
        log_it(L_ERROR, "Can't provide exchange capability");
    }
    if (!dap_chain_net_srv_stake_init()) {
        log_it(L_ERROR, "Can't start delegated stake service");
    }
///    if (dap_config_get_item_bool_default(g_config,"vpn","enabled",false))
///        dap_stream_ch_vpn_deinit();


    //dap_chain_net_load_all();



    //Init python plugins
    #ifdef DAP_SUPPORT_PYTHON_PLUGINS
        log_it(L_NOTICE, "Loading python plugins");
        dap_chain_plugins_init(g_config);
    #endif

    rc = dap_events_wait(l_events);
    log_it( rc ? L_CRITICAL : L_NOTICE, "Server loop stopped with return code %d", rc );
    // Deinit modules

//failure:

    #ifdef DAP_SUPPORT_PYTHON_PLUGINS
        dap_chain_plugins_deinit();
    #endif
    dap_dns_server_stop();
	dap_stream_deinit();
	dap_stream_ctl_deinit();
	dap_http_folder_deinit();
	dap_http_deinit();
	if (bServerEnabled) dap_server_deinit();
	dap_enc_ks_deinit();
    dap_chain_node_mempool_deinit();
    dap_chain_net_srv_xchange_deinit();
    dap_chain_net_srv_stake_deinit();

	dap_config_close( g_config );
	dap_common_deinit();

	return rc * 10;
}

static struct option long_options[] = {

	{ "stop", 0, NULL, 0 },
	{ NULL,   0, NULL, 0 } // must be a last element
};

void parse_args( int argc, const char **argv ) {

	int opt, option_index = 0, is_daemon = 0;

	while ( (opt = getopt_long(argc, (char *const *)argv, "D0",
                              long_options, &option_index)) != -1) {
	    switch ( opt ) {

	    case 0: // --stop
    	{
            pid_t pid = get_pid_from_file(s_pid_file_path);

	    	if ( pid == 0 ) {
	        	log_it( L_ERROR, "Can't read pid from file" );
	        	exit( -20 );
	      	}

	    	if ( kill_process(pid) ) {
	        	log_it( L_INFO, "Server successfully stopped" );
	        	exit( 0 );
	      	}

	    	log_it( L_WARNING, "Server not stopped. Maybe he is not running now?" );
	    	exit( -21 );
	    }

    	case 'D':
    	{
        	log_it( L_INFO, "Daemonize server starting..." );
        	exit_if_server_already_running( );
        	is_daemon = 1;
        	daemonize_process( );
        	break;
      	}

        default:
        log_it( L_WARNING, "Unknown option from command line" );
		}
	}

	if( !is_daemon )
		exit_if_server_already_running( );
}

void exit_if_server_already_running( void ) {

    pid_t pid = get_pid_from_file(s_pid_file_path);

	bool  mf = false;

	#ifdef _WIN32
        CreateMutexW( NULL, FALSE, (WCHAR *) L"DAP_CELLFRAME_NODE_74E9201D33F7F7F684D2FEF1982799A79B6BF94B568446A8D1DE947B00E3C75060F3FD5BF277592D02F77D7E50935E56" );

		if ( GetLastError( ) == 183 ) {
      		mf = true;
    	}
	#endif

	if ( (pid != 0 && is_process_running(pid)) || mf ) {
    	log_it( L_WARNING, "Proccess %d is running, don't allow "
        	                "to run more than one copy of DapServer, exiting...", pid );
		exit( -2 );
	}
}

