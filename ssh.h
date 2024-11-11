#ifndef _SSH_H____
#define _SSH_H____

extern const char *ssh_sigstr(int);
extern const char *ssh_sigstrlong(int);

#define SSH_MSG_DISCONNECT                       1     /* 0x1 */
#define SSH_MSG_IGNORE                           2     /* 0x2 */
#define SSH_MSG_UNIMPLEMENTED                    3     /* 0x3 */
#define SSH_MSG_DEBUG                            4     /* 0x4 */
#define SSH_MSG_SERVICE_REQUEST                  5     /* 0x5 */
#define SSH_MSG_SERVICE_ACCEPT                   6     /* 0x6 */
#define SSH_MSG_KEXINIT                          20    /* 0x14 */
#define SSH_MSG_NEWKEYS                          21    /* 0x15 */
#define SSH_MSG_KEXDH_INIT                       30    /* 0x1e */
#define SSH_MSG_KEXDH_REPLY                      31    /* 0x1f */
#define SSH_MSG_KEX_DH_GEX_REQUEST               30    /* 0x1e */
#define SSH_MSG_KEX_DH_GEX_GROUP                 31    /* 0x1f */
#define SSH_MSG_KEX_DH_GEX_INIT                  32    /* 0x20 */
#define SSH_MSG_KEX_DH_GEX_REPLY                 33    /* 0x21 */
#define SSH_MSG_KEXRSA_PUBKEY                    30    /* 0x1e */
#define SSH_MSG_KEXRSA_SECRET                    31    /* 0x1f */
#define SSH_MSG_KEXRSA_DONE                      32    /* 0x20 */
#define SSH_MSG_USERAUTH_REQUEST                 50    /* 0x32 */
#define SSH_MSG_USERAUTH_FAILURE                 51    /* 0x33 */
#define SSH_MSG_USERAUTH_SUCCESS                 52    /* 0x34 */
#define SSH_MSG_USERAUTH_BANNER                  53    /* 0x35 */
#define SSH_MSG_USERAUTH_PK_OK                   60    /* 0x3c */
#define SSH_MSG_USERAUTH_PASSWD_CHANGEREQ        60    /* 0x3c */
#define SSH_MSG_USERAUTH_INFO_REQUEST            60    /* 0x3c */
#define SSH_MSG_USERAUTH_INFO_RESPONSE           61    /* 0x3d */
#define SSH_MSG_GLOBAL_REQUEST                   80    /* 0x50 */
#define SSH_MSG_REQUEST_SUCCESS                  81    /* 0x51 */
#define SSH_MSG_REQUEST_FAILURE                  82    /* 0x52 */
#define SSH_MSG_CHANNEL_OPEN                     90    /* 0x5a */
#define SSH_MSG_CHANNEL_OPEN_CONFIRMATION        91    /* 0x5b */
#define SSH_MSG_CHANNEL_OPEN_FAILURE             92    /* 0x5c */
#define SSH_MSG_CHANNEL_WINDOW_ADJUST            93    /* 0x5d */
#define SSH_MSG_CHANNEL_DATA                     94    /* 0x5e */
#define SSH_MSG_CHANNEL_EXTENDED_DATA            95    /* 0x5f */
#define SSH_MSG_CHANNEL_EOF                      96    /* 0x60 */
#define SSH_MSG_CHANNEL_CLOSE                    97    /* 0x61 */
#define SSH_MSG_CHANNEL_REQUEST                  98    /* 0x62 */
#define SSH_MSG_CHANNEL_SUCCESS                  99    /* 0x63 */
#define SSH_MSG_CHANNEL_FAILURE                  100   /* 0x64 */
#define SSH_MSG_USERAUTH_GSSAPI_RESPONSE               60
#define SSH_MSG_USERAUTH_GSSAPI_TOKEN                  61
#define SSH_MSG_USERAUTH_GSSAPI_EXCHANGE_COMPLETE      63
#define SSH_MSG_USERAUTH_GSSAPI_ERROR                  64
#define SSH_MSG_USERAUTH_GSSAPI_ERRTOK                 65
#define SSH_MSG_USERAUTH_GSSAPI_MIC                    66


#define      SSH_DISCONNECT_HOST_NOT_ALLOWED_TO_CONNECT             1
#define      SSH_DISCONNECT_PROTOCOL_ERROR                          2
#define      SSH_DISCONNECT_KEY_EXCHANGE_FAILED                     3
#define      SSH_DISCONNECT_RESERVED                                4
#define      SSH_DISCONNECT_MAC_ERROR                               5
#define      SSH_DISCONNECT_COMPRESSION_ERROR                       6
#define      SSH_DISCONNECT_SERVICE_NOT_AVAILABLE                   7
#define      SSH_DISCONNECT_PROTOCOL_VERSION_NOT_SUPPORTED          8
#define      SSH_DISCONNECT_HOST_KEY_NOT_VERIFIABLE                 9
#define      SSH_DISCONNECT_CONNECTION_LOST                        10
#define      SSH_DISCONNECT_BY_APPLICATION                         11
#define      SSH_DISCONNECT_TOO_MANY_CONNECTIONS                   12
#define      SSH_DISCONNECT_AUTH_CANCELLED_BY_USER                 13
#define      SSH_DISCONNECT_NO_MORE_AUTH_METHODS_AVAILABLE         14
#define      SSH_DISCONNECT_ILLEGAL_USER_NAME                      15

#define SSH_OPEN_ADMINISTRATIVELY_PROHIBITED          1
#define SSH_OPEN_CONNECT_FAILED                       2
#define SSH_OPEN_UNKNOWN_CHANNEL_TYPE                 3
#define SSH_OPEN_RESOURCE_SHORTAGE                    4

#endif

