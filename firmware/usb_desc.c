#include "usb_desc.h"
#include "desc.h"

#include "usb_hid_rd.h"

#define O_HIDLIKE
#define O_USEHID

/* HID Report Descriptor */
static const uint8_t USB_HID_ReportDescriptor[] = {
	USAGE_PAGE WORD 0x00, 0xFF, // Vendor
	USAGE BYTE 1, // #1
	COLLECTION APPLICATION,
		REPORT_ID BYTE 1,

		LOGICAL_MIN BYTE 0,
		LOGICAL_MAX BYTE 1,
		REPORT_SIZE BYTE 1,
		REPORT_COUNT BYTE 2,
		USAGE BYTE 0x10, // #RESET
		USAGE BYTE 0x11, // #POWER
		FEATURE BYTE F_VAR,
		REPORT_COUNT BYTE 6,
		FEATURE BYTE F_CONST | F_VAR,
	END_COLL
};

/* USB Standard Device Descriptor */
static const uint8_t USB_DeviceDescriptor[] = {
  USB_DEVICE_DESC_SIZE,              /* bLength */
  USB_DEVICE_DESCRIPTOR_TYPE,        /* bDescriptorType */
  WBVAL(0x0110), /* 1.10 */          /* bcdUSB */
#ifndef O_HIDLIKE
  USB_CDC_DEVICE_CLASS,              /* bDeviceClass */
  USB_CDC_DEVICE_SUBCL,              /* bDeviceSubClass */
  USB_CDC_DEVICE_PROTO,              /* bDeviceProtocol */
#else
  USB_IAD_DEVICE_CLASS,              /* bDeviceClass */
  USB_IAD_DEVICE_SUBCL,              /* bDeviceSubClass */
  USB_IAD_DEVICE_PROTO,              /* bDeviceProtocol */
#endif
  USB_MAXPACKETSIZE0,                /* bMaxPacketSize0 */
  WBVAL(0xFF08),                     /* idVendor (My Awesome Company fake id) */
  WBVAL(_DEVICE_TYPE),               /* idProduct */
  WBVAL(0x0100), /* 1.00 */          /* bcdDevice */
  0x01,                              /* iManufacturer */
  0x02,                              /* iProduct */
  0x03,                              /* iSerialNumber */
  0x01                               /* bNumConfigurations */
};

/* USB Configuration Descriptor */
/*   All Descriptors (Configuration, Interface, Endpoint, Class, Vendor */
static const uint8_t USB_ConfigDescriptor[] = {
/* Configuration 1 */
  USB_CONFIGUARTION_DESC_SIZE,       /* bLength */
  USB_CONFIGURATION_DESCRIPTOR_TYPE, /* bDescriptorType */
  WBVAL(                             /* wTotalLength */
    USB_CONFIGUARTION_DESC_SIZE
#ifdef O_HIDLIKE
    +USB_IAD_DESC_SIZE
#endif
    +USB_INTERFACE_DESC_SIZE
    +USB_ENDPOINT_DESC_SIZE
    +USB_CDC_FUNCDESC_HEADER_SIZE
    +USB_CDC_FUNCDESC_CALL_SIZE
    +USB_CDC_FUNCDESC_ABSTRACT_SIZE
    +USB_CDC_FUNCDESC_UNION_SIZE(1)
    +USB_INTERFACE_DESC_SIZE
    +USB_ENDPOINT_DESC_SIZE*2
#ifdef O_HIDLIKE
    +USB_INTERFACE_DESC_SIZE
#ifdef O_USEHID
    +USB_HID_DESC_SIZE
#endif
    +USB_ENDPOINT_DESC_SIZE*2
#endif
  ),
#ifdef O_HIDLIKE
  0x03,                              /* bNumInterfaces */
#else
  0x02,                              /* bNumInterfaces */
#endif
  0x01,                              /* bConfigurationValue */
  0x04,                              /* iConfiguration */
  USB_CONFIG_SELF_POWERED,           /* bmAttributes */
  USB_CONFIG_POWER_MA(50),           /* bMaxPower */
#ifdef O_HIDLIKE
/* IAD */
  USB_IAD_DESC_SIZE,			// bLength
  USB_IAD_DESCRIPTOR_TYPE,		// bDescriptorType
  0,					// bFirstInterface
  2,					// bInterfaceCount
  USB_CDC_COMMIF_CLASS,			// bFunctionClass
  USB_CDC_COMMIF_SUBCL_ABSTRACT,	// bFunctionSubClass
  USB_CDC_COMMIF_PROTO_AT,		// bFunctionProtocol
  0,					// iInterface
#endif
/* Interface 0, Alternate Setting 0, CDC Class */
  USB_INTERFACE_DESC_SIZE,           /* bLength */
  USB_INTERFACE_DESCRIPTOR_TYPE,     /* bDescriptorType */
  0x00,                              /* bInterfaceNumber */
  0x00,                              /* bAlternateSetting */
  0x01,                              /* bNumEndpoints */
  USB_CDC_COMMIF_CLASS,              /* bInterfaceClass */
  USB_CDC_COMMIF_SUBCL_ABSTRACT,     /* bInterfaceSubClass */
  USB_CDC_COMMIF_PROTO_AT,           /* bInterfaceProtocol */
  0x00,                              /* iInterface */
/* Function Descriptor Header */
  USB_CDC_FUNCDESC_HEADER_SIZE,
  USB_CDC_CS_INTERFACE,
  USB_CDC_SCS_HEADER,
  WBVAL(0x0110), /* 1.10 */          /* bcdCDC */
/* Function Descriptor Call */
  USB_CDC_FUNCDESC_CALL_SIZE,
  USB_CDC_CS_INTERFACE,
  USB_CDC_SCS_CALL,
  0x00,
  0x01,
/* Function Descriptor Abstract */
  USB_CDC_FUNCDESC_ABSTRACT_SIZE,
  USB_CDC_CS_INTERFACE,
  USB_CDC_SCS_ABSTRACT,
  0x00,
/* Function Descriptor Union */
  USB_CDC_FUNCDESC_UNION_SIZE(1),
  USB_CDC_CS_INTERFACE,
  USB_CDC_SCS_UNION,
  0x00,
  0x01,
/* Endpoint, Interrupt In */
  USB_ENDPOINT_DESC_SIZE,            /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,      /* bDescriptorType */
  USB_ENDPOINT_IN(1),                /* bEndpointAddress */
  USB_ENDPOINT_TYPE_INTERRUPT,       /* bmAttributes */
  WBVAL(USB_MAXPACKETSIZE0),         /* wMaxPacketSize */
  0x20,          /* 32ms */          /* bInterval */
/* Interface 1, Alternate Setting 0, CDC Class */
  USB_INTERFACE_DESC_SIZE,           /* bLength */
  USB_INTERFACE_DESCRIPTOR_TYPE,     /* bDescriptorType */
  0x01,                              /* bInterfaceNumber */
  0x00,                              /* bAlternateSetting */
  0x02,                              /* bNumEndpoints */
  USB_CDC_DATAIF_CLASS,              /* bInterfaceClass */
  USB_CDC_DATAIF_SUBCL,              /* bInterfaceSubClass */
  USB_CDC_DATAIF_PROTO_NONE,         /* bInterfaceProtocol */
  0x00,                              /* iInterface */
/* Endpoint, In */
  USB_ENDPOINT_DESC_SIZE,            /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,      /* bDescriptorType */
  USB_ENDPOINT_IN(2),                /* bEndpointAddress */
  USB_ENDPOINT_TYPE_BULK,            /* bmAttributes */
  WBVAL(COMMPACKET_LEN),             /* wMaxPacketSize */
  0x20,          /* 32ms */          /* bInterval */
/* Endpoint, Out */
  USB_ENDPOINT_DESC_SIZE,            /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,      /* bDescriptorType */
  USB_ENDPOINT_OUT(3),               /* bEndpointAddress */
  USB_ENDPOINT_TYPE_BULK,            /* bmAttributes */
  WBVAL(COMMPACKET_LEN),             /* wMaxPacketSize */
  0x20,          /* 32ms */          /* bInterval */
#ifdef O_HIDLIKE
/* Interface 2, Alternate Setting 0, Vendor Class */
  USB_INTERFACE_DESC_SIZE,           /* bLength */
  USB_INTERFACE_DESCRIPTOR_TYPE,     /* bDescriptorType */
  0x02,                              /* bInterfaceNumber */
  0x00,                              /* bAlternateSetting */
  0x02,                              /* bNumEndpoints */
#ifndef O_USEHID
  USB_DEVICE_CLASS_VENDOR_SPECIFIC,  /* bInterfaceClass */
  0x00,                              /* bInterfaceSubClass */
  0xFF,                              /* bInterfaceProtocol */
#else
  USB_DEVICE_CLASS_HUMAN_INTERFACE,  /* bInterfaceClass */
  USB_HID_SUBCLASS_NONE,             /* bInterfaceSubClass */
  USB_HID_PROTOCOL_NONE,             /* bInterfaceProtocol */
#endif
  0x00,                              /* iInterface */
#ifdef O_USEHID
/* HID Class Descriptor */
  USB_HID_DESC_SIZE,                  /* bLength */
  USB_HID_DESCRIPTOR_TYPE,           /* bDescriptorType */
  WBVAL(0x0100), /* 1.00 */          /* bcdHID */
  0x00,                              /* bCountryCode */
  0x01,                              /* bNumDescriptors */
  USB_HID_REPORT_DESCRIPTOR_TYPE,        /* bDescriptorType */
  WBVAL(sizeof(USB_HID_ReportDescriptor)),       /* wDescriptorLength */
#endif
/* Endpoint, Interrupt In */
  USB_ENDPOINT_DESC_SIZE,            /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,      /* bDescriptorType */
  USB_ENDPOINT_IN(4),                /* bEndpointAddress */
  USB_ENDPOINT_TYPE_INTERRUPT,       /* bmAttributes */
  WBVAL(8),                          /* wMaxPacketSize */
  0x20,          /* 32ms */          /* bInterval */
/* Endpoint, Interrupt Out */
  USB_ENDPOINT_DESC_SIZE,            /* bLength */
  USB_ENDPOINT_DESCRIPTOR_TYPE,      /* bDescriptorType */
  USB_ENDPOINT_OUT(5),               /* bEndpointAddress */
  USB_ENDPOINT_TYPE_INTERRUPT,       /* bmAttributes */
  WBVAL(8),                          /* wMaxPacketSize */
  0x20,          /* 32ms */          /* bInterval */
#endif
};

/* USB String Descriptor (optional) */
static const uint8_t USB_StringDescriptor0[] = {
/* LANGID Codes */
  4, /* bLength */
  USB_STRING_DESCRIPTOR_TYPE, /* bDescriptorType */
  WBVAL(0x0409) /* US English */      /* wLANGID */
};
static const USB_STRING_DESC(USB_StringDescriptor1, "My Awesome Company");
static const USB_STRING_DESC(USB_StringDescriptor2, "SPI-bridge");
static const USB_STRING_DESC(USB_StringDescriptor4, "Configuration #1");

void Set_USBDesc() {
	USB_setDescriptor(USB_DEVICE_DESCRIPTOR_TABLE, 0, USB_ADESC(USB_DeviceDescriptor));
	USB_setDescriptor(USB_CONFIGURATION_DESCRIPTOR_TABLE, 0, USB_ADESC(USB_ConfigDescriptor));
	USB_setDescriptor(USB_HID_REPORT_DESCRIPTOR_TABLE, 0, USB_ADESC(USB_HID_ReportDescriptor));
	USB_setDescriptor(USB_STRING_DESCRIPTOR_TABLE, 0, USB_ADESC(USB_StringDescriptor0));
	USB_setDescriptor(USB_STRING_DESCRIPTOR_TABLE, 1, USB_DESC(USB_StringDescriptor1));
	USB_setDescriptor(USB_STRING_DESCRIPTOR_TABLE, 2, USB_DESC(USB_StringDescriptor2));
	USB_setStringDescriptorToUIDReg(3, "SPI-");
	USB_setDescriptor(USB_STRING_DESCRIPTOR_TABLE, 4, USB_DESC(USB_StringDescriptor4));
}
