#ifndef _MODBUS_H
    #define _MODBUS_H

    /* The function returns the CRC as a unsigned short type
        puchMsg  -> message to calculate CRC upon
        usDataLen -> quantity of bytes in message */
    unsigned short modbus_CRC16 (unsigned char *puchMsg, unsigned short usDataLen );
#endif
