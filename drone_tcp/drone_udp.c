#include "auxiliary_funcs.h"
#include "gyro.h"

struct udp_pcb *self_pcb;
struct udp_pcb *client_pcb;
ip_addr_t remote_ip;
u16_t remote_port;

volatile bool connect_flag;//Primary flag to establish connection --Turns off Broadcasting
volatile float battery_voltage;

// //Append Stats
// int append_stats(char* inp){ 
//     return 0;
// }

int wifi_setup(){
    //Init cy43
    cyw43_pm_value(CYW43_NO_POWERSAVE_MODE,200,1,1,10);//VV IMP: otherwise RF switches off???
    cyw43_arch_enable_sta_mode();

    //Init wifi
    printf("Connecting to WiFi...\n");
    if (cyw43_arch_wifi_connect_timeout_ms(WIFI_SSID, WIFI_PASSWORD, CYW43_AUTH_WPA2_AES_PSK, 30000)) {
        printf("failed to connect to WiFi\n");
        pico_led_blink(3,1000);
        return 1;
    } 
    else {
        printf("Connected to WiFi\n");
        pico_led_blink(2,500);
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
        printf("Udp Client identified\n");
        pico_led_blink(2,500);
   }
   struct pbuf * psend = pbuf_alloc(PBUF_TRANSPORT,strlen(p->payload),PBUF_RAM);
   memcpy(psend->payload,p->payload,strlen(p->payload));
   udp_sendto(self_pcb,psend,&remote_ip,remote_port);
   pbuf_free(psend);
   pbuf_free(p);
   instruction_gather(p->payload);
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

void udp_send_mpu_data(char* data){
    struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT,sizeof(mpu),PBUF_RAM);
    memcpy(p->payload,data,sizeof(mpu));
    udp_sendto(self_pcb,p,&remote_ip,remote_port);
    pbuf_free(p);
    //printf("%d",sizeof(mpu));
}

int udp_setup(){
    self_pcb=udp_new();
    //client_pcb=udp_new();
    connect_flag=false;
    if(udp_bind(self_pcb,IP_ADDR_ANY,4242)!=ERR_OK){//Binds Port to IP
        printf("IP Binding failed");
        pico_led_blink(3,1000);
    return 1;
    }
    udp_recv(self_pcb,udp_receiver,NULL);
    cyw43_arch_poll();

    //Broadcast till connected --Blocking
    printf("Broadcast is beginning\n");
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

     //Do not use blink before this
    if (cyw43_arch_init()) {
        printf("failed to initialise\n");
        return 1;
    }
    pico_led_blink(4,500);

    if(wifi_setup()!=0)
        return 1;
    printf("Wifi Setup Successful\n\n");
    pico_led_blink(2,500);

    if(udp_setup()!=0)
        return 1;
    printf("Udp Setup Successful\n\n");
    pico_led_blink(2,500);

    i2c_begin();
    printf("I2C Succesful\n");
    pico_led_blink(2,500);
    
    //timed_accel_gyro();  --Remember to uncomment in the future
    init_pwm();
    bms_init();
    timed_bms();
    //timed_normalizer();  --This too

    //timed_autocorrect(); // within normalization
    //Standard Operation begins
    while(1){
        //Processing Here
        if(send_gyro_flag)
            udp_send_mpu_data((char*)&mpu_normalized);//Keep sending mpu data
        }
    return 0;
}   