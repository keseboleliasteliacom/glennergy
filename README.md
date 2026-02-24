# glennergy
Hämtar spotpris och optimerar elförbrukning

## Installation
```bash
Run glennergy_install.sh script in the root directory
```
/usr/local/bin/ 		        #executeable
/var/log/glennergy/ 		    #logs
/etc/Glennergy-Fastigheter.json	#json-system
/tmp                            #FIFO
/dev/shm/glenn
tail -f *.log

## Usage

```bash
sudo Glennergy-Main [port] [log_level]
```

### Arguments 

- `port` (optional): Server port number (default: 8080)
- `log_level` (optional): Logging level (default: 1)
  - `0` - DEBUG: Detailed debug information
  - `1` - INFO: General information messages
  - `2` - WARNING: Warning messages
  - `3` - ERROR: Error messages only





