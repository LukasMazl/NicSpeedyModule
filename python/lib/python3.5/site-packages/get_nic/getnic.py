
import os
import platform


#Function to find all interfaces.
#Returns a list of interfaces
def interfaces():
    
    if (platform.system() == 'Linux'):
        

        try:
            res = os.popen("ip link show")
            
        except OSError:
            print ("Error execuring ip link show. Currently Linux is only supported")

        split = res.read().split()

        if res.close == 256:
            return ("Did not work")

        j,interfaces=1,[]
        
        for i in range(len(split)):
            if split[i] == str(j)+':':                

                j=j+1
                interface=split[i+1]
                interfaces.append(interface[:-1])
                
        return (interfaces)

    else:
        return ("Linux OS is currently supported, Check OS")

    
#Function to find IPaddress, class, MAC address and status of the interfaces, pass interface as a list.
#Returns dictionary containing details of the interface list passed
def ipaddr(interfaces):
    ipdetails={}
    
    if (platform.system() == 'Linux'):
            
        for interface in interfaces:
                iplist={}
                interfacesdetail={}
                try:                    
                    res = os.popen("ip a list " + str(interface))
                except OSError:
                    print ("Error in executing ip addr list")

                ipaddrd = res.read().split()
             

                if os.popen("ip a list " + str(interface)).close()==256:
                    return ("ip addr list caused an error")                
                    
                
                for i in range(len(ipaddrd)):
                    if ipaddrd[i] == 'inet' or ipaddrd[i] == 'inet6':

                        if ipaddrd[i] == 'inet':
                                                       
                            iplist.update({ipaddrd[i]+'4':ipaddrd[i+1]})                               
                            interfacesdetail.update({interface:iplist})
                                                  

                        if ipaddrd[i] == 'inet6':
                            iplist.update({ipaddrd[i]:ipaddrd[i+1]})                               
                            interfacesdetail.update({interface:iplist})
                                
                        else:
                            interfacesdetail.update({interface:iplist})

                    if ipaddrd[i] == 'link/ether':
                        iplist.update({'HWaddr':ipaddrd[i+1]})                               
                        interfacesdetail.update({interface:iplist})

                    if ipaddrd[i] == 'state':
                        iplist.update({'state':ipaddrd[i+1]})                               
                        interfacesdetail.update({interface:iplist})                        

                ipdetails.update(interfacesdetail)
        return ipdetails

    else:
        return ("Linux OS is currently supported, Check OS")

    



