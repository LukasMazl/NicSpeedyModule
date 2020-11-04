#include <stdio.h>

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/wireless.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>

#define INET6_ADDRSTRLEN 46

#define IFA_NAME "name"
#define IFA_FAMILY "family"
#define IF_IS_UP "up"
#define IF_ADDR "addr"

static char* if_addr_family_to_string(int family) {
    return  ((family == AF_PACKET) ? "AF_PACKET" :
            (family == AF_INET) ? "AF_INET" :
            (family == AF_INET6) ? "AF_INET6" : "???");
}

static char* to_ipv6(struct sockaddr_in6 *sa) {
    char *str = malloc (sizeof (char) * INET6_ADDRSTRLEN);
    inet_ntop(AF_INET6, &sa->sin6_addr, str, INET6_ADDRSTRLEN);
    return str;
}

static char* to_ipv4(struct sockaddr_in *sa) {
    return inet_ntoa(sa->sin_addr);
}

static PyObject* get_interface_info(struct ifaddrs *ifa) {
     char* if_family = if_addr_family_to_string(ifa->ifa_addr->sa_family);
     int is_inteface_up = (ifa->ifa_flags & IFF_UP)? 1: 0;
     char *addr = NULL;

     if(ifa->ifa_addr->sa_family == AF_INET6) {
         addr = to_ipv6((struct sockaddr_in6 *) ifa->ifa_addr);
     } else if(ifa->ifa_addr->sa_family == AF_INET) {
         addr = to_ipv4((struct sockaddr_in *) ifa->ifa_addr);
     }

     return Py_BuildValue("{sssssiss}",
                          IFA_NAME,
                          ifa->ifa_name,
                          IFA_FAMILY,
                          if_family,
                          IF_IS_UP,
                          is_inteface_up,
                          IF_ADDR,
                          addr
                          );
}

static PyObject* get_all_if(PyObject* self) {
    struct ifaddrs *ifap, *ifa;
    getifaddrs(&ifap);
    PyObject *list = PyList_New(0);
    for (ifa = ifap; ifa; ifa = ifa->ifa_next)
    {
        PyObject* item = get_interface_info(ifa);
        PyList_Append(list, item);
    }
    freeifaddrs(ifap);
    return list;
}

/*static void getMac(char *iface) {
    int fd;
    struct ifreq ifr;
    unsigned char *mac;

    fd = socket(AF_INET, SOCK_DGRAM, 0);

    ifr.ifr_addr.sa_family = AF_INET;
    strncpy(ifr.ifr_name , iface , IFNAMSIZ-1);

    ioctl(fd, SIOCGIFHWADDR, &ifr);

    close(fd);

    mac = (unsigned char *)ifr.ifr_hwaddr.sa_data;

    //display mac address
    printf("Mac : %.2x:%.2x:%.2x:%.2x:%.2x:%.2x\n" , mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

}
*/
static int is_if_up(struct ifaddrs *ifa)
{
    return ifa->ifa_flags & IFF_UP;
}

static PyObject* get_all_if_by_AF_int(int AF) {
    struct ifaddrs *ifap, *ifa;
    getifaddrs(&ifap);

    PyObject *list = PyList_New(0);
    for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
        struct sockaddr* ifa_addr = ifa->ifa_addr;
        if(ifa_addr != NULL) {
            if (ifa_addr->sa_family == AF) {
                PyObject* item = Py_BuildValue("s", ifa->ifa_name);
                PyList_Append(list, item);
            }
        }
    }
    freeifaddrs(ifap);
    return list;
}

static PyObject* get_info_if_by_AF_int(int AF) {
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

static PyObject* get_ip(PyObject *self, PyObject *args)
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

static PyObject* get_info_about_active_devices(void)
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

static PyObject* get_active_devices(void)
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
        {"get_all_system_if", (PyCFunction) get_all_if, METH_NOARGS, "usage: show_if()\n"},
        {"get_all_ipv4_if", (PyCFunction) get_list_ipv4_if, METH_NOARGS, "usage: get_list_ipv4_if()\n"},
        {"get_all_ipv6_if", (PyCFunction) get_list_ipv6_if, METH_NOARGS, "usage: get_list_ipv6_if()\n"},
        {"get_all_if_by_AF", (PyCFunction) get_all_if_by_AF, METH_VARARGS, "usage: get_all_if_by_AF()\n"},
        {"get_info_if_by_AF", (PyCFunction) get_info_if_by_AF, METH_VARARGS, "usage: get_info_if_by_AF()\n"},
        {"get_active_devices", (PyCFunction) get_active_devices, METH_NOARGS, "usage: get_active_devices()\n"},
        {"get_info_about_active_devices", (PyCFunction) get_info_about_active_devices, METH_NOARGS, "usage: get_info_about_active_devices()\n"},
        {"get_ip", (PyCFunction) get_ip, METH_VARARGS, "usage: get_ip()\n"},
        {NULL}
};

static struct PyModuleDef SpeedyNicModuleDef =
        {
                PyModuleDef_HEAD_INIT,
                "my_module", /* name of module */
                "usage: my_module.show_if()\n", /* module documentation, may be NULL */
                -1,   /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
                module_methods
        };

PyMODINIT_FUNC PyInit_my_module(void)
{
    return PyModule_Create(&SpeedyNicModuleDef);
}