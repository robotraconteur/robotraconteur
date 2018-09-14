#ifdef ROBOTRACONTEUR_USE_SCHANNEL
#define SECURITY_WIN32
#define CERT_CHAIN_PARA_HAS_EXTRA_FIELDS 
#endif

//Robot Raconteur
#include <RobotRaconteur.h>
#include "AsyncMessageReader.h"
#include "AsyncMessageWriter.h"
#include "Discovery_private.h"
#include "Subscription_private.h"
#include "HardwareTransport_private.h"
#include "LocalTransport_private.h"
#include "PipeMember_private.h"
#include "RobotRaconteurNode_connector_private.h"
#include "Service_lock_private.h"
#include "TcpTransport_private.h"
#include "WireMember_private.h"
#include <RobotRaconteur/StringTable.h>

//stdlib
#include <sstream>
#include <iomanip>
#include <wchar.h>
#include <fstream>
#include <set>
#include <queue>
#include <string>
#include <vector>

//boost
#include <boost/scope_exit.hpp>
#include <boost/foreach.hpp>
#include <boost/range.hpp>
#include <boost/range/algorithm.hpp>
#include <boost/range/adaptors.hpp>
#include <boost/range/combine.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/locale.hpp>
#include <boost/tuple/tuple_comparison.hpp>
#include <boost/interprocess/sync/file_lock.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/nil_generator.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/regex.hpp>
#include <boost/asio/steady_timer.hpp>
#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/bind/protect.hpp>


//platform specific
#ifdef ROBOTRACONTEUR_USE_SMALL_VECTOR
#include <boost/container/small_vector.hpp>
#endif

#ifdef ROBOTRACONTEUR_USE_SCHANNEL
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <wincrypt.h>
//#include <wintrust.h>
#include <schannel.h>
#include <security.h>
#include <sspi.h>
#endif

#ifdef ROBOTRACONTEUR_WINDOWS
#include <Shlobj.h>
#include <initguid.h>
#include <dbt.h>
#include <WinInet.h>
#include <IPHlpApi.h>
#else
#include <unistd.h>
#include <sys/file.h>
#include <sys/types.h>
#include <dirent.h>
#include <pwd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <netdb.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#endif

#ifdef ROBOTRACONTEUR_APPLE
#include <TargetConditionals.h>
#include <CoreFoundation.h>
#include <Security.h>
#endif

#pragma once