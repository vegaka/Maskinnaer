target remote :2331
set output-radix 16
set mem inaccessible-by-default off
#set remote memory-read-packet-size 1200
#set remote memory-read-packet-size fixed
mon speed 4000
mon endian little
mon reset 0
