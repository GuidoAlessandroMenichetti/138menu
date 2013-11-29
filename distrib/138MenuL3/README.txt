29/11/2013

138MenuL3 for VHBL/CEF by GUIDOBOT (Follow @_GUIDOBOT)

Source Code: http://github.com/guidobot/138menu/
Webpage http://guidobot.hostoi.com/software/138menu.html

Features
	
	- Compatible with VHBL and CEF (ARK/TN)

	- Multi-language
		. English
		. Spanish
		. German
		. French
		. Turkish
		. Polish

	- Various settings
		. Loading/Showing games/installers icon
		. Listing games from a custom path and/or PSP/GAME/
		. Uses FBOOT.PBP, VBOOT.PBP, wmenu.bin, EBOOT.PBP (only for CEF) and/or a custom app name

	- Homebrew installing
	
	- Custom theme using

	- Custom wallpaper using

	- Delete App and Homebrew Folder format features included

	- Custom OSK

	- Easy-to-use interface

	- Protection measures included

	- Low sized menu


Controls
	While in main menu:

    	DPAD/Rtrigger/Ltrigger: Roll circle (Change category)
    	CROSS: Enter submenu

	While in sub-menu:

    	Rtrigger/Ltrigger: Roll circle (Change category)
    	UP/DOWN: Navigate
    	CROSS: Excecute/Install
    	TRIANGLE: Delete
    	CIRCLE: Go back

Menu installation
	Copy the files EBOOT.PBP (rename if needed) and DATA.138 to your exploit/menu folder and transfer to the device.

Homebrew installation
	There must be a savedata folder for every install package, following this format:
	ms0:/PSP/SAVEDATA/Name1/INSTALL.ZIP
	ms0:/PSP/SAVEDATA/Name2/INSTALL.ZIP
	ms0:/PSP/SAVEDATA/Name3/INSTALL.ZIP
	(Not necessary to call the folders 'Name')
	Installer file zip structure must be INSTALL.ZIP/Name/EBOOT.PBP compressed in STORE mode, invalid or compressed files will be skiped.

Themes installation
	Copy the theme .138 file to the menu folder, you can select it on the Settings menu

Wallpapers installation
	Copy your .PNG files to the menu folder. Wallpaper names should NOT begin with ICON, otherwise it will be skipped

Notes

    - About the homebrew not returning to the menu when exiting, that's a VHBL port compatibility issue. If 138Menu
    doesn't load anymore when exiting an app then most likely no menu will

    - There are no guarantees that the menu will work on all systems, I have no responsability on the possible damages
    produced by the wrong use of this application.

