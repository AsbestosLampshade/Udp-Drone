//#include "auxiliary_funcs.h"
//#include "gyro.h"

#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "lwip/pbuf.h"
#include "lwip/udp.h"

struct udp_pcb *self_pcb;
ip_addr_t remote_ip;
u16_t remote_port;

volatile bool connect_flag;//Primary flag to establish connection --Turns off Broadcasting

//Append Stats
// int append_stats(char* inp){ 
//     return 0;
// }

int wifi_setup(){
    //Init cy43
    if (cyw43_arch_init()) {
        printf("failed to initialise\n");
        return 1;
    }
    cyw43_arch_enable_sta_mode();

    //Init wifi
    printf("Connecting to WiFi...\n");
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        printf("failed to connect to WiFi\n");
        return 1;
    } else {
        printf("Connected to WiFi\n");
        return 0;
    }

}

void udp_receiver(void * arg, struct udp_pcb *pcb, struct pbuf *p, const ip_addr_t *addr,u16_t port) //- (* arg, udp_pcb, pbuf, ip4_addr,unsigned short)
{
    printf("received from %d.%d.%d.%d port=%d\n",addr->addr&0xff,
                                              (addr->addr>>8)&0xff,
                                              (addr->addr>>16)&0xff,
                                              addr->addr>>24,port);

   printf("Length = %d , Total Length = %d\n",p->len,p->tot_len);
   printf("payload -> %s\n", (char *) p->payload);
   if(!connect_flag){
        connect_flag=true;
        remote_ip=(ip_addr_t)*addr;
        remote_port=port;
        printf("Udp Client identified");
   }
    printf("Entered");
    struct pbuf * psend = pbuf_alloc(PBUF_TRANSPORT,strlen(p->payload),PBUF_RAM);
    memcpy(psend->payload,p->payload,strlen(p->payload));
    udp_sendto(self_pcb,psend,&remote_ip,remote_port);
    pbuf_free(psend);
    printf("Exited");
}

bool udp_broadcaster(){
    char* data = "Requesting Connection";
    struct pbuf * p = pbuf_alloc(PBUF_TRANSPORT,strlen(data),PBUF_RAM);
    memcpy(p->payload,data,strlen(data));
    udp_sendto(self_pcb,p,IP_ADDR_BROADCAST,4242);
    //192.168.1.255 - local broadcast ip -- for 192.168.1.1    !!!!!!!!!!!! - this does full broadcast - !!!!!!!!!!!!!!!!
    pbuf_free(p);
    return true;
}

int udp_setup(){
    self_pcb=udp_new();
    //client_pcb=udp_new();
    connect_flag=false;
    if(udp_bind(self_pcb,IP_ADDR_ANY,4242)!=ERR_OK){//Binds Port to IP
        printf("IP Binding failed");
    return 1;
    }
    udp_recv(self_pcb,udp_receiver,NULL);
    //cyw43_arch_poll();

    printf("Broadcast is beginning\n");
    //Broadcast till connected --Blocking
    struct repeating_timer timer;
    add_repeating_timer_ms(-1000, udp_broadcaster, NULL, &timer);
    while(!connect_flag);
    cancel_repeating_timer(&timer);
    return 0;
}

int main(){
    stdio_init_all();
    sleep_ms(10000);
    printf("Device Booted\n");

    if(wifi_setup()!=0)
        return 1;

    if(udp_setup()!=0)
        return 1;

    printf("Setup exited");
    //Standard Operation begins
    while(1){
        //Processing Here
        sleep_ms(1000);
        printf("------------------------------------Alive");
    }
    return 0;
}