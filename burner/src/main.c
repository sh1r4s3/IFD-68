#include "map.h"
#include <libusb.h>
#include <string.h>

const unsigned short idVendor  = 0x0483; // STMicroelectronics
const unsigned short idProduct = 0x4021; // 69M-LED
const unsigned char  ep        = 0x01;   // from USB sniffing

void terminate(libusb_context *usbContext, libusb_device_handle *devHandle);

int main(int argc, char **argv)
{
  fflush(stdin);
  if (argc != 2)
  {
    printf("Usage: ./ifd68 map.asc\nGenerating default map.asc...");
    def_map();
    printf(" Done!\n");
    return 0;
  }

  int err; // place error code from function call
  libusb_context *usbContext = 0;

  // init libusb
  printf("Initializing libusb\n");
  if (err = libusb_init(&usbContext))
  {
    printf("Can't initialize libusb, ERROR = %i\n", err);
    return 1;
  }

  // get device
  libusb_device_handle *devHandle = libusb_open_device_with_vid_pid(usbContext,
                                                                    idVendor,
                                                                    idProduct);
  if (!devHandle)
  {
    printf("Can't get device handle\n");
    terminate(usbContext, 0);
    return 1;
  }

  // detach kernel driver otherwise resource will be always busy
  if (err = libusb_set_auto_detach_kernel_driver(devHandle, 1))
  {
    printf("Can't detach kernel driver, ERROR = %i\n", err);
    terminate(usbContext, 0);
    return 1;
  }

  // claim an interface
  if (err = libusb_claim_interface(devHandle, 1))
  {
    printf("Can't claim interface, ERROR = %i\n", err);
    terminate(usbContext, devHandle);
    return 1;
  }

  // for bulk transfer
  unsigned char chunk[64];
  int szTrans = 0;

  // hello message
  memset(chunk, 0x00, 64);
  chunk[1] = 0xF1; // HI!
  if (err = libusb_bulk_transfer(devHandle, ep, chunk, 64, &szTrans, 0))
  {
    printf("Chunk transfer ERROR %i\n", err);
    terminate(usbContext, devHandle);
    return 1;
  }

  // setup keyboard
  FILE *hFile = fopen(argv[1], "r");
  if (!hFile)
  {
    printf("Can't open\n");
    terminate(usbContext, devHandle);
    return 1;
  }

  // read input file and set up chunks
  chunk[1] = 0xF0;
  while (1)
  {
    char entry[0xFF];
    fgets(entry, 0xFF, hFile);
    if (feof(hFile))   break;

    printf("--- new token ---\n");
    // read keys "FN_MODE KEY_FROM KEY_1 KEY_2 ... KEY_6
    char *saveptr;
    char doWerite = 0;
    unsigned int keyCounter = 0;
    MAP *m;
    entry[strlen(entry) - 1] = 0; // replace \n with 0
    char *token = strtok_r(entry, " ", &saveptr);
    for (unsigned int i = 0; i < 7; i++)
    {
      if (token == NULL)   break;
      else
      {
        printf("token[%u] = %s | ", i, token);
        switch (i)
        {
          case 0:
            if (strstr(token, "FN_ON"))   chunk[2] = 0x02;
            else                          chunk[2] = 0x01;
            break;
          case 1:
            if (m = search_in_map(token))   chunk[3] = m->lkey;
            else                            err = 1;
            break;
          default:
            if (m = search_in_map(token))   chunk[keyCounter++ + 6] = m->rkey;
            else                            err = 1;
        }
        if (err)   break;
      }
      token = strtok_r(0, " ", &saveptr);
    }

    if (!err)   chunk[5] = keyCounter;
    else
    {
      printf("Ops!\n");
      break;
    }

    if (err = libusb_bulk_transfer(devHandle, ep, chunk, 64, &szTrans, 0))
    {
      printf("ERROR %i\n", err);
    }
    else
    {
      printf("mod=0x%X key=0x%X -> mod=0x%X key=0x%X\n", entry[0], entry[1], entry[2], entry[3]);
    }
  }

  fclose(hFile);

  // bye message
  memset(chunk + 1, 0xF6, 63);
  chunk[0] = 0x00; // BYE!
  if (libusb_bulk_transfer(devHandle, ep, chunk, 64, &szTrans, 0))
    printf("ERROR 4\n");

  terminate(usbContext, devHandle);
  return 0;
}



void terminate(libusb_context *usbContext, libusb_device_handle *devHandle)
{
  if (devHandle)
  {
    libusb_release_interface(devHandle, 1);
    libusb_close(devHandle);
  }
  if (usbContext)   libusb_exit(usbContext);
}
