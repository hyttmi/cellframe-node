# Configuration
## Cellframe node configuration

Cellframe node configuration is done by editing `cellframe-node.cfg` file, which can be found in `/opt/cellframe-node/etc/` for Linux systems. For MacOS, the file is available in `/Users/$USER/Applications/CellFrame.app/contents/resources/etc/cellframe-node.cfg`

When installing Cellframe node for the first time with default values, the file should provide somewhat sane default configuration for your node. However, on some cases you might want or need to change some of the values.

### General configuration
```
[general]
...
debug_mode=false <-- If set to true, prints debug information on log files
...
auto_online=true <-- Bring up links and sync everything automatically

```

### Application resources
```
[resources]
...
log_file=/opt/cellframe-node/var/log/cellframe-node.log <-- Log file location
wallets_path=/opt/cellframe-node/var/lib/wallet <-- Location of created wallets
dap_global_db_path=/opt/cellframe-node/var/lib/global_db <-- GlobalDB path
...
```

### Python plugins
```
[plugins]
...
py_load=false <-- Set to true for enabling Python plugins support
...
py_path=/opt/cellframe-node/var/lib/plugins <-- Path to Python plugins
```

## Configuration for networks
Each network has its own settings file placed in `/opt/cellframe-node/etc/network/`. On Linux systems:

```
/opt/cellframe-node/etc/network/
                                ├── Backbone.cfg
                                ├── kelvpn-minkowski.cfg
                                ├── mileena.cfg
                                └── subzero.cfg
```
By default, each network role is configured as `full` role.