# General section
[general]
debug_mode={DEBUG_MODE}
debug_dump_stream_headers=false
# seed mode. WARNING. Used true only when you start the new network
#seed_mode=false
auto_online={AUTO_ONLINE}

# Chain networking
[chain_net]

# Timeout for network status wait 
status_wait_timeout=10

# Console

# Server part
[server]
#   By default you don't need to open you to the world
enabled={SERVER_ENABLED}
listen_address={SERVER_ADDR}
listen_port_tcp={SERVER_PORT}
news_url_enabled=false
bugreport_url_enabled=false

# Builtin DNS server
[dns_server]
enabled=false
bootstrap_balancer=false

[srv]
order_signed_only=false

[srv_dns]
enabled=false
#pricelist=[]

[stream]
preferred_encryption=IAES

# Mempool
[mempool]
# Automaticaly true if master node
#auto_proc=false

# Central Database
[cdb]
enabled=false
servers_list_enabled=false
servers_list_networks=[kelvin-testnet,private]

# Central Database authorization
[cdb_auth]
enabled=false
domain=mydomain
tx_cond_create=false
registration_open=true
# List of condition templates, created for authorized users. Format of condition:
# <wallet name>:<Value per transaction>:<Minimum time(seconds) between transactions>:<network name>
# tx_cond_templates=[mywallet0:0.00001:3600:KELT:kelvin-testnet,mywallet1:0.000001:3600:cETH:kelvin-testnet,mywallet0:1:10:WOOD:private]

# VPN stream channel processing module
[srv_vpn]
#   Turn to true if you want to share VPN service from you node
enabled=false
geoip_enabled=false

# Switch on debug_more to true if you want to see info about any packet passed through 
debug_more=false
#   List of loca security access groups. Built in: expats,admins,services,nobody,everybody
network_address=10.11.12.0
network_mask=255.255.255.0
pricelist=[kelvin-testnet:0.00001:KELT:3600:SEC:mywallet0,kelvin-testnet:0.00001:cETH:3600:SEC:mywallet1,private:1:WOOD:10:SEC:mywallet0]

# Console interface server
[conserver]
enabled=true
#listen_port_tcp=12345
listen_unix_socket_path=/opt/cellframe-node/var/run/node_cli
# Default permissions 770
#listen_unix_socket_permissions=770

# Application Resources
[resources]
#   0 means auto detect
threads_cnt=0
pid_path=/opt/cellframe-node/var/run/cellframe-node.pid
log_file=/opt/cellframe-node/var/log/cellframe-node.log
wallets_path=/opt/cellframe-node/var/lib/wallet
geoip_db_path=share/geoip/GeoLite2-City.mmdb
ca_folders=[/opt/cellframe-node/var/lib/ca,/opt/cellframe-node/share/ca]
dap_global_db_path=/opt/cellframe-node/var/lib/global_db
dap_global_db_driver=cdb

# Plugins
[plugins]
# Load Python plugins
py_load=false
# Plugins path
py_path=/opt/cellframe-node/var/plugins
