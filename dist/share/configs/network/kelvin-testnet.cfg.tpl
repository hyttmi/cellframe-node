# Kelvin Blockchain: development network
# General section
[general]
id=0x0000000000000001
name=kelvin-testnet
type=development
# Possible values: light, full, archive, master, root
node-role={NODE_TYPE}
#node-alias=addr-%node_addr%
#node-addr=1234::1234::1234::1234
gdb_groups_prefix=kelvin-testnet
# node addr exired time in hours (168h=1w 720h=1m 8760h=1y), by default 720h(1week)
#node-addr=0x10
node-addr-expired=168

seed_nodes_hostnames=[0.root.testnet.klvn.io,1.root.testnet.klvn.io,2.root.testnet.klvn.io,3.root.testnet.klvn.io,4.root.testnet.klvn.io]
seed_nodes_aliases=[kelvin.testnet.root.0,kelvin.testnet.root.1,kelvin.testnet.root.2,kelvin.testnet.root.3,kelvin.testnet.root.4]
seed_nodes_addrs=[ffff::0000::0000::0001,ffff::0000::0000::0002,ffff::0000::0000::0003,ffff::0000::0000::0004,ffff::0000::0000::0005]
seed_nodes_port=[8079,8079,8079,8079,8079]

#[role-master]
#proc_chains=[0x00000001]

#[dag-poa]
#events-sign-cert=mycert

#[dag-pos]
#events-sign-wallet=mywallet

