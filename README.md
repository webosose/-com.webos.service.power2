Summary
-------
Provides the PMS functionality

Description
-----------

This component supports the following methods, which are described in detail in the generated documentation:
Methods:
--------
registerClient
setKeepAwake
clearKeepAwake
machineOff
machineReboot
initiate
shutdownApplicationsRegister
shutdownApplicationsAck
shutdownServicesRegister
shutdownServicesAck
notifyAlarmExpiry

Luna Signals:
------------
suspend
resume
shutdownApplications
shutdownServices

How to Build on Linux
---------------------

## Dependencies

Below are the tools and libraries (and their minimum versions) required to build power-manager-service:

* cmake (version required by openwebos/cmake-modules-webos)
* gcc 4.6.4
* glib-2.0 2.32.1
* make (any version)
* pkg-config 0.26
* openwebos/cmake-modules-webos 1.0.0~rc4
* openwebos/pmloglib 3.0.0
* openwebos/luna-service2 3.3.0
* nyx
* pms-support-wearable
* pms-suppor-api

## Building

Once you have downloaded the source, enter the following to build it (after
changing into the directory under which it was downloaded):

    $ mkdir BUILD
    $ cd BUILD
    $ cmake ..
    $ make
    $ sudo make install

The directory under which the files are installed defaults to `/usr/local/webos`.
You can install them elsewhere by supplying a value for `WEBOS_INSTALL_ROOT`
when invoking `cmake`. For example:

    $ cmake -D WEBOS_INSTALL_ROOT:PATH=$HOME/projects/webos ..
    $ make
    $ make install

will install the files in subdirectories of `$HOME/projects/webos`.

Specifying `WEBOS_INSTALL_ROOT` also causes `pkg-config` to look in that tree
first before searching the standard locations. You can specify additional
directories to be searched prior to this one by setting the `PKG_CONFIG_PATH`
environment variable.

If not specified, `WEBOS_INSTALL_ROOT` defaults to `/usr/local/webos`.

To configure for a debug build, enter:

    $ cmake -D CMAKE_BUILD_TYPE:STRING=Debug ..

To see a list of the make targets that `cmake` has generated, enter:

    $ make help

## Uninstalling

From the directory where you originally ran `make install`, enter:

 $ [sudo] make uninstall

You will need to use `sudo` if you did not specify `WEBOS_INSTALL_ROOT`.

Copyright and License Information
=================================
Unless otherwise specified, all content, including all source code files and
documentation files in this repository are:

Copyright (c) 2015 LG Electronics Inc. All Rights Reserved.
