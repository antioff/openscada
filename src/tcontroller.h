
#ifndef TCONTROLLER_H
#define TCONTROLLER_H

#include <time.h>
#include <string>

#include "tkernel.h"
#include "tconfig.h"
#include "tbds.h"
#include "tparams.h"

#define TCNTR_ENABLE  0x01   //enabled
#define TCNTR_RUN     0x02   //run
#define TCNTR_ERR     0x04   //error

using std::string;

class TParamContr;
class TTipController;

class TController : public TConfig
{
    /** Public methods: */
    public:
     	TController( string name_c, SBDS bd, TTipController *tcntr, TConfigElem *cfgelem );

	virtual ~TController(  );

	virtual void Load(  );
	virtual void Save(  );
	virtual void Free(  );
	virtual void Start(  );
	virtual void Stop(  );
	virtual void Enable(  );
	virtual void Disable(  );

	/*
	 * Add parameter with type Name_P and <name> to position <pos> (<0 - to end) 
	 */
	unsigned  Add( string Name_TP, string name, int pos );
	/*
         * Delete parameter with type Name_P and <name>
	 */    
	void  Del( string name );
	/*
	 * Rotated parameter with type Name_P between name1 and name2
	 */
	void  Rotate( string name1, string name2);

	string &Name() { return(name); }
	char   Stat() { return(stat); }
	/*
	 * Registering parameter(s)
	 */
	void RegParamS();
	void RegParam( unsigned id_hd );
	/*
	 * UnRegistering parameter(s)
	 */
	void UnRegParamS();
	void UnRegParam( unsigned id_hd );

	void List( vector<string> & List );
	/*
	 * Convert Name parameter to hd (hd - individual number of parameter for fast calling to parameter )
	 */
	unsigned NameToHd( string Name );
	/*
	 * Get Parameter throw hd (individual parameter number)
	 */
	TParamContr &at( unsigned id_hd );
	TParamContr &operator[]( unsigned id_hd ){ return( at(id_hd) ); }
    
	TTipController &Owner() { return( *owner ); }
	
    protected:    
	vector< int > hd;                 //header of parameter 
	vector< TParamContr * > cntr_prm; //config parameter
	
    /** Private methods: */
    private:
    	/*
	 * Hd operations
         */
	unsigned HdIns( unsigned id );
	void HdFree( unsigned id );
	void HdChange( unsigned id1, unsigned id2 );

	void LoadParmCfg(  );
	void SaveParmCfg(  );
	void FreeParmCfg(  );
	
	virtual TParamContr *ParamAttach(int type);
	
    /**Attributes: */
    private:    
    	string  name;
	char    stat;
	SBDS    m_bd;
    
	TTipController *owner;    

	static const char *o_name;
};


#endif // TCONTROLLER_H
