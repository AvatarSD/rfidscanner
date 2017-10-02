#include <iostream>
#include <libusb-1.0/libusb.h>
#include <thread>
#include <chrono>

using namespace std;

libusb_context *ctx = NULL; //a libusb session

void printdev(libusb_device *dev); //prototype of the function
void scanner_work();

int main() {
    int r; //for return values
    ssize_t cnt; //holding number of devices in list
    r = libusb_init(&ctx); //initialize a library session
    if(r < 0) {
        cout<<"Init Error "<<r<<endl; //there was an error
        return 1;
    }
    libusb_set_debug(ctx, 3); //set verbosity level to 3, as suggested in the documentation

    /*libusb_device **devs; //pointer to pointer of device, used to retrieve a list of devices
    cnt = libusb_get_device_list(ctx, &devs); //get the list of devices
    if(cnt < 0) {
        cout<<"Get Device Error"<<endl; //there was an error
    }
    cout<<cnt<<" Devices in list."<<endl; //print total number of usb devices
    ssize_t i; //for iterating through the list
    for(i = 0; i < cnt; i++) {

        libusb_device_descriptor desc;
        int r = libusb_get_device_descriptor(devs[i], &desc);
        if (r < 0) {
            cout<<"failed to get device descriptor"<<endl;
            continue;
        }

        if(desc.idVendor == 0x04d8)
            cout << "scanner!!!: ";


        printdev(devs[i]); //print specs of this device
    }
    libusb_free_device_list(devs, 1); //free the list, unref the devices in it*/

    scanner_work();
    libusb_exit(ctx); //close the session
    return 0;
}

void usb_callback_readed(libusb_transfer *transfer){
    cout << "actLen:" << transfer->actual_length << "   buf: " ;

    for(int i = 0; i < transfer->actual_length; i++){
        if( /*(transfer->buffer[i] == 0x81) ||
            (transfer->buffer[i] == 0x82)||*/
            (transfer->buffer[i] == 0xCC)||
            (transfer->buffer[i] == 0x7C))
            cout << endl;
        printf("%02X ", transfer->buffer[i]);
        if( (transfer->buffer[i] == 0x98)||
            (transfer->buffer[i] == 0x46)||
            (transfer->buffer[i] == 0x81))
            cout << endl;
    }
    cout << endl;

    libusb_submit_transfer(transfer);
}

void usb_callback_writed(libusb_transfer *transfer){
}

void runRequest(libusb_device_handle* dev_h)
{
    static unsigned char buf[30];
    buf[0] = 0x82;//7C;
    buf[1] = 0x00;//FF;
    buf[2] = 0x02;//FF;
    buf[3] = 0xa0;//01;
    buf[4] = 0xb8;//08;
    buf[5] = 0x00;//7D;
    buf[6] = 0x85;//

    libusb_transfer * transferWrite = libusb_alloc_transfer(0);

    while(1)
    {
        libusb_fill_interrupt_transfer(transferWrite,
                                       dev_h,
                                       0x01,
                                       buf,
                                       7,
                                       &usb_callback_writed,
                                       nullptr,
                                       000);
        int ret = libusb_submit_transfer(transferWrite);
        if(ret<0){
            cout << "write end!! err:" << ret
                 << "  count:" << transferWrite->actual_length
                 << endl;
            //            return;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

void scanner_work()
{
#define VID 0x04d8 //0x25a7
#define PID 0x003f //0x0701
#define IFACE 0

    while(1){
        /****** opening *****/
        libusb_device_handle * dev_h = libusb_open_device_with_vid_pid(ctx,VID,PID);
        if(dev_h == NULL){
            cout << "cannot open device\r\n";
            return;
        }
        cout << "dev opened!\r\n";

        /****** reset device *****/
        //        cout << "reset stat: " << libusb_reset_device(dev_h) << endl;

        /****** dethaching driver *****/
        int drvEn = libusb_kernel_driver_active(dev_h, IFACE);
        if(1==drvEn){
            cout << "kernel driver is active" << endl;
            if(0==libusb_detach_kernel_driver(dev_h, IFACE))
                cout << "driver dethatched" << endl;
            else
                cout << "cannot dedhacj" << endl;
            cout << "kernel driver activity:" << libusb_kernel_driver_active(dev_h, IFACE) << endl;
        } else if(0==drvEn)
            cout << "kernel driver is not active - ok" << endl;
        else
            cout << "kernel driver get status error!" << endl;

        /****** clain iface *****/
        int r = libusb_claim_interface(dev_h, IFACE);
        if(r<0)
            cout << "can not claim interface\r\n";
        else
            cout << "claim interface - ok\r\n";

        /***** ep status *****/
        unsigned char data[2];
        r = libusb_control_transfer(dev_h, 0b10000010,
                                    LIBUSB_REQUEST_GET_STATUS,
                                    0, 0x01, data, 2, 0);
        cout<<"ep 0x01(OUT) ret:" << r << "  status: " << (int)data[0] << ":" << (int)data[1]<<endl;
        r = libusb_control_transfer(dev_h, 0b10000010,
                                    LIBUSB_REQUEST_GET_STATUS,
                                    0, 0x81, data, 2, 0);
        cout<<"ep 0x01(OUT) ret:" << r << "  status: " << (int)data[0] << ":" << (int)data[1]<<endl;
        r = libusb_clear_halt(dev_h, 0x01);
        cout << "0x01 ep halt ret:" << r << endl;
        r = libusb_clear_halt(dev_h, 0x81);
        cout << "0x81 ep halt ret:" << r << endl;

        /***** ep set idle *****/
         r = libusb_control_transfer(dev_h,0x21,0x0a,0,0,data,0,5000);
        cout << "0x01 set_idle ret:" << r << endl;

        /***** create read thransfer *****/
        static unsigned char buf[1000] = {0,0,0,0,0,0,0,0,0,0};
        libusb_transfer * transferRead = libusb_alloc_transfer(0);
        libusb_fill_interrupt_transfer(transferRead,
                                       dev_h,
                                       0x81,
                                       buf,
                                       1000,
                                       &usb_callback_readed,
                                       nullptr,
                                       00);
        libusb_submit_transfer(transferRead);

        /***** do request in separete thread *****/
        std::thread requestThread([&](){runRequest(dev_h);});

        /***** handle events *****/
        while(1){
            libusb_handle_events(ctx);
        }

        //requestThread.join();

        libusb_close(dev_h);
    }
}


void printdev(libusb_device *dev) {

    libusb_device_descriptor desc;
    int r = libusb_get_device_descriptor(dev, &desc);
    if (r < 0) {
        cout<<"failed to get device descriptor"<<endl;
        return;
    }
    cout<< (int)desc.iManufacturer <<" ||| ";
    cout<<"Number of config's: "<<(int)desc.bNumConfigurations<<"  ";
    cout<<"Device Class: "<<(int)desc.bDeviceClass<<"  ";
    cout<<"VendorID: "<<desc.idVendor<<"  ";
    cout<<"ProductID: "<<desc.idProduct<<endl;
    libusb_config_descriptor *config;
    libusb_get_config_descriptor(dev, 0, &config);
    cout<<"Interfaces: "<<(int)config->bNumInterfaces<<" ||| ";
    const libusb_interface *inter;
    const libusb_interface_descriptor *interdesc;
    const libusb_endpoint_descriptor *epdesc;
    for(int i=0; i<(int)config->bNumInterfaces; i++) {
        inter = &config->interface[i];
        cout<<"Number of alternate settings: "<<inter->num_altsetting<<" | ";
        for(int j=0; j<inter->num_altsetting; j++) {
            interdesc = &inter->altsetting[j];
            cout<<"Interface Number: "<<(int)interdesc->bInterfaceNumber<<" | ";
            cout<<"Number of endpoints: "<<(int)interdesc->bNumEndpoints<<" | ";
            for(int k=0; k<(int)interdesc->bNumEndpoints; k++) {
                epdesc = &interdesc->endpoint[k];
                cout<<"Descriptor Type: "<<(int)epdesc->bDescriptorType<<" | ";
                cout<<"EP Address: "<<(int)epdesc->bEndpointAddress<<" | ";
            }
        }
    }
    cout<<endl<<endl<<endl;
    libusb_free_config_descriptor(config);
}
