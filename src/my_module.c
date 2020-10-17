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


#define IFA_NAME "name"
#define IFA_FAMILY "family"
#define IF_IS_UP "up"
#define IF_ADDR_IP4 "if_addr_ip4"
#define IF_ADDR_IP6 "if_addr_ip6"

static char* if_addr_family_to_string(int family) {
    return  ((family == AF_PACKET) ? "AF_PACKET" :
            (family == AF_INET) ? "AF_INET" :
            (family == AF_INET6) ? "AF_INET6" : "???");
}

static char* to_ipv6(struct sockaddr_in6 *sa) {
    char *buf = malloc (sizeof (char) * 128);
    inet_ntop(AF_INET6, sa, buf, sizeof (char) * 128);
    return buf;
}

static char* to_ipv4(struct sockaddr_in *sa) {
    return inet_ntoa(sa->sin_addr);
}

static PyObject* get_interface_info(struct ifaddrs *ifa) {
     char* if_family = if_addr_family_to_string(ifa->ifa_addr->sa_family);
     int is_inteface_up = (ifa->ifa_flags & IFF_UP)? 1: 0;
     char *addrIp4 = NULL;
     char *addrIp6 = NULL;

     if(ifa->ifa_addr->sa_family == AF_INET6) {
         addrIp6 = to_ipv6((struct sockaddr_in6 *) ifa->ifa_addr);
     } else if(ifa->ifa_addr->sa_family == AF_INET) {
         addrIp4 = to_ipv4((struct sockaddr_in *) ifa->ifa_addr);
     }

     return Py_BuildValue("{sssssissss}",
                          IFA_NAME,
                          ifa->ifa_name,
                          IFA_FAMILY,
                          if_family,
                          IF_IS_UP,
                          is_inteface_up,
                          IF_ADDR_IP4,
                          addrIp4,
                          IF_ADDR_IP6,
                          addrIp6
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
static PyObject* show_if(PyObject* self) {
    struct ifaddrs *ifap, *ifa;
    char *addr;
    int N = 0;
    getifaddrs(&ifap);
    for (ifa = ifap; ifa; ifa = ifa->ifa_next)
            N++;

    int i = 0;
    PyObject *python_val = PyList_New(N);
    for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
        addr = to_ipv4((struct sockaddr_in *) ifa->ifa_addr);
        PyObject *python_int;
        if (ifa->ifa_flags & IFF_UP) {
            python_int = Py_BuildValue("{s{s[ss]}}", ifa->ifa_name, "ipv4", addr, "up");
        } else {
            python_int = Py_BuildValue("{s{s[ss]}}", ifa->ifa_name, "ipv4", addr, "down");
        }
            PyList_SetItem(python_val, i, python_int);
            printf("Interface: %s\tAddress: %s\n", ifa->ifa_name, addr);
            i++;
    }
    freeifaddrs(ifap);
    return python_val;
}


static PyMethodDef module_methods[] = {
        {"show_if", (PyCFunction) show_if, METH_NOARGS, "usage: show_if()\n"},
        {"get_all_if", (PyCFunction) get_all_if, METH_NOARGS, "usage: show_if()\n"},
        {NULL}
};


static struct PyModuleDef Combinations =
        {
                PyModuleDef_HEAD_INIT,
                "my_module", /* name of module */
                "usage: my_module.show_if()\n", /* module documentation, may be NULL */
                -1,   /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
                module_methods
        };

PyMODINIT_FUNC PyInit_my_module(void)
{
    return PyModule_Create(&Combinations);
}