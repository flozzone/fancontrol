define target remote
  target extended-remote $arg0
file /home/floz/projects/fancontrol/fancontrol/cmake-build-debug/fancontrol.elf
monitor swdp_scan
attach 1
set mem inaccessible-by-default off
load
end



