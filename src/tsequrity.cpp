/***************************************************************************
 *   Copyright (C) 2004 by Roman Savochenko                                *
 *   rom_as@fromru.com                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <getopt.h>

#include "xml.h"
#include "tsys.h"
#include "tmessage.h"
#include "tsequrity.h"

const char *TSequrity::o_name = "TSequrity";
const char *TSequrity::s_name = "Sequrity";

TSequrity::TSequrity( TKernel *app ) : 
    user_el(""), grp_el(""), owner(app), m_hd_usr(o_name), m_hd_grp(o_name), 
    m_bd_usr("", "", "seq_usr.dbf"), m_bd_grp("", "", "seq_grp.dbf")
{
    SFld gen_elem[] =
    {
	{"NAME" ,Mess->I18N("Name")         ,T_STRING,"","20"},
	{"DESCR",Mess->I18N("Full name")    ,T_STRING,"","50"},    
	{"ID"   ,Mess->I18N("Identificator"),T_DEC   ,"","3" }
    };

    SFld user_elem[] =
    {
	{"PASS",Mess->I18N("User password")     ,T_STRING,"","20"},
	{"GRP" ,Mess->I18N("User default group"),T_STRING,"","20"}
    };

    SFld grp_elem[] =
    {
	{"USERS",Mess->I18N("Users in group"),T_STRING,"","50"}
    };

    // Fill users elements
    for(unsigned i = 0; i < sizeof(gen_elem)/sizeof(SFld); i++)  user_el.elAdd(&gen_elem[i]);
    for(unsigned i = 0; i < sizeof(user_elem)/sizeof(SFld); i++) user_el.elAdd(&user_elem[i]);
    // Fill groups elements
    for(unsigned i = 0; i < sizeof(gen_elem)/sizeof(SFld); i++) grp_el.elAdd(&gen_elem[i]);
    for(unsigned i = 0; i < sizeof(grp_elem)/sizeof(SFld); i++) grp_el.elAdd(&grp_elem[i]);
	
    //Add surely users, groups and set parameters
    usr_add("root");
    usr_at("root").at().Descr("Administrator (superuser)!!!"); 
    usr_at("root").at().Pass("openscada"); 
    
    grp_add("root");
    grp_at("root").at().Descr("Administrators group.");     
}

TSequrity::~TSequrity(  )
{
    vector<string> list;

    //Free groups
    m_hd_grp.lock();
    grp_list(list);
    for( unsigned i_ls = 0; i_ls < list.size(); i_ls++)
        grp_del(list[i_ls]);    	
    //Free users list
    m_hd_usr.lock();
    usr_list(list);
    for( unsigned i_ls = 0; i_ls < list.size(); i_ls++)
	usr_del(list[i_ls]);    
}
	
string TSequrity::Name()
{
    return(Mess->I18N((char *)s_name)); 
}

void TSequrity::usr_add( const string &name )
{    
    TUser *user = new TUser(this,name,usr_id_f(),&user_el);
    try{ m_hd_usr.obj_add( user, &user->Name() ); }
    catch(TError err) {	delete user; }
}

void TSequrity::grp_add( const string &name )
{
    TGroup *grp = new TGroup(this,name,grp_id_f(),&grp_el);
    try{ m_hd_grp.obj_add( grp, &grp->Name() ); }
    catch(TError err) {	delete grp; }
}

unsigned TSequrity::usr_id_f()
{
    unsigned id = 0;
    vector<string> list;
    usr_list(list); 
    for( int i_l = 0; i_l < list.size(); i_l++ )
	if( usr_at(list[i_l]).at().Id() == id ){ id++; i_l=-1; }
    return(id);
}

unsigned TSequrity::grp_id_f()
{
    unsigned id = 0;
    vector<string> list;
    grp_list(list); 
    for( int i_l = 0; i_l < list.size(); i_l++ )
	if( grp_at(list[i_l]).at().Id() == id ){ id++; i_l=-1; }
    return(id);
}

string TSequrity::usr( int id )
{
    vector<string> list;
    
    usr_list(list); 
    for( int i_l = 0; i_l < list.size(); i_l++ )
	if( usr_at(list[i_l]).at().Id() == id ) return(list[i_l]);
    return("");
}

string TSequrity::grp( int id )
{
    vector<string> list;
    
    grp_list(list); 
    for( int i_l = 0; i_l < list.size(); i_l++ )
	if( grp_at(list[i_l]).at().Id() == id ) return(list[i_l]);
    return("");
}

bool TSequrity::access( const string &user, char mode, int owner, int group, int access )
{
    bool rez = false;

    try
    {
    	AutoHD<TUser> r_usr = usr_at(user);
	// Check owner permision
	if( r_usr.at().Id() == 0 || r_usr.at().Id() == owner )
	    if( ((mode&SEQ_RD)?access&0400:true) && 
		((mode&SEQ_WR)?access&0200:true) && 
		((mode&SEQ_XT)?access&0100:true) )
	    rez = true; 
	// Check other permision
	if( !rez && ((mode&SEQ_RD)?access&0004:true) && 
	    ((mode&SEQ_WR)?access&0002:true) && 
	    ((mode&SEQ_XT)?access&0001:true) )
	    rez = true; 	
	// Check groupe permision
	if( !rez )
	{
	    string n_grp = grp(group);
	    if( n_grp.size() )
	    {
		if( (n_grp == r_usr.at().Grp() || grp_at(n_grp).at().user(user)) &&
		    ((mode&SEQ_RD)?access&0040:true) && 
		    ((mode&SEQ_WR)?access&0020:true) && 
		    ((mode&SEQ_XT)?access&0010:true) )
		    rez = true;
	    }
	}	
    }catch(...){  }

    return(rez);
}

XMLNode *TSequrity::XMLCfgNode()
{
    int i_k = 0;
    while(true)
    {
	XMLNode *t_n = Owner().XMLCfgNode()->get_child("section",i_k++);
	if( t_n->get_attr("id") == s_name ) return( t_n );
    }
}

void TSequrity::Init( )
{
    LoadBD();
}

string TSequrity::opt_descr( )
{
    char buf[STR_BUF_LEN];
    snprintf(buf,sizeof(buf),Mess->I18N(
	"======================= The Sequrity subsystem options =====================\n"
	"------------ Parameters of section <%s> in config file -----------\n"
	"UserBD  <fullname>  User bd, recorded:  \"<TypeBD>:<NameBD>:<NameTable>\";\n"
	"GrpBD   <fullname>  Group bd, recorded: \"<TypeBD>:<NameBD>:<NameTable>\";\n\n"),s_name);
    
    return(buf);
}

void TSequrity::CheckCommandLine(  )
{
#if OSC_DEBUG
    Owner().m_put("DEBUG",MESS_INFO,"%s:Read commandline options!",s_name);
#endif
	
    int next_opt;
    char *short_opt="h";
    struct option long_opt[] =
    {
	{"help"     ,0,NULL,'h'},
	{NULL       ,0,NULL,0  }
    };

    optind=opterr=0;	 
    do
    {
	next_opt=getopt_long(SYS->argc,( char *const * ) SYS->argv,short_opt,long_opt,NULL);
	switch(next_opt)
	{
	    case 'h': fprintf(stdout,opt_descr().c_str()); break;
	    case -1 : break;
	}
    } while(next_opt != -1);
    
#if OSC_DEBUG
    Owner().m_put("DEBUG",MESS_DEBUG,"%s:Read commandline options ok!",s_name);
#endif
}

void TSequrity::UpdateOpt()
{
    string opt;

    try
    {
    	opt = XMLCfgNode()->get_child("id","UserBD")->get_text(); 
	int pos = 0;
        m_bd_usr.tp  = opt.substr(pos,opt.find(":",pos)-pos); pos = opt.find(":",pos)+1;
        m_bd_usr.bd  = opt.substr(pos,opt.find(":",pos)-pos); pos = opt.find(":",pos)+1;
	m_bd_usr.tbl = opt.substr(pos,opt.find(":",pos)-pos); pos = opt.find(":",pos)+1;
    }
    catch(...) {  }    
    if( !m_bd_usr.tp.size() ) m_bd_usr.tp = Owner().DefBDType;
    if( !m_bd_usr.bd.size() ) m_bd_usr.bd = Owner().DefBDName;
    
    try
    {
    	opt = XMLCfgNode()->get_child("id","GrpBD")->get_text(); 
	int pos = 0;
        m_bd_grp.tp  = opt.substr(pos,opt.find(":",pos)-pos); pos = opt.find(":",pos)+1;
        m_bd_grp.bd  = opt.substr(pos,opt.find(":",pos)-pos); pos = opt.find(":",pos)+1;
	m_bd_grp.tbl = opt.substr(pos,opt.find(":",pos)-pos); pos = opt.find(":",pos)+1;
    }
    catch(...) {  }    
    if( !m_bd_grp.tp.size() ) m_bd_grp.tp = Owner().DefBDType;
    if( !m_bd_grp.bd.size() ) m_bd_grp.bd = Owner().DefBDName;
}

void TSequrity::LoadBD( )
{
    TConfig *c_el;
    string name,type;
    SHDBD b_hd;    

    // Load user bd
    try
    {
	b_hd = Owner().BD().open( m_bd_usr );    
	for( int i_ln = 0; i_ln < Owner().BD().at(b_hd).NLines(); i_ln++ )
	{	    
	    c_el = new TConfig(&user_el);
	    c_el->cfLoadValBD(i_ln,Owner().BD().at(b_hd));
	    name = c_el->cfg("NAME").getS();
	    delete c_el;	
	    	    
	    try{usr_add(name);}catch(...){}
	    usr_at(name).at().cfLoadValBD("NAME",Owner().BD().at(b_hd));
	}
	Owner().BD().close(b_hd);
    }catch(...){}
    
    // Load group bd
    try
    {
	b_hd = Owner().BD().open( m_bd_grp );    
	for( int i_ln = 0; i_ln < Owner().BD().at(b_hd).NLines(); i_ln++ )
	{
	    c_el = new TConfig(&grp_el);
	    c_el->cfLoadValBD(i_ln,Owner().BD().at(b_hd));
	    name = c_el->cfg("NAME").getS();
	    delete c_el;	
	    
	    try{grp_add(name);}catch(...){}
	    grp_at(name).at().cfLoadValBD("NAME",Owner().BD().at(b_hd));
	}
	Owner().BD().close(b_hd);
    }catch(...){}
}

void TSequrity::UpdateBD( )
{
    vector<string> list;
    SHDBD b_hd;
    
    // Save user bd
    b_hd = Owner().BD().open( m_bd_usr, true );
    Owner().BD().at(b_hd).Clean();
    user_el.elUpdateBDAttr( Owner().BD().at(b_hd) );
    usr_list(list);
    for( int i_l = 0; i_l < list.size(); i_l++ )
	usr_at(list[i_l]).at().cfSaveValBD("NAME",Owner().BD().at(b_hd));
    Owner().BD().at(b_hd).Save();
    Owner().BD().close(b_hd);
    // Save group bd
    b_hd = Owner().BD().open( m_bd_grp, true );
    Owner().BD().at(b_hd).Clean();
    grp_el.elUpdateBDAttr( Owner().BD().at(b_hd) );
    grp_list(list);
    for( int i_l = 0; i_l < list.size(); i_l++ )
	grp_at(list[i_l]).at().cfSaveValBD("NAME",Owner().BD().at(b_hd));
    Owner().BD().at(b_hd).Save();
    Owner().BD().close(b_hd);
}

//==============================================================
//================== Controll functions ========================
//==============================================================
void TSequrity::ctr_fill_info( XMLNode *inf )
{
    char *i_cntr = 
    	"<oscada_cntr>"
	" <area id='a_usgr'>"
	"  <list id='users' s_com='add,del' tp='br' mode='att'/>"    
	"  <list id='grps' s_com='add,del' tp='br' mode='att'/>"    
	" </area>"    
	" <area id='a_bd' acs='0440'>"
	"  <fld id='u_t_bd' acs='0660' tp='str' dest='select' select='/a_bd/b_mod'/>"
	"  <fld id='u_bd' acs='0660' tp='str'/>"
	"  <fld id='u_tbl' acs='0660' tp='str'/>"
	"  <fld id='g_t_bd' acs='0660' tp='str' dest='select' select='/a_bd/b_mod'/>"
	"  <fld id='g_bd' acs='0660' tp='str'/>"
	"  <fld id='g_tbl' acs='0660' tp='str'/>"
	"  <fld id='g_help' acs='0440' tp='str' cols='90' rows='5'/>"
	"  <comm id='load_bd'/>"
	"  <comm id='upd_bd'/>"
	"  <list id='b_mod' tp='str' hide='1'/>"
	" </area>"    
	"</oscada_cntr>";
    char *dscr = "dscr";
    
    inf->load_xml( i_cntr );
    inf->set_text(Mess->I18N("Sequrity subsystem"));
    //a_bd
    XMLNode *c_nd = inf->get_child(1);
    c_nd->set_attr(dscr,Mess->I18N("Subsystem control"));
    c_nd->get_child(0)->set_attr(dscr,Mess->I18N("User BD (module:bd:table)"));
    c_nd->get_child(3)->set_attr(dscr,Mess->I18N("Group BD (module:bd:table)"));
    c_nd->get_child(6)->set_attr(dscr,Mess->I18N("Options help"));
    c_nd->get_child(7)->set_attr(dscr,Mess->I18N("Load BD"));
    c_nd->get_child(8)->set_attr(dscr,Mess->I18N("Update BD"));
    //a_usgr
    c_nd = inf->get_child(0);
    c_nd->set_attr(dscr,Mess->I18N("Users and groups"));
    c_nd->get_child(0)->set_attr(dscr,Mess->I18N("Users"));
    c_nd->get_child(1)->set_attr(dscr,Mess->I18N("Groups"));    
}

void TSequrity::ctr_din_get_( const string &a_path, XMLNode *opt )
{
    vector<string> list;
    
    if( a_path == "/a_bd/u_t_bd" )     ctr_opt_setS( opt, m_bd_usr.tp );
    else if( a_path == "/a_bd/u_bd" )  ctr_opt_setS( opt, m_bd_usr.bd );
    else if( a_path == "/a_bd/u_tbl" ) ctr_opt_setS( opt, m_bd_usr.tbl );
    else if( a_path == "/a_bd/g_t_bd" )ctr_opt_setS( opt, m_bd_grp.tp );
    else if( a_path == "/a_bd/g_bd" )  ctr_opt_setS( opt, m_bd_grp.bd );
    else if( a_path == "/a_bd/g_tbl" ) ctr_opt_setS( opt, m_bd_grp.tbl );
    else if( a_path == "/a_bd/b_mod" )
    {
	Owner().BD().gmd_list(list);
	opt->clean_childs();
	for( unsigned i_a=0; i_a < list.size(); i_a++ )
	    ctr_opt_setS( opt, list[i_a], i_a );
    }
    else if( a_path == "/a_bd/g_help" ) ctr_opt_setS( opt, opt_descr() );       
    else if( a_path == "/a_usgr/users" )
    {
	usr_list(list);
	opt->clean_childs();
	for( unsigned i_a=0; i_a < list.size(); i_a++ )
	    ctr_opt_setS( opt, list[i_a], i_a ); 	
    }
    else if( a_path == "/a_usgr/grps" )
    {
	grp_list(list);
	opt->clean_childs();
	for( unsigned i_a=0; i_a < list.size(); i_a++ )
	    ctr_opt_setS( opt, list[i_a], i_a ); 	
    }    
    else throw TError("(%s) Branch %s error!",o_name,a_path.c_str());
}

void TSequrity::ctr_din_set_( const string &a_path, XMLNode *opt )
{
    if( a_path == "/a_bd/u_t_bd" )     m_bd_usr.tp  = ctr_opt_getS( opt );
    else if( a_path == "/a_bd/u_bd" )  m_bd_usr.bd  = ctr_opt_getS( opt );
    else if( a_path == "/a_bd/u_tbl" ) m_bd_usr.tbl = ctr_opt_getS( opt );
    else if( a_path == "/a_bd/g_t_bd" )m_bd_grp.tp  = ctr_opt_getS( opt );
    else if( a_path == "/a_bd/g_bd" )  m_bd_grp.bd  = ctr_opt_getS( opt );
    else if( a_path == "/a_bd/g_tbl" ) m_bd_grp.tbl = ctr_opt_getS( opt );
    else if( a_path.substr(0,13) == "/a_usgr/users" )
	for( int i_el=0; i_el < opt->get_child_count(); i_el++)	    
	{
	    XMLNode *t_c = opt->get_child(i_el);
	    if( t_c->get_name() == "el")
	    {
		if(t_c->get_attr("do") == "add")      usr_add(t_c->get_text());
		else if(t_c->get_attr("do") == "del") usr_del(t_c->get_text());
	    }
	}
    else if( a_path.substr(0,12) == "/a_usgr/grps" )
	for( int i_el=0; i_el < opt->get_child_count(); i_el++)	    
	{
	    XMLNode *t_c = opt->get_child(i_el);
	    if( t_c->get_name() == "el")
	    {
		if(t_c->get_attr("do") == "add")      grp_add(t_c->get_text());
		else if(t_c->get_attr("do") == "del") grp_del(t_c->get_text());
	    }
	}
    else throw TError("(%s) Branch %s error!",o_name,a_path.c_str());
}

void TSequrity::ctr_cmd_go_( const string &a_path, XMLNode *fld, XMLNode *rez )
{
    if( a_path == "/a_bd/load_bd" )     LoadBD();
    else if( a_path == "/a_bd/upd_bd" ) UpdateBD();
    else throw TError("(%s) Branch %s error!",o_name,a_path.c_str());
}

AutoHD<TContr> TSequrity::ctr_at1( const string &br )
{
    if( br.substr(0,13) == "/a_usgr/users" )     return( usr_at(ctr_path_l(br,2)) ); 
    else if( br.substr(0,12) == "/a_usgr/grps" ) return( grp_at(ctr_path_l(br,2)) ); 
    else throw TError("(%s) Branch %s error!",o_name,br.c_str());
}

//**************************************************************
//*********************** TUser ********************************
//**************************************************************
    
TUser::TUser( TSequrity *owner, const string &name, unsigned id, TElem *el ) : 
    m_owner(owner), TConfig(el),
    m_lname(cfg("DESCR").getS()), m_pass(cfg("PASS").getS()), m_name(cfg("NAME").getS()), 
    m_id(cfg("ID").getI()), m_grp(cfg("GRP").getS())
{
    Name(name);
    Id(id);
}

TUser::~TUser(  )
{

}

void TUser::Load( )
{
    TBDS &bds  = Owner().Owner().BD();
    SHDBD t_hd = bds.open( Owner().BD_user() );	
    cfLoadValBD("NAME",bds.at(t_hd));
    bds.close(t_hd);
}

void TUser::Save( )
{
    TBDS &bds  = Owner().Owner().BD();
    SHDBD t_hd = bds.open( Owner().BD_user() );	
    cfSaveValBD("NAME",bds.at(t_hd));
    bds.at(t_hd).Save(); 
    bds.close(t_hd);
}
//==============================================================
//================== Controll functions ========================
//==============================================================
void TUser::ctr_fill_info( XMLNode *inf )
{
    char *i_cntr = 
    	"<oscada_cntr>"
	" <area id='a_prm'>"
	"  <fld id='name' acs='0644' tp='str'/>"
	"  <fld id='dscr' acs='0644' tp='str'/>"
	"  <fld id='grp' acs='0644' tp='str' dest='select' select='/a_prm/grps'/>"
	"  <fld id='id' acs='0644' tp='dec'/>"
	"  <list id='grps' tp='str' hide='1'/>"
	"  <comm id='pass' acs='0500'>"
	"   <fld id='ps' tp='str'/>"
	"  </comm>"
	"  <comm id='load' acs='0550'/>"
	"  <comm id='save' acs='0550'/>"    
	" </area>"
	"</oscada_cntr>";
    char *dscr = "dscr";

    inf->load_xml( i_cntr );
    inf->set_text(Mess->I18Ns("User ")+Name());
    //a_prm
    XMLNode *c_nd = inf->get_child(0);
    c_nd->set_attr(dscr,Mess->I18N("Parameters"));
    c_nd->get_child(0)->set_attr(dscr,cfg("NAME").fld().descr());
    c_nd->get_child(0)->set_attr("own",TSYS::int2str(m_id));
    c_nd->get_child(1)->set_attr(dscr,cfg("DESCR").fld().descr());
    c_nd->get_child(1)->set_attr("own",TSYS::int2str(m_id));
    c_nd->get_child(2)->set_attr(dscr,cfg("GRP").fld().descr());
    c_nd->get_child(3)->set_attr(dscr,cfg("ID").fld().descr());
    c_nd->get_child(6)->set_attr(dscr,Mess->I18N("Load user"));
    c_nd->get_child(7)->set_attr(dscr,Mess->I18N("Save user"));
    c_nd = c_nd->get_child(5);
    c_nd->set_attr(dscr,Mess->I18N("Set"));
    c_nd->get_child(0)->set_attr(dscr,cfg("PASS").fld().descr());
}

void TUser::ctr_din_get_( const string &a_path, XMLNode *opt )
{        
    if( a_path == "/a_prm/name" )       ctr_opt_setS( opt, Name() );
    else if( a_path == "/a_prm/dscr" )  ctr_opt_setS( opt, Descr() );
    else if( a_path == "/a_prm/grp" )   ctr_opt_setS( opt, Grp() );
    else if( a_path == "/a_prm/id" )    ctr_opt_setI( opt, Id() );
    else if( a_path == "/a_prm/grps" )  
    {
	vector<string> list;
	Owner().grp_list(list);
	opt->clean_childs();
	for( unsigned i_a=0; i_a < list.size(); i_a++ )
	    ctr_opt_setS( opt, list[i_a], i_a );
    }
    else throw TError("(%s) Branch %s error!",__func__,a_path.c_str());
}

void TUser::ctr_din_set_( const string &a_path, XMLNode *opt )
{
    if( a_path == "/a_prm/name" )       Name( ctr_opt_getS( opt ) );
    else if( a_path == "/a_prm/dscr" )  Descr( ctr_opt_getS( opt ) );
    else if( a_path == "/a_prm/grp" )   Grp( ctr_opt_getS( opt ) );
    else if( a_path == "/a_prm/id" )    Id( ctr_opt_getI( opt ) );
    else throw TError("(%s) Branch %s error!",__func__,a_path.c_str());
}

void TUser::ctr_cmd_go_( const string &a_path, XMLNode *fld, XMLNode *rez )
{
    if( a_path == "/a_prm/pass" )      Pass( ctr_opt_getS(ctr_id(fld,"ps")) );
    else if( a_path == "/a_prm/load" ) Load();
    else if( a_path == "/a_prm/save" ) Save();
    else throw TError("(%s) Branch %s error!",__func__,a_path.c_str());
}
//**************************************************************
//*********************** TGroup *******************************
//**************************************************************
    
TGroup::TGroup( TSequrity *owner, const string &name, unsigned id, TElem *el ) : 
    m_owner(owner), TConfig(el),
    m_lname(cfg("DESCR").getS()), m_usrs(cfg("USERS").getS()), m_name(cfg("NAME").getS()), m_id(cfg("ID").getI())
{
    Name(name);
    Id(id);
}

TGroup::~TGroup(  )
{

}

void TGroup::Load( )
{
    TBDS &bds  = Owner().Owner().BD();
    SHDBD t_hd = bds.open( Owner().BD_grp() );	
    cfLoadValBD("NAME",bds.at(t_hd));
    bds.close(t_hd);
}

void TGroup::Save( )
{
    TBDS &bds  = Owner().Owner().BD();
    SHDBD t_hd = bds.open( Owner().BD_grp() );	
    cfSaveValBD("NAME",bds.at(t_hd));
    bds.at(t_hd).Save(); 
    bds.close(t_hd);
}

bool TGroup::user( const string &name )
{
    if( m_usrs.find(name,0) != string::npos ) return(true);
    return(false);
}

//==============================================================
//================== Controll functions ========================
//==============================================================
void TGroup::ctr_fill_info( XMLNode *inf )
{
    char *i_cntr = 
    	"<oscada_cntr>"
	" <area id='a_prm'>"
	"  <fld id='name' acs='0644' tp='str'/>"
	"  <fld id='dscr' acs='0644' tp='str'/>"
	"  <fld id='id' acs='0644' tp='dec'/>"
	"  <list id='users' acs='0644' tp='str' s_com='add,del' dest='select' select='/a_prm/usrs'/>"
	"  <list id='usrs' tp='str' hide='1'/>"
	"  <comm id='load' acs='0550'/>"
	"  <comm id='save' acs='0550'/>"    
	" </area>"
	"</oscada_cntr>";
    char *dscr = "dscr";

    inf->load_xml( i_cntr );
    inf->set_text(Mess->I18Ns("Group ")+Name());
    //a_prm
    XMLNode *c_nd = inf->get_child(0);
    c_nd->set_attr(dscr,Mess->I18N("Parameters"));
    c_nd->get_child(0)->set_attr(dscr,cfg("NAME").fld().descr());
    c_nd->get_child(1)->set_attr(dscr,cfg("DESCR").fld().descr());
    c_nd->get_child(2)->set_attr(dscr,cfg("ID").fld().descr());
    c_nd->get_child(3)->set_attr(dscr,cfg("USERS").fld().descr());
    c_nd->get_child(5)->set_attr(dscr,Mess->I18N("Load group"));
    c_nd->get_child(6)->set_attr(dscr,Mess->I18N("Save group"));
}

void TGroup::ctr_din_get_( const string &a_path, XMLNode *opt )
{
    if( a_path == "/a_prm/name" )       ctr_opt_setS( opt, Name() );
    else if( a_path == "/a_prm/dscr" )  ctr_opt_setS( opt, Descr() );
    else if( a_path == "/a_prm/id" )    ctr_opt_setI( opt, Id() );
    else if( a_path == "/a_prm/users" )
    {
	int pos = 0,c_pos,i_us=0;
	opt->clean_childs();
	do
	{
	    c_pos = m_usrs.find(";",pos);
	    string val = m_usrs.substr(pos,c_pos-pos);
	    if( val.size() ) ctr_opt_setS( opt, val, i_us++ );
	    pos = c_pos+1;
	}while(c_pos != string::npos);
    }
    else if( a_path == "/a_prm/usrs" )  
    {
	vector<string> list;
	Owner().usr_list(list);
	opt->clean_childs();
	for( unsigned i_a=0; i_a < list.size(); i_a++ )
	    ctr_opt_setS( opt, list[i_a], i_a );
    }
}

void TGroup::ctr_din_set_( const string &a_path, XMLNode *opt )
{
    if( a_path == "/a_prm/name" )       Name(ctr_opt_getS( opt ));
    else if( a_path == "/a_prm/dscr" )  Descr(ctr_opt_getS( opt ));
    else if( a_path == "/a_prm/id" )    Id(ctr_opt_getI( opt ));
    else if( a_path.substr(0,12) == "/a_prm/users" )
	for( int i_el=0; i_el < opt->get_child_count(); i_el++)	    
	{
	    XMLNode *t_c = opt->get_child(i_el);
	    if( t_c->get_name() == "el")
	    {
		if(t_c->get_attr("do") == "add")
		{
		    if(m_usrs.size()) m_usrs=m_usrs+";";
		    m_usrs=m_usrs+t_c->get_text();
		}
		else if(t_c->get_attr("do") == "del") 
		{
		    int pos = m_usrs.find(string(";")+t_c->get_text(),0);
		    if(pos != string::npos) 
			m_usrs.erase(pos,t_c->get_text().size()+1);
		    else                    
			m_usrs.erase(m_usrs.find(t_c->get_text(),0),t_c->get_text().size()+1);
		}
	    }
	}
}

void TGroup::ctr_cmd_go_( const string &a_path, XMLNode *fld, XMLNode *rez )
{
    if( a_path == "/a_prm/load" )      Load();
    else if( a_path == "/a_prm/save" ) Save();	
}

