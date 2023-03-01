# Configuration
## Cellframe node configuration
Cellframe node configuration is done by editing `cellframe-node.cfg` file, which can be found in `/opt/cellframe-node/etc/`.

When installing Cellframe node for the first time with default values, the file should be ready to provide somewhat sane default configuration for your node. However, on some cases you might want or need to change some of the values (e.g. where wallets are stored by default).

### General configuration
```
[general]
...
debug_mode=false <-- Enable debug mode, prints debug messages in log files
...
auto_online=true <-- Bring up links and sync everything automatically

```

### Application resources
```
[resources]
...
log_file=/opt/cellframe-node/var/log/cellframe-node.log <-- Log file location
wallets_path=/opt/cellframe-node/var/lib/wallet <-- Where your created wallets will be located
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


