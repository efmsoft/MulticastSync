# MulticastSync
Simple multicast sync application

# How to build
Pull it into Samples directory of Cinder. Open project in visual studio 2019 and run build. cinder.lib have to be previously built

# Test
Start one server instance by specifying the command line parameter --server
```
BasicApp.exe --server
```
Start any number of client instances starting BasicApp.exe without parameters. Clients can be started on the same computer or other computers in the local network. Port and multicast address values are hardcoded 
```
    constexpr static const uint16_t DESTINATION_PORT = 10001;
    constexpr static const uint16_t SERVER_PORT = 10000;
    constexpr static const char* MULTICAST_GROUP = "224.0.0.0";
```
An attempt to run more than one server instance, cause displaying an error message
