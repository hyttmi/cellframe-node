# Subzero testnet

# General section
[general]
id=0x000000000000acca
name=subzero
native_ticker=tCELL

# Possible values: light, full, archive, master, root
node-role={NODE_TYPE}
seed_nodes_aliases=[subzero.cellframe.root.0,subzero.cellframe.root.1,subzero.cellframe.root.2,subzero.cellframe.root.3,subzero.cellframe.root.4]
seed_nodes_hostnames=[0.root.subzero.cellframe.net,1.root.subzero.cellframe.net,2.root.subzero.cellframe.net,3.root.subzero.cellframe.net,4.root.subzero.cellframe.net]
seed_nodes_addrs=[ACCA::0000::0000::0001,ACCA::0000::0000::0002,ACCA::0000::0000::0003,ACCA::0000::0000::0004,ACCA::0000::0000::0005]
seed_nodes_port=[8099,8099,8099,8099,8099]

#[auth]
#type=ca
#acl_accept_ca_list=[]
#acl_accept_ca_gdb=
#acl_accept_ca_chains=all


#[dag-poa]
#events-sign-cert=mycert

#[block-poa]
#blocks-sign-cert=mycert
