
#include <unistd.h>
#include <getopt.h>
#include <signal.h>

#include "tsys.h"
#include "tbds.h"
#include "tkernel.h"
#include "tarhives.h"

//================================================================
//=============== TArhiveS =======================================
//================================================================
SCfgFld TArhiveS::gen_elem[] =
{
    {"NAME"    ,"Arhive name."         ,CFG_T_STRING              ,"","",""           ,"20",""          ,"%s"},
    {"DESCRIPT","Arhive description."  ,CFG_T_STRING              ,"","",""           ,"50",""          ,"%s"},
    {"MODUL"   ,"Module(plugin) name." ,CFG_T_STRING              ,"","",""           ,"20",""          ,"%s"},
    {"CATEG"   ,"Category for MESS."   ,CFG_T_STRING              ,"","",""           ,"20",""          ,"%s"},
    {"ADDR"    ,"Arhive address."      ,CFG_T_STRING              ,"","",""           ,"20",""          ,"%s"},
    {"TYPE"    ,"Type arhive."         ,CFG_T_INT|CFG_T_SELECT    ,"","","0"          ,"1" ,"0;1"       ,"%s","Message;Value"},
    {"STAT"    ,"Arhive stat."         ,CFG_T_BOOLEAN|CFG_T_SELECT,"","","false"      ,"1" ,"false;true","%s","Disable;Enable"}
};

const char *TArhiveS::o_name = "TArhiveS";

TArhiveS::TArhiveS( TKernel *app ) : TGRPModule(app,"Arhiv"), TConfig(NULL), m_mess_r_stat(false), m_mess_per(2)
{
    hd_res = SYS->ResCreate();
    
    for(unsigned i = 0; i < sizeof(gen_elem)/sizeof(SCfgFld); i++) 
	cf_ConfElem()->cfe_Add(&gen_elem[i]);
}

TArhiveS::~TArhiveS(  )
{
    if( m_mess_r_stat )
    {
	m_mess_r_endrun = true;
	pthread_kill( m_mess_pthr,SIGALRM );
	sleep(1);
	while( m_mess_r_stat )
	{
	    Mess->put("SYS",MESS_CRIT,"%s: Thread no stoped!",o_name);
	    sleep(1);
    	}
    }
    
    SYS->WResRequest(hd_res);	
    for(unsigned i_arh = 0; i_arh < m_mess.size(); i_arh++)
	if( m_mess[i_arh].use )
	{
	    at_tp(m_mess[i_arh].type).CloseMess(m_mess[i_arh].obj);
	    m_mess[i_arh].use = false;
	}
    SYS->WResRelease(hd_res);	
    
    SYS->ResDelete(hd_res);    
}

void TArhiveS::gmd_Init( )
{
    LoadBD();
}

void TArhiveS::pr_opt_descr( FILE * stream )
{
    fprintf(stream,
    "========================= %s options ================================\n"
    "    --ArhPath = <path>  set modules <path>;\n"
    "------------------ Section fields of config file --------------------\n"
    " mod_path = <path>      set modules <path>;\n"	   
    " GenBD    = <fullname>  generic bd recorded: \"<TypeBD>:<NameBD>:<NameTable>\";\n"
    " mess_period = <per>    set message arhiving period;\n"
    " MESS id=<name> type=<module> addr=<addr> cat=<categ>\n"
    "                        add message arhivator <name> <module> <addr> <categ> and description into text;\n"
    "    name   - name transport;\n"
    "    module - module transport;\n"
    "    addr   - addres transport;\n"
    "    categ  - message categories (TEST;DEBUG);\n"
    "\n",gmd_Name().c_str());
}

void TArhiveS::gmd_CheckCommandLine( )
{
    TGRPModule::gmd_CheckCommandLine( );

    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{"ArhPath" ,1,NULL,'m'},
	{NULL      ,0,NULL,0  }
    };

    optind=0,opterr=0;	
    do
    {
	next_opt=getopt_long(SYS->argc,(char * const *)SYS->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': pr_opt_descr(stdout); break;
	    case 'm': DirPath = optarg;     break;
	    case -1 : break;
	}
    } while(next_opt != -1);
}

void TArhiveS::gmd_UpdateOpt()
{
    TGRPModule::gmd_UpdateOpt();
    
    try{ DirPath = gmd_XMLCfgNode()->get_child("mod_path")->get_text(); }
    catch(...) {  }
    try{ m_mess_per = atoi( gmd_XMLCfgNode()->get_child("mess_period")->get_text().c_str() ); }
    catch(...) {  }
    try
    {
    	string opt = gmd_XMLCfgNode()->get_child("GenBD")->get_text(); 
	int pos = 0;
	t_bd = opt.substr(pos,opt.find(":",pos)-pos); pos = opt.find(":",pos)+1;
	n_bd = opt.substr(pos,opt.find(":",pos)-pos); pos = opt.find(":",pos)+1;
	n_tb = opt.substr(pos,opt.find(":",pos)-pos); pos = opt.find(":",pos)+1;
	if( !t_bd.size() ) t_bd = Owner().DefBDType;
	if( !n_bd.size() ) n_bd = Owner().DefBDName;
    }
    catch(...) {  }
    while(cf_Size()) cf_FreeRecord(0);
    try
    {
    	int i = 0;
    	while( true )
	{
	    XMLNode *t_n = gmd_XMLCfgNode()->get_child("MESS",i++);
	    int rec = cf_AddRecord( cf_Size() );
	    cf_Set_S("NAME"    , t_n->get_attr("id")  , rec);
	    cf_Set_S("DESCRIPT", t_n->get_text()      , rec);
	    cf_Set_S("MODUL"   , t_n->get_attr("type"), rec);
	    cf_Set_S("ADDR"    , t_n->get_attr("addr"), rec);
	    cf_Set_S("CATEG"   , t_n->get_attr("cat") , rec);
	    cf_Set_SEL("TYPE"  , "Message", rec);
	    cf_Set_SEL("STAT"  , "Enable", rec);
	}	
    }
    catch(...) {  }    
}

void TArhiveS::LoadBD( )
{    
    try
    {
	int b_hd = Owner().BD().OpenTable(t_bd,n_bd,n_tb);
	cf_LoadAllValBD( Owner().BD().at_tbl(b_hd) );
	cf_FreeDubl("NAME",false);   //Del new (from bd)
	Owner().BD().CloseTable(b_hd);
    }catch(TError err) { Mess->put("SYS",MESS_ERR,"%s:%s",o_name,err.what().c_str()); }    
    
    //Open transports (open new transports)
    for(unsigned i_cfg = 0; i_cfg < cf_Size(); i_cfg++)
    {
	if( cf_Get_SEL("TYPE", i_cfg) == "Message" && cf_Get_SEL("STAT", i_cfg) == "Enable" )
	{
	    //Check avoid transport
	    try{ MessNameToId( cf_Get_S("NAME", i_cfg) ); }
	    catch(...)
	    {
		try 
		{ 
		    MessOpen( cf_Get_S("NAME", i_cfg), cf_Get_S("MODUL", i_cfg), cf_Get_S("ADDR", i_cfg), cf_Get_S("CATEG", i_cfg) ); 
		}
		catch(TError err) { Mess->put("SYS",MESS_ERR,"%s:%s",o_name,err.what().c_str()); }
	    }
	}
	else if( cf_Get_SEL("TYPE", i_cfg) == "Value" && cf_Get_SEL("STAT", i_cfg) == "Enable" )
	{
	    //Check avoid transport
	    try{ ValNameToId( cf_Get_S("NAME", i_cfg) ); }
	    catch(...)
	    {
    		try 
    		{ 
		    ValOpen( cf_Get_S("NAME", i_cfg), cf_Get_S("MODUL", i_cfg), cf_Get_S("ADDR", i_cfg) ); 
    		}
    		catch(TError err) { Mess->put("SYS",MESS_ERR,"%s:%s",o_name,err.what().c_str()); }
	    }
	}
    }
    //Close no avoid in bd transports    
}

void TArhiveS::UpdateBD( )
{
    int b_hd;
    try{ b_hd = Owner().BD().OpenTable(t_bd,n_bd,n_tb); }
    catch(...) { b_hd = Owner().BD().OpenTable(t_bd,n_bd,n_tb,true); }
    cf_ConfElem()->cfe_UpdateBDAttr( Owner().BD().at_tbl(b_hd) );
    cf_SaveAllValBD( Owner().BD().at_tbl(b_hd) );
    Owner().BD().at_tbl(b_hd).Save();
    Owner().BD().CloseTable(b_hd);
}
		    
int TArhiveS::MessOpen( string name, string t_name, string addr, string categor )
{    
    try{ MessNameToId( name ); }
    catch(...)
    {
    	SArhive n_obj;
	n_obj.use  = true;
	n_obj.type = gmd_NameToId(t_name);
	n_obj.obj  = at_tp(n_obj.type).OpenMess(name,addr,categor);
	
    	SYS->WResRequest(hd_res);	
	unsigned id;
	for( id = 0; id < m_mess.size(); id++ )
	    if( !m_mess[id].use ) break;
	if( id == m_mess.size() ) m_mess.push_back(n_obj);
	else                      m_mess[id] = n_obj;	
    	SYS->WResRelease(hd_res);
	return(id);
    }
    throw TError("%s: Message arhive %s already open!",o_name,name.c_str());
}

void TArhiveS::MessClose( unsigned int id )
{
    SYS->WResRequest(hd_res);	
    if(id > m_mess.size() || !m_mess[id].use )
    {
    	SYS->WResRelease(hd_res);
	throw TError("%s: Message arhive identificator error!",o_name);
    }
    at_tp(m_mess[id].type).CloseMess(m_mess[id].obj);
    m_mess[id].use = false;
    SYS->WResRelease(hd_res);
}

unsigned TArhiveS::MessNameToId( string name )
{
    SYS->RResRequest(hd_res);	
    for(unsigned i_arh=0; i_arh < m_mess.size(); i_arh++)
	if( m_mess[i_arh].use && Mess_at(i_arh)->Name() == name )
	{
	    SYS->RResRelease(hd_res);	
    	    return(i_arh);
	}
    SYS->RResRelease(hd_res);	
    throw TError("%s: Message arhive %s no avoid!",o_name,name.c_str());
}

TArhiveMess *TArhiveS::Mess_at( unsigned int id )
{
    SYS->RResRequest(hd_res);	
    if(id > m_mess.size() || !m_mess[id].use ) 
    {
    	SYS->RResRelease(hd_res);	
	throw TError("%s: Message arhive identificator error!",o_name);
    }
    SYS->RResRelease(hd_res);	
    return(at_tp(m_mess[id].type).atMess(m_mess[id].obj));
}

void TArhiveS::MessList( vector<string> &list )
{
    SYS->RResRequest(hd_res);	
    list.clear();
    for(unsigned id=0;id < m_mess.size(); id++)
	if( m_mess[id].use ) list.push_back( Mess_at(id)->Name() );
    SYS->RResRelease(hd_res);	
}

int TArhiveS::ValOpen( string name, string t_name, string bd )
{    
    try{ ValNameToId( name ); }
    catch(...)
    {
    	SArhive n_obj;
	n_obj.use  = true;
	n_obj.type = gmd_NameToId(t_name);
	n_obj.obj  = at_tp(n_obj.type).OpenVal(name,bd);
	
	unsigned id;
	for( id = 0; id < m_val.size(); id++ )
	    if( !m_val[id].use ) break;
	if( id == m_val.size() ) m_val.push_back(n_obj);
	else                     m_val[id] = n_obj;
	
	return(id);
    }
    throw TError("%s: Value arhive %s already open!",o_name,name.c_str());
}

void TArhiveS::ValClose( unsigned int id )
{
    if(id > m_val.size() || !m_val[id].use ) 
	throw TError("%s: Value arhive identificator error!",o_name);
    at_tp(m_val[id].type).CloseVal(m_val[id].obj);
    m_val[id].use = false;
}

unsigned TArhiveS::ValNameToId( string name )
{
    for(unsigned i_arh=0; i_arh < m_val.size(); i_arh++)
	if( m_val[i_arh].use && Val_at(i_arh)->Name() == name ) return(i_arh);
    throw TError("%s: Value arhive %s no avoid!",o_name,name.c_str());
}

TArhiveVal *TArhiveS::Val_at( unsigned int id )
{
    if(id > m_val.size() || !m_val[id].use ) 
	throw TError("%s: Value arhive identificator error!",o_name);
    return(at_tp(m_val[id].type).atVal(m_val[id].obj));
}

void TArhiveS::ValList( vector<string> &list )
{
    list.clear();
    for(unsigned id=0;id < m_val.size(); id++)
	if( m_val[id].use ) list.push_back( Val_at(id)->Name() );
}

void TArhiveS::gmd_Start( )
{    
    pthread_attr_t      pthr_attr;
    if( m_mess_r_stat ) return; 

    pthread_attr_init(&pthr_attr);
    pthread_attr_setschedpolicy(&pthr_attr,SCHED_OTHER);
    pthread_create(&m_mess_pthr,&pthr_attr,TArhiveS::MessArhTask,this);
    pthread_attr_destroy(&pthr_attr);
    sleep(1);
    if( !m_mess_r_stat ) throw TError("%s: Task of Messages arhivator no starting!",o_name);    
}

void TArhiveS::gmd_Stop( )
{    
    if( m_mess_r_stat )
    {
    	m_mess_r_endrun = true;
	pthread_kill(m_mess_pthr,SIGALRM);
	sleep(1);
	if( m_mess_r_stat ) throw TError("%s: Task of Messages arhivator no stoping!",o_name);
    }
}

void *TArhiveS::MessArhTask(void *param)
{
    int i_cnt = 0;
    TArhiveS *arh = (TArhiveS *)param;
    vector<SBufRec> i_mess, o_mess;    
    time_t t_last = 0, t_cur;

    struct sigaction sa;
    memset (&sa, 0, sizeof(sa));
    sa.sa_handler= SYS->sighandler;
    sigaction(SIGALRM,&sa,NULL);

    arh->m_mess_r_stat = true;
    arh->m_mess_r_endrun = false;
    
    while( !arh->m_mess_r_endrun )
    {	
	if( ++i_cnt > arh->m_mess_per)
	{
	    i_cnt = 0;
    	    try
    	    {
    		t_cur = time(NULL);
    		Mess->GetMess( t_last, t_cur, i_mess );
		if( i_mess.size() )
		{
		    t_last = i_mess[i_mess.size()-1].time+1;    		
		    SYS->RResRequest(arh->hd_res);	
		    for( unsigned i_am = 0; i_am < arh->m_mess.size(); i_am++ )
			if( arh->m_mess[i_am].use ) 
			{
			    o_mess.clear();
			    for(unsigned i_m = 0; i_m < i_mess.size(); i_m++)
			    {
				unsigned i_cat;
				vector<string> &categ = arh->Mess_at(i_am)->Categ();
				for( i_cat = 0; i_cat < categ.size(); i_cat++ )
				    if( categ[i_cat] == i_mess[i_m].categ ) break;
				if( i_cat < categ.size() || !categ.size() ) 
				    o_mess.push_back(i_mess[i_m]);
			    }
			    if( o_mess.size() ) arh->Mess_at(i_am)->put(o_mess);
			}
    		    SYS->RResRelease(arh->hd_res);			
		}
    	    }
    	    catch(TError err){ Mess->put("SYS",MESS_ERR,"%s:%s",o_name,err.what().c_str()); }
	}	
	sleep(1);
    }
    
    arh->m_mess_r_stat = false;
}

void TArhiveS::gmd_DelM( unsigned hd )
{
    SYS->WResRequest(hd_res);	
    for(unsigned i_m = 0; i_m < m_mess.size(); i_m++)
	if( m_mess[i_m].use && m_mess[i_m].type == hd )
	{
	    at_tp(m_mess[i_m].type).CloseMess(m_mess[i_m].obj);
	    m_mess[i_m].use = false;
	}
    SYS->WResRelease(hd_res);	
	    
    TGRPModule::gmd_DelM( hd );
}

//================================================================
//=========== TTipArhive =========================================
//================================================================
const char *TTipArhive::o_name = "TTipArhive";

TTipArhive::TTipArhive()
{
    hd_res = SYS->ResCreate();
}
    
TTipArhive::~TTipArhive()
{
    SYS->ResDelete(hd_res);    
}

unsigned TTipArhive::OpenMess(string name, string addr, string categories )
{
    unsigned id;
    
    TArhiveMess *a_mess = AMess(name, addr, categories);
    
    SYS->WResRequest(hd_res);
    for(id=0; id < m_mess.size(); id++) 
	if( m_mess[id] == TO_FREE ) break;
    if( id == m_mess.size() ) m_mess.push_back(a_mess);
    else                      m_mess[id] = a_mess;
    SYS->WResRelease(hd_res);    
    
    //throw TError("TEST");

    return(id);
}

void TTipArhive::CloseMess( unsigned int id )
{
    SYS->WResRequest(hd_res);
    if(id > m_mess.size() || m_mess[id] == TO_FREE )
    {
	SYS->WResRelease(hd_res);
	throw TError("%s: Message arhive id=%d - error!",o_name,id);
    }
    delete m_mess[id];  
    m_mess[id] = TO_FREE;
    SYS->WResRelease(hd_res);
}

TArhiveMess *TTipArhive::atMess( unsigned int id )
{
    SYS->RResRequest(hd_res);
    if(id > m_mess.size() || m_mess[id] == TO_FREE ) 
    {
    	SYS->RResRelease(hd_res);
	throw TError("%s: Message arhive id=%d - error!",o_name,id);
    }
    SYS->RResRelease(hd_res);
    return(m_mess[id]);
}

unsigned TTipArhive::OpenVal(string name, string bd )
{
    unsigned id;
    
    TArhiveVal *a_val = AVal(name,bd);
    SYS->WResRequest(hd_res);
    for(id=0; id < m_val.size(); id++) 
	if(m_val[id] == TO_FREE) break;
    if(id == m_val.size()) m_val.push_back(a_val);
    else                   m_val[id] = a_val;
    SYS->WResRelease(hd_res);
    
    return(id);
}

void TTipArhive::CloseVal( unsigned int id )
{
    SYS->WResRequest(hd_res);
    if(id > m_val.size() || m_val[id] == TO_FREE ) 
    {
	SYS->WResRelease(hd_res);	
	throw TError("%s: Value arhiv id=%d - error!",o_name,id);
    }
    delete m_val[id];    
    m_val[id] = TO_FREE;
    SYS->WResRelease(hd_res);	
}

TArhiveVal *TTipArhive::atVal( unsigned int id )
{
    SYS->RResRequest(hd_res);
    if(id > m_val.size() || m_val[id] == TO_FREE ) 
    {
    	SYS->RResRelease(hd_res);
	throw TError("%s: Value arhive id=%d - error!",o_name,id);
    }
    SYS->RResRelease(hd_res);
    return(m_val[id]);
}

//================================================================
//=========== TArhiveMess ========================================
//================================================================
const char *TArhiveMess::o_name = "TArhiveMess";

TArhiveMess::TArhiveMess(string name, string addr, string categoris, TTipArhive *owner) : 
	m_name(name), m_addr(addr), m_cat_o(categoris), m_owner(owner) 
{     
    int pos = 0;
    
    while( true )
    {    
    	m_categ.push_back( categoris.substr(pos,categoris.find(";",pos)-pos) ); 
	pos = categoris.find(";",pos);
       	if( pos++ == string::npos ) break;
    }    
};

TArhiveMess::~TArhiveMess()
{
    
}

//================================================================
//=========== TArhiveVal =========================================
//================================================================
const char *TArhiveVal::o_name = "TArhiveVal";

TArhiveVal::~TArhiveVal()
{
    
}

