
//OpenSCADA system module DAQ.BFN file: mod_BFN.h
/***************************************************************************
 *   Copyright (C) 2010 by Roman Savochenko                                *
 *   rom_as@oscada.org, rom_as@fromru.com                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; version 2 of the License.               *
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

#ifndef MOD_BFN_H
#define MOD_BFN_H

#include <string>
#include <vector>

#include <tcontroller.h>
#include <ttipdaq.h>
#include <tparamcontr.h>

#undef _
#define _(mess) mod->I18N(mess)

using std::string;
using std::vector;
using namespace OSCADA;

namespace ModBFN
{

//*************************************************
//* ModBFN::TMdPrm                                *
//*************************************************
class TMdContr;

class TMdPrm : public TParamContr
{
    friend class TMdContr;
    public:
	//Methods
	TMdPrm(string name, TTipParam *tp_prm);
	~TMdPrm( );

	TElem &elem( )		{ return p_el; }

	void enable( );
	void disable( );

	TMdContr &owner( );

    protected:
	//Methods
	void load_( );
	void save_( );

    private:
	//Methods
	void postEnable(int flag);
	void cntrCmdProc(XMLNode *opt);
	void vlArchMake(TVal &val);

	//Attributes
	int	curAlrmsId;
	TElem	p_el;			//Work atribute elements
};

//*************************************************
//* ModBFN::TMdContr                              *
//*************************************************
class TMdContr: public TController
{
    friend class TMdPrm;
    public:
	//Methods
	TMdContr(string name_c, const string &daq_db, ::TElem *cfgelem);
	~TMdContr( );

	string getStatus( );

	long long period( )	{ return mPer; }
	string	cron( )		{ return mSched; }
	int	prior( )	{ return mPrior; }
	double	syncPer( )	{ return mSync; }

	AutoHD<TMdPrm> at(const string &nm)	{ return TController::at(nm); }

	void reqBFN( XMLNode &io );

    protected:
	//Methods
	void prmEn(const string &id, bool val);

	void enable_( );
	void start_( );
	void stop_( );

	void cntrCmdProc( XMLNode *opt );	//Control interface command process

    private:
	//Methods
	TParamContr *ParamAttach(const string &name, int type);
	static void *Task(void *icntr);

	//Attributes
	Res	en_res;		//Resource for enable params
	int	&mPrior;	// Process task priority
	double	&mSync;		//Synchronization inter remote station: attributes list update.
	string	&mSched,	//Calc schedule
		&mAddr,		//Transport device address
		//&mHouse,	//BFN house for get
		&mUser,		//Auth user name
		&mPassword;	//Auth pasword
	long long mPer;

	bool	prc_st,		// Process task active
		endrun_req;	// Request to stop of the Process task

	vector< AutoHD<TMdPrm> > p_hd;

	double	tm_gath;	// Gathering time
};

//*************************************************
//* ModBFN::TTpContr                              *
//*************************************************
class TTpContr: public TTipDAQ
{
    public:
	//Methods
	TTpContr(string name);
	~TTpContr( );

    protected:
	//Methods
	void postEnable(int flag);

	void load_( );
	void save_( );

	bool redntAllow( )	{ return true; }

    private:
	//Methods
	TController *ContrAttach(const string &name, const string &daq_db);
};

extern TTpContr *mod;

} //End namespace ModBFN

#endif //MOD_BFN_H
