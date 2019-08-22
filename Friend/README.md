# ECEN 5823 Bluetooth Mesh Skeleton Project

This project contains skeleton code used for coursework in University of Colorado [ECEN 5823 IoT Embedded Firmware](https://sites.google.com/colorado.edu/ecen5823/home).

Below is an overview of the sequence used to generate this repository:
* The project was generated starting with the new project Wizard built into [Simplicity Studio 4](https://www.silabs.com/products/development-tools/software/simplicity-studio).  
* The AppBuilder project was used with application type "Bluetooth Mesh SDK" with version 2.11.2.0 and Bluetooth SDK 2.9.3.0, application stack "Bluetooth Mesh SDK 1.4.1.0"
* The SOC- BT Mesh Empty project application was used.
* Board and part were configured for BRD4104A Rev 00 and EFR32BG13P632F512GM48 respectively
* Configurations were setup to target GNU ARM 7.2.1.
* Simplicity project Workspace paths were setup to pull in emlib functions needed for display support, including middleware/glib directory and glib/glib glib/dmd directories
* Relevant emlib project files were copied from SiliconLabs\SimplicityStudio\v4\developer\sdks\gecko_sdk_suite\v2.5\platform as needed and added into the respective directories at the root.
* The main.c file in the root folder was renamed [gecko_main.c](gecko_main.c).  Contents of the main while loop were moved into functions and the main() function was #ifdef'd out.
* The [src](src) subfolder was added to contain code specific to the ECEN 5823 course and source files were added to support ECEN 5823 and the simplicity studio exercise assignment.

## Overall progress for project Update 1

* Project is based on the assignment 10 base repository.
* Friend node functionalities are up. Friend node is able to accept connection from two friend node. 
* Provisioning of the vendor models is in progress.
* Group project documents: https://drive.google.com/drive/u/1/folders/19pP2BnPFkOunKWceiNoMzz06ycwfbmVY
* Individual project documents(Nachiket): https://drive.google.com/drive/u/1/folders/1XefLhQ-oaW7LvgSsHN2dp4qmauuXOWxK

## Overall progress for project Update 2

* The two models are able to send the data to the friend and friend is successfully receiving it.
* Trying to implement the vendor models. The provisioner is unable to get the update that the device to be provisioned is getting the address.
* The work for out of band authentication is in progress.
* Added sensor.h(function description) and sensor.c which contains the functions for sensor interfacing.
* The sensor functionality is up and working.
* Group project documents: https://drive.google.com/drive/u/1/folders/19pP2BnPFkOunKWceiNoMzz06ycwfbmVY
* Individual project documents(Nachiket): https://drive.google.com/drive/u/1/folders/1XefLhQ-oaW7LvgSsHN2dp4qmauuXOWxK
* Provisioner code: https://github.com/CU-ECEN-5823/project-provisioner-NachiketKelkar


## Final Project Update
* The interrupt based motion sensor was implemented.
* The load power management is done by switcing off the sound detector sensor when not in use to reduce the power consumption.
* The no of rows supported by the LCD was increased to display the actuation messages.
* The actuation message is displayed based on the data and the threshold.
* Group project documents: https://drive.google.com/drive/u/1/folders/19pP2BnPFkOunKWceiNoMzz06ycwfbmVY
* Individual project documents(Nachiket): https://drive.google.com/drive/u/1/folders/1XefLhQ-oaW7LvgSsHN2dp4qmauuXOWxK
* Provisioner code: https://github.com/CU-ECEN-5823/project-provisioner-NachiketKelkar