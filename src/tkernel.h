
#ifndef TKERNEL_H
#define TKERNEL_H

#include <stdio.h>

#include <string>
#include <vector>

#include "xml.h"
#include "tcontr.h"

using std::string;
using std::vector;

class    TUIS;
class    TArhiveS;
class    TBDS;
class    TControllerS;
class    TProtocolS;
class    TTransportS;
class    TSpecialS;
class    TParamS;
class    TModSchedul;
class    TSequrity;

class TKernel : public TContr 
{
    /** Public methods: */
    public:
	TKernel( string name );
	~TKernel(  );

	TUIS         &UI()     { return (*ui); }
	TArhiveS     &Arhive() { return (*arhive); } 
	TBDS         &BD()     { return (*bd); }
	TControllerS &Controller() { return (*controller); }
	TProtocolS   &Protocol()   { return (*protocol); }
	TTransportS  &Transport()  { return (*transport); }
	TSpecialS    &Special()    { return (*special); }
	TParamS      &Param()      { return (*param); }
	TModSchedul  &ModSchedul() { return (*modschedul); }
	TSequrity    &Sequrity()   { return (*sequrity); }
	
	// Run server
	int run(  );
	// Get optio from generic config file.
	void UpdateOpt();
	// Update comand line option
    	void CheckCommandLine(bool mode = false );
	// Print comand line options! 
	void pr_opt_descr( FILE * stream );
    	// Get XML kernel node
    	XMLNode *XMLCfgNode();
	// Kernel name
    	string &Name() { return( m_name ); }
        //================== Controll functions ========================
	void ctr_din_get( XMLNode *opt );
        //================== Message functions ========================
	void m_put( string categ, int level, char *fmt,  ... );
	void m_put_s( string categ, int level, string mess );

    /**Attributes: */
    public:
	string ModPath;
	string DefBDType;
	string DefBDName;
    
	vector<string> auto_m_list;
	
    /** Private attributes: */
    private:
        //================== Controll functions ========================
	void ctr_fill_info( XMLNode *inf );
	TContr &ctr_at( XMLNode *br );
    /** Private methods: */
    private:    
	string       m_name;
	
	TUIS         *ui;
	TArhiveS     *arhive;
	TBDS         *bd;
	TControllerS *controller;
	TProtocolS   *protocol;
	TTransportS  *transport;
	TSpecialS    *special;
	TParamS      *param;
	TModSchedul  *modschedul;
	TSequrity    *sequrity;

	static const char *n_opt;
	static const char *o_name;
	static const char *i_cntr;
};

#endif // TKERNEL_H
