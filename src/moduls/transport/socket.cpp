
#include <sys/types.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <getopt.h>
#include <string>
//#include <signal.h>
#include <errno.h>

#include "../../tsys.h"
#include "../../tkernel.h"
#include "../../tmessage.h"
#include "../../tprotocols.h"
#include "../../tmodule.h"
#include "socket.h"

//============ Modul info! =====================================================
#define NAME_MODUL  "socket"
#define NAME_TYPE   "Transport"
#define VERSION     "0.1.1"
#define AUTORS      "Roman Savochenko"
#define DESCRIPTION "Transport based for inet, unix sockets. inet socket support TCP and UDP"
#define LICENSE     "GPL"
//==============================================================================

extern "C"
{
    TModule *attach( char *FName, int n_mod )
    {
	Sockets::TTransSock *self_addr;
	if(n_mod==0) self_addr = new Sockets::TTransSock( FName );
	else         self_addr = NULL;
	return static_cast< TModule *>( self_addr );
    }
}

using namespace Sockets;

//==============================================================================
//== TTransSock ================================================================
//==============================================================================

TTransSock::TTransSock(char *name) 
    : max_queue(10), max_fork(10), buf_len(4)
{
    NameModul = NAME_MODUL;
    NameType  = NAME_TYPE;
    Vers      = VERSION;
    Autors    = AUTORS;
    DescrMod  = DESCRIPTION;
    License   = LICENSE;
    FileName  = name;
}

TTransSock::~TTransSock()
{

}


void TTransSock::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "============== Module %s command line options =======================\n"
    "------------------ Fields <%s> sections of config file --------------\n"
    "max_sock_queue = <len>      set len queue for TCP and UNIX sockets (default 10);\n" 
    "max_fork       = <connects> set maximum number client's connecting for TCP and UNIX sockets (default 10);\n" 
    "buf_len        = <kb>       set input buffer len (default 4 kb);\n" 
    "\n",NAME_MODUL,NAME_MODUL);
}

void TTransSock::mod_CheckCommandLine(  )
{
    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{NULL        ,0,NULL,0  }
    };

    optind=opterr=0;
    do
    {
	next_opt=getopt_long(SYS->argc,(char * const *)SYS->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': pr_opt_descr(stdout); break;
	    case -1 : break;
	}
    } while(next_opt != -1);
}
 
void TTransSock::mod_UpdateOpt()
{
    try{ max_queue = atoi( mod_XMLCfgNode()->get_child("max_sock_queue")->get_text().c_str() ); }
    catch(...) {  }
    try{ max_fork = atoi( mod_XMLCfgNode()->get_child("max_fork")->get_text().c_str() ); }
    catch(...) {  }
    try{ buf_len = atoi( mod_XMLCfgNode()->get_child("buf_len")->get_text().c_str() ); }
    catch(...) {  }
}

TTransportIn *TTransSock::In(string name, string address, string prot )
{
    TSocketIn *sock = new TSocketIn(name,address,prot,this);
    sock->SetParams(max_queue,max_fork,buf_len);
    return(sock);
}

TTransportOut *TTransSock::Out(string name, string address )
{
    return(new TSocketOut(name,address));
}

//==============================================================================
//== TSocketIn =================================================================
//==============================================================================

TSocketIn::TSocketIn(string name, string address, string prot, TTipTransport *owner ) 
    : TTransportIn(name,address,prot,owner), max_queue(10), max_fork(10), buf_len(4), cl_free(true)
{
    int            pos = 0;
    pthread_attr_t pthr_attr;
    
    string s_type = address.substr(pos,address.find(":",pos)-pos); pos = address.find(":",pos)+1;
    
    if( s_type == S_NM_TCP )
    {
    	if( (sock_fd = socket(PF_INET,SOCK_STREAM,0) )== -1 ) 
    	    throw TError("%s: error create %s socket!",NAME_MODUL,s_type.c_str());
	type = SOCK_TCP;
    }
    else if( s_type == S_NM_UDP )
    {	
	if( (sock_fd = socket(PF_INET,SOCK_DGRAM,0) )== -1 ) 
    	    throw TError("%s: error create %s socket!",NAME_MODUL,s_type.c_str());
	type = SOCK_UDP;
    }
    else if( s_type == S_NM_UNIX )
    {
    	if( (sock_fd = socket(PF_UNIX,SOCK_STREAM,0) )== -1) 
    	    throw TError("%s: error create %s socket!",NAME_MODUL,s_type.c_str());
	type = SOCK_UNIX;
    }
    else throw TError("%s: type socket <%s> error!",NAME_MODUL,s_type.c_str());

    if( type == SOCK_TCP || type == SOCK_UDP )
    {
	struct sockaddr_in  name_in;
	struct hostent *loc_host_nm;
	memset(&name_in,0,sizeof(name_in));
	name_in.sin_family = AF_INET;
        
	host = address.substr(pos,address.find(":",pos)-pos); pos = address.find(":",pos)+1;
	port = address.substr(pos,address.find(":",pos)-pos); pos = address.find(":",pos)+1;
	if( host.size() )
	{
	    loc_host_nm = gethostbyname(host.c_str());
	    if(loc_host_nm == NULL || loc_host_nm->h_length == 0)
		throw TError("%s: socket name <%s> error!",NAME_MODUL,host.c_str());
	    name_in.sin_addr.s_addr = *( (int *) (loc_host_nm->h_addr_list[0]) );
	}
	else name_in.sin_addr.s_addr = INADDR_ANY;  
	if( type == SOCK_TCP )
	{
	    mode = atoi(address.substr(pos,address.find(":",pos)-pos).c_str()); pos = address.find(":",pos)+1;
	    //Get system port for "oscada" /etc/services
	    struct servent *sptr = getservbyname(port.c_str(),"tcp");
	    if( sptr != NULL )                       name_in.sin_port = sptr->s_port;
	    else if( htons(atol(port.c_str())) > 0 ) name_in.sin_port = htons( atol(port.c_str()) );
	    else name_in.sin_port = 10001;
	    
    	    if( bind(sock_fd,(sockaddr *)&name_in,sizeof(name_in) ) == -1) 
	    {
	    	shutdown( sock_fd,SHUT_RDWR );
		close( sock_fd );
		throw TError("%s: TCP socket no bind <%s>!",NAME_MODUL,address.c_str());
	    }
	    listen(sock_fd,max_queue);
	}
	else if(type == SOCK_UDP )
	{
	    //Get system port for "oscada" /etc/services
	    struct servent *sptr = getservbyname(port.c_str(),"udp");
	    if( sptr != NULL )                       name_in.sin_port = sptr->s_port;
	    else if( htons(atol(port.c_str())) > 0 ) name_in.sin_port = htons( atol(port.c_str()) );
	    else name_in.sin_port = 10001;
	    
    	    if( bind(sock_fd,(sockaddr *)&name_in,sizeof(name_in) ) == -1) 
	    {
	    	shutdown( sock_fd,SHUT_RDWR );
		close( sock_fd );
		throw TError("%s: UDP socket no bind <%s>!",NAME_MODUL,address.c_str());
	    }
	}
    }
    else if( type == SOCK_UNIX )
    {
	path = address.substr(pos,address.find(":",pos)-pos); pos = address.find(":",pos)+1;
	mode = atoi(address.substr(pos,address.find(":",pos)-pos).c_str()); pos = address.find(":",pos)+1;
	if( !path.size() ) path = "/tmp/oscada";	
	remove( path.c_str());
	struct sockaddr_un  name_un;	
	memset(&name_un,0,sizeof(name_un));
	name_un.sun_family = AF_UNIX;
	strncpy( name_un.sun_path,path.c_str(),sizeof(name_un.sun_path) );
	if( bind(sock_fd,(sockaddr *)&name_un,sizeof(name_un) ) == -1) 
	{
	    close( sock_fd );
	    throw TError("%s: UNIX socket no bind <%s>!",NAME_MODUL,address.c_str());
	}
	listen(sock_fd,max_queue);
    }
    
    sock_res = SYS->ResCreate();
    
    pthread_attr_init(&pthr_attr);
    pthread_attr_setschedpolicy(&pthr_attr,SCHED_OTHER);
    pthread_create(&pthr_tsk,&pthr_attr,Task,this);
    pthread_attr_destroy(&pthr_attr);
    if( SYS->event_wait( run_st, true, string(NAME_MODUL)+": SocketIn "+Name()+" is opening....",5) )
       	throw TError("%s: SocketIn %s no open!",NAME_MODUL,Name().c_str());   
}

TSocketIn::~TSocketIn()
{
    int cnt = 3;
    if( run_st )
    {
	endrun = true;
        SYS->event_wait( run_st, false, string(NAME_MODUL)+": SocketIn "+Name()+" is closing....");
	pthread_join( pthr_tsk, NULL );
    } 
    shutdown(sock_fd,SHUT_RDWR);
    close(sock_fd); 
    if( type == SOCK_UNIX ) remove( path.c_str() );

    SYS->ResDelete(sock_res);
}

void *TSocketIn::Task(void *sock_in)
{  
    char           *buf;   
    fd_set         rd_fd;
    struct timeval tv;
    TSocketIn *sock = (TSocketIn *)sock_in;

#if OSC_DEBUG
    Mess->put("DEBUG",MESS_DEBUG,"%s:%s: Thread <%d>!",NAME_MODUL,sock->Name().c_str(),getpid() );
#endif	
    
    pthread_t      th;
    pthread_attr_t pthr_attr;
    pthread_attr_init(&pthr_attr);
    pthread_attr_setschedpolicy(&pthr_attr,SCHED_OTHER);
    
    sock->run_st    = true;  
    sock->endrun_cl = false;  
    sock->endrun    = false;
    
    if( sock->type == SOCK_UDP ) 
	buf = new char[sock->buf_len*1000 + 1];
		
    while( !sock->endrun )
    {
	tv.tv_sec  = 1;
	tv.tv_usec = 0;  
    	FD_ZERO(&rd_fd);
	FD_SET(sock->sock_fd,&rd_fd);		
	
	int kz = select(sock->sock_fd+1,&rd_fd,NULL,NULL,&tv);
	if( kz == 0 || (kz == -1 && errno == EINTR) ) continue;
	if( kz < 0) continue;
	if( FD_ISSET(sock->sock_fd, &rd_fd) )
	{
	    struct sockaddr_in name_cl;
	    socklen_t          name_cl_len = sizeof(name_cl);			    
	    if( sock->type == SOCK_TCP )
	    {
		int sock_fd_CL = accept(sock->sock_fd, (sockaddr *)&name_cl, &name_cl_len);
		if( sock_fd_CL != -1 )
		{
		    if( sock->max_fork <= (int)sock->cl_id.size() )
		    {
			close(sock_fd_CL);
			continue;
		    }
#if OSC_DEBUG
    		    Mess->put("DEBUG",MESS_DEBUG,"%s: Connected to TCP socket from <%s>!",NAME_MODUL,inet_ntoa(name_cl.sin_addr) );
#endif		   
		    SSockIn *s_inf = new SSockIn;
		    s_inf->s_in    = sock;
		    s_inf->cl_sock = sock_fd_CL;
		    if( pthread_create(&th,&pthr_attr,ClTask,s_inf) < 0)
    		        Mess->put("SYS",MESS_ERR,"%s: Error create pthread!",NAME_MODUL );
		}
	    }
	    else if( sock->type == SOCK_UNIX )
	    {
		int sock_fd_CL = accept(sock->sock_fd, NULL, NULL);
		if( sock_fd_CL != -1 )
		{
		    if( sock->max_fork <= (int)sock->cl_id.size() )
		    {
			close(sock_fd_CL);
			continue;
		    }		    
#if OSC_DEBUG
		    Mess->put("DEBUG",MESS_DEBUG,"%s: Connected to UNIX socket!",NAME_MODUL);
#endif		    
		    SSockIn *s_inf = new SSockIn;
		    s_inf->s_in    = sock;
		    s_inf->cl_sock = sock_fd_CL;
                    if( pthread_create(&th,&pthr_attr,ClTask,s_inf) < 0 )
			Mess->put("SYS",MESS_ERR,"%s: Error create pthread!",NAME_MODUL );
		}	    
	    }
	    else if( sock->type == SOCK_UDP )
    	    {
    		int r_len;
    		string  req, answ;

    		r_len = recvfrom(sock->sock_fd, buf, sock->buf_len*1000, 0,(sockaddr *)&name_cl, &name_cl_len);
    		if( r_len <= 0 ) continue;
#if OSC_DEBUG
    		Mess->put("DEBUG",MESS_DEBUG,"%s: Recived UDP packet %d from <%s>!",NAME_MODUL,r_len,inet_ntoa(name_cl.sin_addr));
#endif		        
		req.assign(buf,r_len);
	    	sock->PutMess(req,answ);
		if(!answ.size()) continue;
		sendto(sock->sock_fd,answ.c_str(),answ.size(),0,(sockaddr *)&name_cl, name_cl_len);
	    }
	}
    }
    pthread_attr_destroy(&pthr_attr);
    
    if( sock->type == SOCK_UDP ) delete []buf;
    //Client tasks stop command
    sock->endrun_cl = true;
    SYS->event_wait( sock->cl_free, true, string(NAME_MODUL)+": "+sock->Name()+" client task is stoping....");

    sock->run_st = false;
    
    return(NULL);
}

void *TSocketIn::ClTask(void *s_inf)
{
    SSockIn *s_in = (SSockIn *)s_inf;    
    
#if OSC_DEBUG
    Mess->put("DEBUG",MESS_DEBUG,"%s:%s: Client thread <%d>!",NAME_MODUL,s_in->s_in->Name().c_str(),getpid() );
#endif	
   
    s_in->s_in->RegClient( getpid( ), s_in->cl_sock );
    s_in->s_in->ClSock(s_in->cl_sock);
    s_in->s_in->UnregClient( getpid( ) );
    delete s_in;
    
    return(NULL);
}

void TSocketIn::ClSock(int sock)
{
    struct  timeval tv;
    fd_set  rd_fd;
    int     r_len;
    string  req, answ;
    //char    buf[2000]; 
    //char    buf[buf_len*1000 + 1];    
    char *buf = new char[buf_len*1000 + 1];    
    
    if(mode)
    {
    	while( !endrun_cl )
	{
    	    tv.tv_sec  = 1;
    	    tv.tv_usec = 0;  
	    FD_ZERO(&rd_fd);
	    FD_SET(sock,&rd_fd);		
	    
	    int kz = select(sock+1,&rd_fd,NULL,NULL,&tv);
	    if( kz == 0 || (kz == -1 && errno == EINTR) ) continue;
	    if( kz < 0) continue;
	    if( FD_ISSET(sock, &rd_fd) )
	    {
		r_len = read(sock,buf,buf_len*1000);
#if OSC_DEBUG
    		Mess->put("DEBUG",MESS_DEBUG,"%s: Read %d!",NAME_MODUL,r_len);
#endif		    
    		if(r_len <= 0) break;
    		req.assign(buf,r_len);	    	    
    		PutMess(req,answ);
    		if(!answ.size()) continue;
    		r_len = write(sock,answ.c_str(),answ.size());   
	    }
	}
    }
    else
    {
	r_len = read(sock,buf,buf_len*1000);
#if OSC_DEBUG
	Mess->put("DEBUG",MESS_DEBUG,"%s: Read %d!",NAME_MODUL,r_len);
#endif	
	if(r_len > 0) 
	{
	    req.assign(buf,r_len);
	    PutMess(req,answ);
	    if(answ.size()) 
		r_len = write(sock,answ.c_str(),answ.size());   
	}
    }    
    delete []buf;
}

void TSocketIn::PutMess(string &request, string &answer )
{
    TProtocolS &proto = Owner().Owner().Owner().Protocol();
    unsigned hd;
    try { hd = proto.gmd_att(m_prot); }
    catch(...)
    { 
	answer = ""; 
	return; 
    }
    proto.gmd_at(hd).in_mess(request,answer);
    proto.gmd_det(hd);    
}

void TSocketIn::RegClient(pid_t pid, int i_sock)
{
    SYS->WResRequest(sock_res);
    //find already registry
    for( unsigned i_id = 0; i_id < cl_id.size(); i_id++)
	if( cl_id[i_id].cl_pid == pid ) return;
    SSockCl scl = { pid, i_sock };
    cl_id.push_back(scl);
    cl_free = false;
    SYS->WResRelease(sock_res);
}

void TSocketIn::UnregClient(pid_t pid)
{
    SYS->WResRequest(sock_res);
    for( unsigned i_id = 0; i_id < cl_id.size(); i_id++ ) 
	if( cl_id[i_id].cl_pid == pid ) 
	{
	    shutdown(cl_id[i_id].cl_sock,SHUT_RDWR);
	    close(cl_id[i_id].cl_sock);
	    cl_id.erase(cl_id.begin() + i_id);
	    if( !cl_id.size() ) cl_free = true;
	    break;
	}
    SYS->WResRelease(sock_res);
}

//==============================================================================
//== TSocketOut ================================================================
//==============================================================================

TSocketOut::TSocketOut(string name, string address) : TTransportOut(name,address)
{
    int            pos = 0;

    string s_type = address.substr(pos,address.find(":",pos)-pos); pos = address.find(":",pos)+1;
    if( s_type == S_NM_TCP )
    {
    	if( (sock_fd = socket(PF_INET,SOCK_STREAM,0) )== -1 ) 
    	    throw TError("%s: error create %s socket!",NAME_MODUL,s_type.c_str());
	type = SOCK_TCP;
    }
    else if( s_type == S_NM_UDP )
    {	
	if( (sock_fd = socket(PF_INET,SOCK_DGRAM,0) )== -1 ) 
    	    throw TError("%s: error create %s socket!",NAME_MODUL,s_type.c_str());
	type = SOCK_UDP;
    }
    else if( s_type == S_NM_UNIX )
    {
    	if( (sock_fd = socket(PF_UNIX,SOCK_STREAM,0) )== -1) 
    	    throw TError("%s: error create %s socket!",NAME_MODUL,s_type.c_str());
	type = SOCK_UNIX;
    }
    else throw TError("%s: type socket <%s> error!",NAME_MODUL,s_type.c_str());

    if( type == SOCK_TCP || type == SOCK_UDP )
    {
	memset(&name_in,0,sizeof(name_in));
	name_in.sin_family = AF_INET;
        
        string host = address.substr(pos,address.find(":",pos)-pos); pos = address.find(":",pos)+1;
        string port = address.substr(pos,address.find(":",pos)-pos); pos = address.find(":",pos)+1;
	if( !host.size() )
	{
   	    struct hostent *loc_host_nm = gethostbyname(host.c_str());
	    if(loc_host_nm == NULL || loc_host_nm->h_length == 0)
		throw TError("%s: socket name <%s> error!",NAME_MODUL,host.c_str());
	    name_in.sin_addr.s_addr = *( (int *) (loc_host_nm->h_addr_list[0]) );
	}
	else name_in.sin_addr.s_addr = INADDR_ANY;  
	//Get system port for "oscada" /etc/services
	struct servent *sptr = getservbyname(port.c_str(),(type == SOCK_TCP)?"tcp":"udp");
	if( sptr != NULL )                       name_in.sin_port = sptr->s_port;
	else if( htons(atol(port.c_str())) > 0 ) name_in.sin_port = htons( atol(port.c_str()) );
	else name_in.sin_port = 10001;
    }
    else if( type == SOCK_UNIX )
    {
        string path = address.substr(pos,address.find(":",pos)-pos); pos = address.find(":",pos)+1;
	if( !path.size() ) path = "/tmp/oscada";	
	memset(&name_un,0,sizeof(name_un));
	name_un.sun_family = AF_UNIX;
	strncpy( name_un.sun_path,path.c_str(),sizeof(name_un.sun_path) );
    }
}

TSocketOut::~TSocketOut()
{
    shutdown(sock_fd,SHUT_RDWR);
    close(sock_fd); 
}

int TSocketOut::IOMess(char *obuf, int len_ob, char *ibuf, int len_ib, int time )
{
    if( obuf != NULL && len_ob > 0)
    {
    	if( type == SOCK_TCP  )   
	{
	    if( write(sock_fd,obuf,len_ob) == -1 )
		if( connect(sock_fd, (sockaddr *)&name_in, sizeof(name_in)) == -1 )
		    throw TError("%s: %s connect to TCP error!",NAME_MODUL,Name().c_str());
		else write(sock_fd,obuf,len_ob);
	}
	if( type == SOCK_UNIX )
	{
	    if( write(sock_fd,obuf,len_ob) == -1 )
		if( connect(sock_fd, (sockaddr *)&name_un, sizeof(name_un)) == -1 )
		    throw TError("%s: %s connect to UNIX error!",NAME_MODUL,Name().c_str());
		else write(sock_fd,obuf,len_ob);
	}
	if( type == SOCK_UDP )
	{
	    if( connect(sock_fd, (sockaddr *)&name_in, sizeof(name_in)) == -1 )
		throw TError("%s: %s connect to UDP error!",NAME_MODUL,Name().c_str());
	    write(sock_fd,obuf,len_ob);
	}
    }

    int i_b = 0;
    if( ibuf != NULL && len_ib > 0 && time > 0 )
    {
	fd_set rd_fd;
    	struct timeval tv;

	tv.tv_sec  = time;
	tv.tv_usec = 0;
	FD_ZERO(&rd_fd);
	FD_SET(sock_fd,&rd_fd);
	int kz = select(sock_fd+1,&rd_fd,NULL,NULL,&tv);
	if( kz < 0) throw TError("%s: socket error!",NAME_MODUL);
	if( kz == 0 || (kz == -1 && errno == EINTR) ) i_b = 0;
	else if( FD_ISSET(sock_fd, &rd_fd) ) i_b = read(sock_fd,ibuf,len_ib);
	else throw TError("%s: timeout error!",NAME_MODUL);
    }

    return(i_b);
}

