# HID Bootflash

After connecting the Megacard to USB the LED7 should blink. The blinking LED7 indicates that the boot loader on Megacard has started and waits for instructions. If the LED7 isn´t blinking the HID boot loader can´t load the hex dump onto Megacard. To exit the boot loader press **reset (S4)** button and the previous loaded program will be started. To restart the boot loader it is necessary to hold down **S3** and press reset.

* [Install HID boot loader](#Install-HID-boot-loader)
* [Flash Megacard manually](#Flash-Megacard-manually)
* [Flash Megacard with Atmel Studio](#Flash-Megacard-with-Atmel-Studio)
* [Possible errors of HID boot loader](#Possible-errors-of-HID-boot-loader)

---

## Install HID boot loader

For setup it is necessary to download the [HID boot loader](http://vusb.wikidot.com/local--files/project:hidbootflash/HIDBootFlash.zip) from V-USB. Move the boot loader somewhere to

```
C:\Tools\Downloader
```

![Path](/docs/image/hid_path.png "Path to HID boot loader")

---

## Flash Megacard manually

Start HID boot loader manually and try to load a hex dump onto the Megacard. It is necessary that the Megacard is connected to the computer over USB and that the boot loader on Megacard started (blinking LED7).

![Find Device](/docs/image/hid_find.png "Find device (Megacard)")

After the device has been found **HIDBoot detected** should be displayed and the **Open .hex File** button occurs. There is a demo hex dump ([blink.hex](/blink.hex)) that can be loaded.

![Open File](/docs/image/hid_open.png "Open hex dump")

The hex dump should be displayed below the **Open .hex File** button.

![Loaded File](/docs/image/hid_read.png "Loaded hex dump")

Now the Megacard can be flashed with the blink.hex! If the device should be rebooted after flashing it is necessary to select the **Reboot AVR** checkbox.

![Flash Megacard](/docs/image/hid_flash.png "Flash hex dump onto Megacard")

If everything worked the LEDs of the Megacard should blink with 1/2 Hz.

---

## Flash Megacard with Atmel Studio

To include HID boot loader into Atmel Studio it is necessary to add the boot loader to **External Tools...**.

![External Tools](/docs/image/as_external_tools.png "Atmel Studio external tools")

Add a new tool with following parameters:

```
# Title
Megacard

# Command
C:\Tools\Downloader\HIDBootFlash.exe

# Arguments
-r $(TargetDir)$(TargetName).hex
```

![HID boot loader](/docs/image/as_external_tools_megacard.png "Add HID boot loader to external tools")

After that the **main.c** of a project from the example (or any other) solution can be selected and built.

![Project build](/docs/image/as_build_project.png "Build an Atmel Studio project")

After build the hex dump can be loaded onto the Megacard with the configured External Tool **Megacard**. It is necessary that the Megacard is connected to the computer over USB and that the boot loader on Megacard started (blinking LED7).

![Load boot loader](/docs/image/as_megacard.png "Load hex dump with Atmel Studio external tool")

If everything worked the project should be loaded to the hardware.

---

## Possible errors of HID boot loader

The most appearing problem is that the boot loader on Megacard isn´t started (Blinking LED7). Sometimes there are some connection errors. Mostly if timers or ports are used nearby the USB port. So the HID boot loader can´t be connected to Megacard.

![Boot loader error](/docs/image/hid_error.png "Error due connection problems")

**Solution**

Reconnect the Megacard to the USB port (blinking PC7)!

---

[Back](/README.md)