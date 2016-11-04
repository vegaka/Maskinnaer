.PHONY : driver
driver :
	-cp -r /media/sf_share/ex3/ /home/tdt4258/
	-ptxdist clean driver-gamepad
	-ptxdist compile driver-gamepad


.PHONY : upload-driver
upload-driver :
	-ptxdist targetinstall driver-gamepad
	-ptxdist targetinstall kernel
	-ptxdist image root.romfs
	-ptxdist test flash-rootfs

.PHONY : game
game :
	-cp -r /media/sf_share/ex3/ /home/tdt4258/
	-ptxdist clean game
	-ptxdist compile game

.PHONY : upload-game
upload-game :
	-ptxdist targetinstall game
	-ptxdist image root.romfs
	-ptxdist test flash-rootfs