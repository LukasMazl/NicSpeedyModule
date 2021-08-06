#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/if.h>

#define INET6_ADDRSTRLEN 46

#define IFA_NAME "name"
#define IFA_FAMILY "family"
#define IFA_IS_UP "up"
#define IFA_FLAGS "flags"
#define IFA_ADDR "addr"
#define IFA_ADDR_NETMASK "addr_netmask"

static char* to_ipv6(struct sockaddr_in6 *sa) {
    char *str = malloc (sizeof (char) * INET6_ADDRSTRLEN);
    struct in6_addr in6Addr = sa->sin6_addr;
    inet_ntop(AF_INET6, &in6Addr, str, INET6_ADDRSTRLEN);
    return str;
}

static char* to_ipv4(struct sockaddr_in *sa) {
    if(sa == NULL)
    {
        return NULL;
    }
    struct in_addr inAddr = sa->sin_addr;
    return inet_ntoa(inAddr);
}

static inline char* get_formated_addr(struct sockaddr* ifa_addr) {
   int family = ifa_addr->sa_family;
   switch(family) {
        case AF_INET6:
            return to_ipv6((struct sockaddr_in6 *) ifa_addr);
        case AF_INET:
            return to_ipv4((struct sockaddr_in *) ifa_addr);
        default:
            return NULL;
   } 
}

static PyObject* get_interface_info(struct ifaddrs *ifa) {
    if(ifa == NULL) {
        return NULL;
    }

    int ifa_flags = ifa->ifa_flags;
    int is_inteface_up = (ifa_flags & IFF_UP)? 1: 0;
    char* addr = NULL;
    char* addr_netmask = NULL;
    int sa_family = -1;

    if(ifa->ifa_addr != NULL) {
        struct sockaddr* ifa_addr = ifa->ifa_addr;
        sa_family = ifa_addr->sa_family;
        addr = get_formated_addr(ifa_addr);
    }

    if(ifa->ifa_netmask != NULL) 
    {
        addr_netmask = get_formated_addr(ifa->ifa_netmask);
    }

    PyObject* results_dict = PyDict_New();
    
    //Sets ifa name
    PyObject* ifa_name_key = PyUnicode_FromString(IFA_NAME);
    PyObject* ifa_name_value = PyUnicode_FromString(ifa->ifa_name);
    PyDict_SetItem(results_dict, ifa_name_key, ifa_name_value);

    //Sets if family
    PyObject* ifa_family_key = PyUnicode_FromString(IFA_FAMILY);
    PyObject* ifa_family_value = PyLong_FromLong(sa_family);
    PyDict_SetItem(results_dict, ifa_family_key, ifa_family_value);

    //Sets is interface up
    PyObject* ifa_isup_key = PyUnicode_FromString(IFA_IS_UP);
    if(is_inteface_up)
    {
        PyDict_SetItem(results_dict, ifa_isup_key, Py_True);
    } 
    else 
    {
        PyDict_SetItem(results_dict, ifa_isup_key, Py_False);
    }
    
    //Sets flags
    PyObject* ifa_flags_key = PyUnicode_FromString(IFA_FLAGS);
    PyObject* ifa_flags_value = PyLong_FromLong(ifa->ifa_flags);
    PyDict_SetItem(results_dict, ifa_flags_key, ifa_flags_value);

    //Sets IP of interface if exists
    PyObject* ifa_ip_key = PyUnicode_FromString(IFA_ADDR);
    PyObject* ifa_ip_value;
    if(addr == NULL) 
    {
        ifa_ip_value = Py_None;
    } 
    else
    {
        ifa_ip_value = PyUnicode_FromString(addr);
    }
    PyDict_SetItem(results_dict, ifa_ip_key, ifa_ip_value);


    //Sets NETMASK of interface if exists
    PyObject* ifa_ip_netmask_key = PyUnicode_FromString(IFA_ADDR_NETMASK);
    PyObject* ifa_ip_netmask_value;
    if(addr == NULL) 
    {
        ifa_ip_netmask_value = Py_None;
    } 
    else
    {
        ifa_ip_netmask_value = PyUnicode_FromString(addr_netmask);
    }
    PyDict_SetItem(results_dict, ifa_ip_netmask_key, ifa_ip_netmask_value);
    
    return results_dict;
}

static PyObject* get_all_if(PyObject* self) 
{
    struct ifaddrs *ifap, *ifa;
    getifaddrs(&ifap);
    PyObject *list = PyList_New(0);
    for (ifa = ifap; ifa; ifa = ifa->ifa_next)
    {
        PyObject* item = get_interface_info(ifa);
        if(item != NULL) 
        {
            PyList_Append(list, item);
        }
    }
    freeifaddrs(ifap);
    return list;
}

static PyObject* get_them_all(void) 
{
    struct ifaddrs *ifap, *ifa;
    getifaddrs(&ifap);
    PyObject *dict = PyDict_New();
    for (ifa = ifap; ifa; ifa = ifa->ifa_next) 
    {
        PyObject* item = get_interface_info(ifa);
        if(!item) 
            continue;
        
        char *ifname = ifa->ifa_name;
        PyObject* pyIfname = PyUnicode_FromString(ifname);
        PyObject* if_results;
        if(PyDict_Contains(dict, pyIfname)) 
        {
            if_results = PyDict_GetItem(dict, pyIfname);
        } 
        else 
        {
            if_results = PyList_New(0);
            PyDict_SetItem(dict, pyIfname, if_results);
        }
        
        PyList_Append(if_results, item);
        Py_XDECREF(pyIfname); 
    }
    freeifaddrs(ifap);
    return dict;
}

static inline int is_if_up(struct ifaddrs *ifa)
{
    if(ifa == NULL)
        return 0;

    return ifa->ifa_flags & IFF_UP;
}

static PyObject* get_all_if_by_AF_int(int AF)
{
    struct ifaddrs *ifap, *ifa;
    getifaddrs(&ifap);

    PyObject *list = PyList_New(0);
    for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
        struct sockaddr* ifa_addr = ifa->ifa_addr;
        if(ifa_addr != NULL) {
            int sa_family = ifa_addr->sa_family;
            if (sa_family == AF) {
                PyObject* item = Py_BuildValue("s", ifa->ifa_name);
                PyList_Append(list, item);
            }
        }
    }
    freeifaddrs(ifap);
    return list;
}

static PyObject* get_info_if_by_AF_int(int AF)
{
    struct ifaddrs *ifap, *ifa;
    getifaddrs(&ifap);

    PyObject *list = PyList_New(0);
    for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
        struct sockaddr* ifa_addr = ifa->ifa_addr;
        if(ifa_addr != NULL) {
            if (ifa_addr->sa_family == AF) {
                PyObject* item = get_interface_info(ifa);
                PyList_Append(list, item);
            }
        }
    }
    freeifaddrs(ifap);
    return list;
}

static PyObject* get_ip_inet(PyObject *self, PyObject *args)
{
    char* name;
    if (!PyArg_ParseTuple(args, "s", &name)) {
        return NULL;
    }

    struct ifaddrs *ifap, *ifa;
    getifaddrs(&ifap);
    for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
        struct sockaddr* ifa_addr = ifa->ifa_addr;
        if(ifa_addr != NULL) {
            if (ifa_addr->sa_family == AF_INET && strcmp(name, ifa->ifa_name) == 0) {
                PyObject* ip = Py_BuildValue("s", to_ipv4((struct sockaddr_in *) ifa_addr));
                freeifaddrs(ifap);
                return ip;
            }
        }
    }
    freeifaddrs(ifap);
    return Py_None;
}

static PyObject* get_all_if_by_AF(PyObject *self, PyObject *args)
{
    int a;
    if (!PyArg_ParseTuple(args, "i", &a)) {
        return NULL;
    }
    return get_all_if_by_AF_int(a);
}

static PyObject* get_info_if_by_AF(PyObject *self, PyObject *args)
{
    int a;
    if (!PyArg_ParseTuple(args, "i", &a)) {
        return NULL;
    }
    return get_info_if_by_AF_int(a);
}

static PyObject* get_list_ipv4_if(void)
{
    return get_all_if_by_AF_int(AF_INET);
}

static PyObject* get_list_ipv6_if(void)
{
    return get_all_if_by_AF_int(AF_INET6);
}

static PyObject* get_info_about_active_interfaces(void)
{
    struct ifaddrs *ifap, *ifa;
    getifaddrs(&ifap);

    PyObject *list = PyList_New(0);
    for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
        struct sockaddr* ifa_addr = ifa->ifa_addr;
        if(ifa_addr != NULL) {
            if (ifa_addr->sa_family == AF_INET && is_if_up(ifa)) {
                PyObject* item = get_interface_info(ifa);
                PyList_Append(list, item);
            }
        }
    }
    freeifaddrs(ifap);
    return list;
}

static PyObject* get_active_interfaces(void)
{
    struct ifaddrs *ifap, *ifa;
    getifaddrs(&ifap);

    PyObject *list = PyList_New(0);
    for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
        struct sockaddr* ifa_addr = ifa->ifa_addr;
        if(ifa_addr != NULL) {
            if (ifa_addr->sa_family == AF_INET && is_if_up(ifa)) {
                PyObject* item = Py_BuildValue("s", ifa->ifa_name);
                PyList_Append(list, item);
            }
        }
    }
    freeifaddrs(ifap);
    return list;
}

static PyMethodDef module_methods[] = {
        {"get_them_all",                        (PyCFunction) get_them_all,                  METH_NOARGS,  "Function get_them_all() returns information about all network interfaces as dict. Key is name of NIC, value is info.\n"},
        {"get_all_system_if",                   (PyCFunction) get_all_if,                    METH_NOARGS,  "Returns info about all network interfaces as list."},
        {"get_all_ipv4_if",                     (PyCFunction) get_list_ipv4_if,              METH_NOARGS,  "Returns info about network interfaces which support protocol IPv4."},
        {"get_all_ipv6_if",                     (PyCFunction) get_list_ipv6_if,              METH_NOARGS,  "Returns info about network interfaces which support protocol IPv6."},
        {"get_all_if_by_AF",                    (PyCFunction) get_all_if_by_AF,              METH_VARARGS, "Returns list of names network interfaces which has given."},
        {"get_info_if_by_AF",                   (PyCFunction) get_info_if_by_AF,             METH_VARARGS, "Returns info about network interfaces which has given."},
        {"get_active_interfaces",               (PyCFunction) get_active_interfaces,            METH_NOARGS,  "This function returns active devices, which are up and with IPv4 protocol family."},
        {"get_info_about_active_interfaces",    (PyCFunction) get_info_about_active_interfaces, METH_NOARGS,  "This function returns information only about up devices."},
        {"get_ip_inet",                         (PyCFunction) get_ip_inet,                   METH_VARARGS, "Returns IP address of interface which has type IPv4"},
        {NULL}
};

static struct PyModuleDef SpeedyNicModuleDef =
{
        PyModuleDef_HEAD_INIT,
        "nicSpeedyModule",
        NULL,
        -1,
        module_methods
};

static inline void _init_constants_for_module(PyObject* module)
{
    #ifdef IFF_UP
         PyModule_AddIntConstant(module, "IFF_UP",  IFF_UP);
    #endif

    #ifdef IFF_BROADCAST
         PyModule_AddIntConstant(module, "IFF_BROADCAST",  IFF_BROADCAST);
    #endif

    #ifdef IFF_DEBUG
         PyModule_AddIntConstant(module, "IFF_DEBUG",  IFF_DEBUG);
    #endif

    #ifdef IFF_LOOPBACK
         PyModule_AddIntConstant(module, "IFF_LOOPBACK",  IFF_LOOPBACK);
    #endif
    
    #ifdef IFF_POINTOPOINT
         PyModule_AddIntConstant(module, "IFF_POINTOPOINT",  IFF_POINTOPOINT);
    #endif
    
    #ifdef IFF_RUNNING
         PyModule_AddIntConstant(module, "IFF_RUNNING",  IFF_RUNNING);
    #endif
    
    #ifdef IFF_NOARP
         PyModule_AddIntConstant(module, "IFF_NOARP",  IFF_NOARP);
    #endif

    #ifdef IFF_PROMISC
         PyModule_AddIntConstant(module, "IFF_PROMISC",  IFF_PROMISC);
    #endif

    #ifdef IFF_ALLMULTI
         PyModule_AddIntConstant(module, "IFF_ALLMULTI",  IFF_ALLMULTI);
    #endif

    #ifdef IFF_MASTER
         PyModule_AddIntConstant(module, "IFF_MASTER",  IFF_MASTER);
    #endif

    #ifdef IFF_SLAVE
         PyModule_AddIntConstant(module, "IFF_SLAVE",  IFF_SLAVE);
    #endif

    #ifdef IFF_MULTICAST
         PyModule_AddIntConstant(module, "IFF_MULTICAST",  IFF_MULTICAST);
    #endif

    #ifdef IFF_PORTSEL
         PyModule_AddIntConstant(module, "IFF_PORTSEL",  IFF_PORTSEL);
    #endif

    #ifdef IFF_AUTOMEDIA
         PyModule_AddIntConstant(module, "IFF_AUTOMEDIA",  IFF_AUTOMEDIA);
    #endif

    #ifdef IFF_DYNAMIC
         PyModule_AddIntConstant(module, "IFF_DYNAMIC",  IFF_DYNAMIC);
    #endif

    #ifdef IFF_LOWER_UP
         PyModule_AddIntConstant(module, "IFF_LOWER_UP",  IFF_LOWER_UP);
    #endif

    #ifdef IFF_DORMANT
         PyModule_AddIntConstant(module, "IFF_DORMANT",  IFF_DORMANT);
    #endif

    #ifdef IFF_ECHO
         PyModule_AddIntConstant(module, "IFF_ECHO",  IFF_ECHO);
    #endif
}

PyMODINIT_FUNC PyInit_nicSpeedyModule(void)
{
    PyObject* module = PyModule_Create(&SpeedyNicModuleDef);
    _init_constants_for_module(module);

    return module;
}
