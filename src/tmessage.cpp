#include <sys/types.h>

#include <syslog.h>
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include <iconv.h>
#include <langinfo.h>
#include <getopt.h>
#include <stdlib.h>

#include "terror.h"
#include "tsys.h"
#include "tarhives.h"
#include "tmessage.h"

const char *TMessage::o_name = "TMessage";

TMessage::TMessage(  ) : IOCharSet("UTF8"), d_level(8), log_dir(2), head_buf(0)
{
    openlog("OpenScada",0,LOG_USER);
    SetCharset(nl_langinfo(CODESET));
    
    SBufRec buf_r = { 0 };
    int i_buf = 10;
    while( i_buf--) m_buf.push_back(buf_r);
}


TMessage::~TMessage(  )
{
    closelog();
}


// ������� ������� (d_level) ����� ���������� � �������� 0-8 ������������:
// 0 - �� �������� ������� ��������� ������ 
// 8 - ������������ ������� �������
// ������� ��������� (level) ������������� ��� �������������� � ���������� � �������� 0-7:
// 0 - ���������� �������;
// 7 - ������� ���������� ������� ������;
/*
void TMessage::put( int level, char *fmt,  ... )
{
    char str[256];                  //!!!!
    va_list argptr;

    va_start (argptr,fmt);
    vsprintf(str,fmt,argptr);
    va_end(argptr);
    put("SYS",level,str);
}
*/
void TMessage::put( string categ, int level, char *fmt,  ... )
{
    char str[1024];                  //!!!!
    va_list argptr;

    va_start (argptr,fmt);
    vsprintf(str,fmt,argptr);
    va_end(argptr);
    if(level<0) level=0; if(level>7) level=7;
    if(level>=(8-d_level)) 
    {
	int level_sys=LOG_DEBUG;
	if(level<1)       level_sys=LOG_DEBUG;
	else if(level==1) level_sys=LOG_INFO;
	else if(level==2) level_sys=LOG_NOTICE;
	else if(level==3) level_sys=LOG_WARNING;
	else if(level==4) level_sys=LOG_ERR;
	else if(level==5) level_sys=LOG_CRIT;
	else if(level==6) level_sys=LOG_ALERT;
	else if(level==7) level_sys=LOG_EMERG;
	string s_mess = categ + "| " + str;
	if(log_dir&1) syslog(level_sys,s_mess.c_str());
	if(log_dir&2) fprintf(stdout,"%s \n",s_mess.c_str());
	if(log_dir&4) fprintf(stderr,"%s \n",s_mess.c_str());
	m_buf[head_buf].time  = time(NULL);
	m_buf[head_buf].categ = categ;
	m_buf[head_buf].level = level;
	m_buf[head_buf].mess  = str;
	if( ++head_buf >= m_buf.size() ) head_buf = 0;
    }
}

void TMessage::GetMess( time_t b_tm, time_t e_tm, vector<SBufRec> & recs, string category, char level )
{
    recs.clear();

    int i_buf = head_buf;
    while(true)
    {
	if( m_buf[i_buf].time >= b_tm && m_buf[i_buf].time != 0 && m_buf[i_buf].time < e_tm &&
		( !category.size() || category == m_buf[i_buf].categ ) && m_buf[i_buf].level >= level )
	    recs.push_back(m_buf[i_buf]);
	if( ++i_buf >= m_buf.size() ) i_buf = 0;
    	if(i_buf == head_buf) break;	    
    }
}

int TMessage::SconvIn(const char *fromCH, string & buf)
{
    return( Sconv(fromCH, IOCharSet.c_str(), buf) );
}    
int TMessage::SconvOut(const char *toCH, string & buf)
{
    return( Sconv( IOCharSet.c_str(), toCH , buf) );
}

int TMessage::Sconv(const char *fromCH, const char *toCH, string & buf)
{
    //Make convert to blocks 100 bytes !!!    
    char   *ibuf, outbuf[100], *obuf;
    size_t ilen, olen;
    iconv_t hd;
    
    hd = iconv_open(toCH, fromCH);
    if( hd == (iconv_t)(-1) ) return(-1);
    
    string t_buf = buf; 
    buf.erase();
    ibuf = (char *)t_buf.c_str();
    ilen = t_buf.size();
    
    while(ilen)
    {
	obuf = outbuf;
	olen = sizeof(outbuf)-1;
	iconv(hd,&ibuf,&ilen,&obuf,&olen);
	buf.append(outbuf,sizeof(outbuf)-1-olen);
    }
    iconv_close(hd);
    
    return(0);
}

void TMessage::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "============================ Message options ==============================\n"
    "-d, --debug=<level>    Set <level> debug (0-8);\n"
    "    --log=<direct>     Set direction a log and other info;\n"
    "                         <direct> & 1 - syslogd;\n"
    "                         <direct> & 2 - stdout;\n"
    "                         <direct> & 4 - stderr;\n"
    "    --IOCharset=<name> Set io charset;\n"
    "------------------ Section fields of config file --------------------\n"
    "debug      = <level>     set <level> debug (0-8);\n"
    "target_log = <direction> set direction a log and other info;\n"
    "                           <direct> & 1 - syslogd;\n"
    "                           <direct> & 2 - stdout;\n"
    "                           <direct> & 4 - stderr;\n"
    "io_chrset  = <charset>   set io charset;\n"
    "mess_buf   = <len>       set messages buffer len;\n"
    "\n");
}

void TMessage::CheckCommandLine( )
{
#if OSC_DEBUG
    Mess->put("DEBUG",MESS_INFO,"%s: Read commandline options!",o_name);
#endif

    int i,next_opt;
    char *short_opt="hd:";
    struct option long_opt[] =
    {
	{"help"     ,0,NULL,'h'},
	{"debug"    ,1,NULL,'d'},
	{"log"      ,1,NULL,'l'},
	{"IOCharset",1,NULL,'c'},
	{NULL       ,0,NULL,0  }
    };

    optind=opterr=0;
    do
    {
	next_opt=getopt_long(SYS->argc,(char * const *)SYS->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': pr_opt_descr(stdout); break;
	    case 'd': i = atoi(optarg); if(i>=0&&i<=8) SetDLevel(i); break;
	    case 'l': SetLogDir(atoi(optarg)); break;
	    case 'c': SetCharset(optarg); break;
	    case -1 : break;
	}
    } while(next_opt != -1);
    
#if OSC_DEBUG
    Mess->put("DEBUG",MESS_DEBUG,"%s: Read commandline options ok!",o_name);
#endif
}

void TMessage::UpdateOpt()
{
#if OSC_DEBUG
    Mess->put("DEBUG",MESS_INFO,"%s: Read config options!",o_name);
#endif

    string opt;

    try
    {
	int i = atoi(SYS->XMLCfgNode()->get_child("debug")->get_text().c_str());
	if( i >= 0 && i <= 8 ) SetDLevel(i);
    //}catch(...) {  }
    }catch( TError err ) { put("SYS",MESS_DEBUG,"MESS:%s",err.what().c_str()); }
    try{ SetLogDir(atoi(SYS->XMLCfgNode()->get_child("target_log")->get_text().c_str())); }
    catch(...) { }
    try{ SetCharset(SYS->XMLCfgNode()->get_child("io_charset")->get_text()); }
    catch(...) { }    
    try
    { 
	int len = atoi( SYS->XMLCfgNode()->get_child("mess_buf")->get_text().c_str() ); 
	while( m_buf.size() > len )
	{
	    m_buf.erase( m_buf.begin() + head_buf );
	    if( head_buf >= m_buf.size() ) head_buf = 0;
	}
	while( m_buf.size() < len )
	    m_buf.insert( m_buf.begin() + head_buf );
    }
    catch(...) { }    
    
#if OSC_DEBUG
    Mess->put("DEBUG",MESS_INFO,"%s: Read config options ok!",o_name);
#endif
}

