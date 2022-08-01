# Kelvpn-minkowski testnet 2

# General section
[general]
id=0x000000000000aaaa
name=kelvpn-minkowski
# Possible values: light, full, archive, master, root
node-role={NODE_TYPE}
gdb_groups_prefix=minkowski
node_addr_type=static
node-addr=AAAA::0000::0000::0000
seed_nodes_aliases=[minkowski.kelvpn.root.0,minkowski.kelvpn.root.1,minkowski.kelvpn.root.2]
seed_nodes_hostnames=[0.root.minkowski.kelvpn.com,1.root.minkowski.kelvpn.com,2.root.minkowski.kelvpn.com]
seed_nodes_addrs=[AAAA::0000::0000::0000,AAAA::0000::0000::0001,AAAA::0000::0000::0002]
seed_nodes_port=[8099,8099,8099]

private=false

#[auth]
#type=ca
#acl_accept_ca_list=[]
#acl_accept_ca_gdb=
#acl_accept_ca_chains=all


#[dag-poa]
#events-sign-cert=kelvpn-minkowski.root.0

#[block-ton]
#blocks-sign-cert=minkowski.master.pvt.0


