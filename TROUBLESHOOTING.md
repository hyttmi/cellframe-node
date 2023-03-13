# Troubleshooting / FAQ

## Where are the log files located?

On Linux systems:
```
/opt/cellframe-node/var/log/
```
On MacOS:
```
/Users/$USER/Applications/CellFrame.app/contents/resources/var/log/
```

## My log files are getting really large, how to prevent it?

Cellframe node uses `logrotate` on Linux to rotate log files daily if log file size exceeds 1GB. If your log files are not rotated correctly, you should check if you have `logrotate` installed and have the proper configuration file available in `/etc/logrotate.d/` directory.

## How to remove Cellframe node?

**NOTE: Be careful, take a backup of your created wallets from `/opt/cellframe-node/var/lib/wallet/` before proceeding!**

On Debian and it's derivatives, you can use `apt remove cellframe-node` and delete `/opt/cellframe-node` directory after removal.

On other Linux systems:
```
sudo systemctl stop cellframe-node.service
sudo systemctl disable cellframe-node.service
sudo unlink /etc/logrotate.d/cellframe-node
sudo unlink /etc/profile.d/cellframe-node.sh
sudo rm -rf /opt/cellframe-node
```

## Running and debugging in Qt (Linux Mint)

During debugging, the configuration files located in /opt/cellframe-node/ are used. Therefore, it is necessary to grant access rights to this directory to the user who will do the debugging.

```
sudo chown <username> /opt/cellframe-node/
```
