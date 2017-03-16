ethercat-demo (EtherCAT Server API Prototype)
=============
##1 Description
The EtherCAT Server API exposes a simple interface that users can build EtherCAT based applications in C++ on Linux quickly and easily, without requiring the user to program against an EtherCAT protocol specific API.  Instead, the API enables device reads and writes using the device name or ring position or alias address and a parameter name.

The API includes methods to readout the current device and parameter names defined by the connected hardware, and recognizes these names in the API for reading and writing values.

##2 Quick Start Instructions
1.	Install the IgH EtherLab Master.
2.	Download this distribution from GitHub. 
3.	Build using the Makefile in src dir.
4.	Copy the generated shared library: ‘libtmtec.so’ to a location in the library path.
5.	Configure server to connected hardware (see section 3.2)
6.	Run the ectest program.

##3 Writing a Program
Link against libtmtec.so, and include EtherCatServer.h interface definition in your source files.
###3.1 Example Program
The example program UserExample.c is included in the distribution.  This demonstrates the capabilities of the API.
###3.2 Configuring Server to Connected Hardware
Run “ethercat xml > <yourconfigfilename>.xml” to generate the configuration xml file.

Identify location of file when configuring the server in code:
```
etherCatServer.configServer("<locationofmyconfigfile>.xml");    
```
##4 Alias Addressing
Device modules can be addressed using names, ring position and/or alias addressing.  Name addressing is not currently working when two modules in the ring are the same manufacture and model.

Alias addressing can be used to access a module even if the module is moved from one position in the ring to another.   For this scheme to work we need to:
1.	Write EPROM on the module with the alias address
2.	Run the XML generator and update the XML config file each time a module is moved.  (This step will be automated in a future release).

###4.1 Wrting EPROM Alias to Modules
To write EPROM alias addressing, use the following ethercat command line:
```
ethercat alias –p <pos> -a <alias>
```
Where <pos> is the current ring position of the module to change the alias and <alias> is an unsigned 16 bit number.

To use alias addressing in the API, use the form of the get/set parameter value method that takes deviceAlias and devicePosition as arguments, for example:
```
getParameterValue(1111, 0, “Channel 1::Input”);
``` 
In the example, the alias is 1111 and the position is ignored.   If the alias is 0, then the ring position argument is used instead.

##Design Overview

![Design](/doc/images/design.png)

A user program provides the location of the configuration XML file.  This is passed to the EtherCatServer API, that in turn passes it to the ConfigLoader that loads the configuration, creating an internal data structure of SlaveConfig objects, that contain associated SyncManager objects, Pdo objects and PdoEntry objects.

Starting the server calls the start() method of the CyclicMotor that initializes the state of the object and starts the detached thread that wakes up and executes at regular intervals to receive and send datagrams.

Every time the CyclicMotor thread wakes up and updates its Domain process data, that data is copied to the PdoEntryCache singleton cache.  Whenever a call is made to getParameterValue in the EtherCatServer API, this call in turn calls the getPdoEntryValue() call on the singleton cache.  The value in the cache is always the most recently read value from the domain.

Similarly, the EtherCatServer API call: setParameterValue() creates and populates PdoEntryValue structs and passes these to the CommandQueue singleton’s addToQueue() method.  The CommandQueue contains accumulated commands.  These commands are popped off the queue by the CyclicMotor thread each time it wakes up and finds entries on the queue.  All entries are sent out with the next datagram and the queue is emptied.

Testing autobuild. Please ignore.
