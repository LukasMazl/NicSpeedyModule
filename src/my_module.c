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

static char* to_ipv4(struct sockaddr_in *sa) {
    return inet_ntoa(sa->sin_addr);
}

static void getMac();

static PyObject* show_if(PyObject* self) {
    struct ifaddrs *ifap, *ifa;
    char *addr;
    int family, s, n;
    char host[NI_MAXHOST];

    int N = 0;
    getifaddrs(&ifap);
    for (ifa = ifap; ifa; ifa = ifa->ifa_next)
            N++;

    int i = 0;
    PyObject *python_val = PyList_New(N);
    for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
        family = ifa->ifa_addr->sa_family;
        /* Display interface name and family (including symbolic
           form of the latter for the common families) */

        printf("%-8s %s (%d)\n",
               ifa->ifa_name,
               (family == AF_PACKET) ? "AF_PACKET" :
               (family == AF_INET) ? "AF_INET" :
               (family == AF_INET6) ? "AF_INET6" : "???",
               family);
        addr = to_ipv4((struct sockaddr_in *) ifa->ifa_addr);
        getMac(ifa->ifa_name);
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
    getMac();
    freeifaddrs(ifap);
    return python_val;
}

static void getMac(char *iface) {
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

static char show_if_docs[] =
        "usage: show_if()\n";

static PyMethodDef module_methods[] = {
        {"show_if", (PyCFunction) show_if, METH_NOARGS, show_if_docs},
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