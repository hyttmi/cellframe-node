# mileena net config

# General section
[general]
gdb_sync_nodes_addrs=[CCCC::0000::0000::0000,CCCC::0000::0000::0001,CCCC::0000::0000::0002]
id=0x000000000000cccc
name=mileena
native_ticker=tMIL
links_static_only=true
gdb_groups_prefix=mileena
# Possible values: light, full, archive, master, root
node-role={NODE_TYPE}
seed_nodes_aliases=[0.root.mileena,1.root.mileena,2.root.mileena]
seed_nodes_hostnames=[0.root.mileena.cellframe.net,1.root.mileena.cellframe.net,2.root.mileena.cellframe.net]
seed_nodes_addrs=[CCCC::0000::0000::0000,CCCC::0000::0000::0001,CCCC::0000::0000::0002]
seed_nodes_port=[8099,8099,8099]
require_links=2

private=false

#[auth]
#type=ca
#acl_accept_ca_list=[]
#acl_accept_ca_gdb=
#acl_accept_ca_chains=all


#[dag-poa]
#events-sign-cert=mileena.0.root

#[block-ton]
#blocks-sign-cert=mileena.0.master

#[block-poa]
#blocks-sign-cert=mycert
