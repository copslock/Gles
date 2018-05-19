#include "com_app_jagles_smartlink_SmartLink.h"
#include <jni.h>

static int m_smart_link_socket_fd;
static char m_smart_link_socket_buffer[2000];
static struct sockaddr_in m_smart_link_addr_serv; 
static int m_smart_link_serv_len;
static int sendto_ret ;
static int shibai;
static int  message_len, packet_num, i, il;	
static int message_int[ 90 ];
static char message_cun[ 45 ];
static bool fabao;

static int smart_link_sendto(  int sendbuf_length )
{
	if ( fabao == 1 ) {
		sendto_ret = sendto( m_smart_link_socket_fd, m_smart_link_socket_buffer, sendbuf_length, 0, (struct sockaddr *)&m_smart_link_addr_serv, m_smart_link_serv_len);
		shibai = 0;
	}
	
	if( sendto_ret < 0 )
	{
		printf( "smartlink sendto error %d -- %s  errno = %d\n ", sendto_ret, strerror(errno), errno );
		shibai = errno;
		fabao = 0;
		return -1;
	}
	return 0;
}

static void smart_link_send_baotou( int packet_num )
{
	smart_link_sendto( 276 );
	smart_link_sendto( 341 );
	smart_link_sendto( 276 );
	smart_link_sendto( 341 );
	smart_link_sendto( 276 );
	smart_link_sendto( 341 );
	smart_link_sendto( 342 + packet_num);
	//smart_link_sendto( 1342 + packet_num ); 
	smart_link_sendto( 1302 + packet_num ); 
	smart_link_sendto( 342 + packet_num );
	//smart_link_sendto( 1342 + packet_num );
	smart_link_sendto( 1302 + packet_num );  
}


/* ��ȡ���������Ĺ㲥��ַ broadAddr�����㲥��ַ��if_nameΪ������������ */
static int get_brd_addr( char *broadAddr, char *if_name ) 
{  
	int gba_fd;   
	struct ifreq ifr;  
	//char *brd_addr;
	//brd_addr=calloc(16,1);
	char brd_addr[16];
	if ( ( gba_fd = socket( AF_INET, SOCK_DGRAM, 0 ) ) < 0 )  
	{  
		perror ( "socket" ) ;  
		close ( gba_fd ) ;  
		return -1;  
	}  
	snprintf ( ifr.ifr_name,strlen ( if_name ) + 1, "%s", if_name ) ;
	if ( ioctl ( gba_fd, SIOCGIFBRDADDR, &ifr ) )  
	{  

		printf ( "ioctl: %s [%s:%d]\n", strerror(errno),  __FILE__,  __LINE__ ) ;  
		//free ( brd_addr ) ;
		close ( gba_fd ) ;  
		return -1;  		
	}  
	snprintf ( brd_addr, 16, "%s",  
	(char *)inet_ntoa( ( (struct sockaddr_in *)&(ifr.ifr_broadaddr) )->sin_addr) );  
	strncpy ( broadAddr, brd_addr, 16 ) ;
	//free ( brd_addr ) ;
	close ( gba_fd ) ;
	
	return 0;
}   


int nk_smart_link_init( char *interface )
{
	int ret,on=1;   
	char broad_addr[16];

	fabao = 1;
	
	ret = get_brd_addr ( broad_addr, interface ) ;
	if( ret != 0 )
		{
			printf("please check whether there is ip��netmask��Bcast in the network card\n");
			return 1;
		}
	
    if ( ( m_smart_link_socket_fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP) ) < 0 )
	    {
	       perror( "socket(PF_INET, SOCK_DGRAM, IPPROTO_IP):" ) ;
	       return 2;
	    }
	
    ret = setsockopt( m_smart_link_socket_fd, SOL_SOCKET, SO_BROADCAST, &on, sizeof ( on ) );
	if ( ret != 0 ) {
		return 3;
	}
    usleep ( 250 ) ;
    memset( &m_smart_link_addr_serv, 0, sizeof ( m_smart_link_addr_serv ) ) ;  
    m_smart_link_addr_serv.sin_family = AF_INET;  
    m_smart_link_addr_serv.sin_addr.s_addr = inet_addr ( broad_addr ) ;  
    m_smart_link_addr_serv.sin_port = htons( DEST_PORT );   
    m_smart_link_serv_len = sizeof ( m_smart_link_addr_serv ) ; 
	
	return 0;
}

int nk_smart_link_quit()
{
    int ret;
	ret = close ( m_smart_link_socket_fd ) ;
	
	return ret;
}

int nk_smart_link_send( char *message )
{
	//int  message_len, packet_num, i, il;	
	//int message_int[ 90 ];
	//char message_cun[ 45 ];
	
	memset ( message_int, 0, sizeof( message_int ) ) ;
	strncpy( message_cun, message, 45 );
	message_len = strlen( message_cun );
	packet_num = message_len - 1;
	for( i=0; i<message_len * 2; i += 2 )
		{
			il = message_cun[ i/2 ] ;
			message_int[i] = ( il - 32 ) / 10;
			message_int[i+1] = ( il - 32 ) % 10;
		}
	
	smart_link_send_baotou ( packet_num );
	for( i=0; i < message_len * 2; i += 2 )
		{
			smart_link_sendto( 402 + 20 * ( i / 2 ) + message_int[i] );				
			smart_link_sendto( 402 + 20 * ( i / 2 ) + message_int[i] );
			smart_link_sendto( 402 + 20 * ( i / 2 ) + 10 + message_int[ i + 1 ] );		
			smart_link_sendto( 402 + 20 * ( i / 2 ) + 10 + message_int[ i + 1 ] );				
		}	

	return shibai;
}

int JA_smartlink_sendpacket(char *netdev, char *msg)
{
	int ret; 
	ret = nk_smart_link_init( netdev );

	 if ( ret == 0 ) {
	 		ret = nk_smart_link_send( msg );
			nk_smart_link_quit();
			//return ret;
	 }
	 else if ( ret != 1 ) {
	 	nk_smart_link_quit();
	 }
	 
	return ret;	
}

int isinit;
JNIEXPORT int JNICALL Java_com_app_jagles_smartlink_SmartLink_smartLink(
    JNIEnv * env, jobject object, jstring msg)
{
    char *interface="wlan0";
    char * message = (*env)->GetStringUTFChars(env,msg, 0);
    int sendRet;

    sendRet = JA_smartlink_sendpacket(interface, message);
    usleep(100000);
    return sendRet;
}

JNIEXPORT void JNICALL Java_com_app_jagles_smartlink_SmartLink_quitsmartLink(
    JNIEnv *env, jobject thiz)
{
    isinit = 0;
}

JNIEXPORT void JNICALL Java_com_app_jagles_smartlink_SmartLink_smartLinkack(
    JNIEnv *env, jobject thiz)
{
//	char *interface="wlan0";
//    smart_link_send_ack1(interface);
}
