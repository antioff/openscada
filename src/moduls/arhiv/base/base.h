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

#ifndef BASE_H
#define BASE_H

#include <tmodule.h>
#include <xml.h>
#include <tarhives.h>

namespace BaseArh
{
    class TMessArh;
    
    class TFileArh
    {
	public:
 	    TFileArh( TMessArh *owner );
 	    TFileArh( const string &name, time_t beg, time_t end, TMessArh *owner); // string charset, int time_size );
 	    ~TFileArh();

	    void Attach( const string &name );
	    void put( SBufRec mess );
	    void get( time_t b_tm, time_t e_tm, vector<SBufRec> &mess, const string &category, char level );
	    // Write changes to arhive file 
	    //  free - surely free used memory
	    void Sync( bool free = false );     

	    string &Name() { return(m_name); }
	    time_t &Begin(){ return(m_beg); }
	    time_t &End()  { return(m_end); }
	    bool   &Err()  { return(m_err); }

	    TMessArh &Owner() { return(*m_owner); }
	public:
    	    bool    scan;    // arhive scaned (for check deleted files)
	private:	    
	    TMessArh* m_owner;
	
    	    string  m_name;    // name arhive file;
    	    string  m_chars;   // arhive charset;
    	    bool    m_err;     // arhive err
    	    bool    m_write;   // arhive had changed but no writed to file
	    bool    m_load;    // arhiv load to m_node
	    time_t  m_acces;   // last of time acces to arhive file
    	    time_t  m_beg;     // begin arhive file;
    	    time_t  m_end;     // end arhive file;
    	    XMLNode m_node;    // XMLNode = !NULL if opened 
    	    int     m_res;     // resource to access;	
    };

    class TMessArh: public TArhiveMess
    {
	public:
	    TMessArh( const string &name, TTipArhive *owner );
	    ~TMessArh( );

	    void put( vector<SBufRec> &mess );
	    void get( time_t b_tm, time_t e_tm, vector<SBufRec> &mess, const string &category = "", char level = 0 );
	    void start();
	    void stop();
	private:	
	    void ScanDir();

	    static void *Task(void *param);	
	private:	
    	    int       m_res;     // resource to access;	
            bool      m_endrun;  // pthread end run command;	    
	    pthread_t m_pthr;
	    
	    vector<TFileArh *>  arh_s;	    
    };
    
    class TMArhive: public TTipArhive
    {
	friend class TMessArh;
	friend class TFileArh;
	public:
	    TMArhive( const string &name );
	    ~TMArhive();

	    void mod_CheckCommandLine( );
	    void mod_UpdateOpt();
	public:
    
	private:
	    void mod_connect(  );

	    TArhiveMess *AMess(const string &name);
	    
	    string opt_descr( );
	    //================== Controll functions ========================
	    void ctr_fill_info( XMLNode *inf );
	    void ctr_din_get_( const string &a_path, XMLNode *opt );
	    void ctr_din_set_( const string &a_path, XMLNode *opt );
	private:
	    string m_mess_charset;   // default message charset
	    int    m_mess_max_size;  // maximum size kb of arhives file
	    int    m_mess_numb_file; // number of arhive files
	    int    m_mess_time_size; // number days to one file
	    int    m_mess_timeout_free; // timeout of free no used message file buffer;
	    static SExpFunc ExpFuncLc[];
    };
}

#endif //BASE_H

