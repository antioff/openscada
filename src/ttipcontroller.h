
#ifndef TTIPCONTROLLER_H
#define TTIPCONTROLLER_H

#include <string>
using std::string;

#include "tconfig.h"
#include "tgrpmodule.h"

#define TCNTR_FREE    0   //Cell free 
#define TCNTR_DISABLE 1   //Controller present and disable 
#define TCNTR_ENABLE  2   //Controller present and enabled
#define TCNTR_ERR     3   //Controller present, disabled and error

struct SContr            //Contain data from GENERIC BD
{
    int    stat;         //Stat controller
    string name;	 //Name controller
    string modul;	 //Name controller's module
    int    id_mod;	 //Modul's ID 
    int    id_contr;     //Controller's ID into module    
    string bd;           //Controller's BD  
};

class TController;

struct STContr            //Contain controller type data link to TGRPModule
{
    int                   idmod;   //ID module into TGRPModule
    vector<TController *> contr;   //List controller's params 
    TConfig               config;  //Structure and value a configure params of controller
};

class TTipController : public TGRPModule
{

/** Public methods: */
public:
    TTipController(  );

//    int (*MGetParamVal)(  );

//    int (*MSetParamVal)(  );
    /*
     * List controllers for <NameContrTip> type's
     */
    void ContrList( const string NameContrTip, string & List );
    /*
     * Init All controller's modules
     */    
    virtual int InitAll( );
    /*
     * Init moduls controller's types. 
     */
    void Init(  );
    /*
     * Init and start all configured controllers.
     */ 
    void DeInit(  );                                        //?!?!
    /*
     * Init and start all configured controllers.
     */ 
    void Start(  );                                         //?!?!
    /*
     * Stop and deinit all configured controllers.
     */ 
    void Stop(  );                                          //?!?!
    /*
     * Add Controller for type controllers <tip> with BD <bd>
     */    
    int AddContr( string name, string tip, string bd );     //?!?!
    /*
     * Delete Controller
     */    
    int DelContr( string name );                            //?!?!
    /**
      * Load/Reload all BD and update internal controllers structure!
      */
    void LoadBD( );
    /*
     * Update all BD from current to external BD.
     */
    int UpdateBD( );
    /**
     * Create general BD.
     */
    int CreateGenerBD( string type_bd );

    /**
      * Get the stat of controller's task. 
      * @param NameTipCtr
      *        Name of controller.
      */
//    int GetContrInfo( string NameTipCtr, string NameCtr );
    /*
     * Put command to controller: 
     * DISABLE (GLOBAL) - disable;
     * ENABLE (GLOBAL) - enable;
     * INIT (LOCAL) - init controller; 
     * DEINIT (LOCAL) - deinit controller;
     * START (LOCAL) - start controller;
     * STOP (LOCAL) - stop controller;
     * ADD (LOCAL) - add controller;
     * DELETE (LOCAL) - delete controller;
     */
//    int PutCntrComm( string comm, string NameCtr = "" );          //?!?!?!
    int PutCntrComm( string comm, int id_ctr );

//    string GetParamTipList( string NameCtr, string NameTask );

//    int ReloadBDCtr( string Command );

//    int PutCom( string type );


//    int (*MGetConfigCadr)( string & cadr );

//    int (*MPutConfigCadr)( string cadr );

//    int OperContr( string NameTip, int NameCtr, string NameSecond, string opper );

    void CheckCommandLine(  );

/** Private methods: */
private:
    /**
      * It's gate to function of modul.   Load modul's (self) parameters from phisic BD
      * to local BD.
      */
//    int (*MLoadSelfBD)( string NameCtr, int hdBD, string mode );

    /**
      * It's gate to function of modul.   Save modul's (self) parameters from local BD to
      * phisic BD.
      */
//    int (*MSaveSelfBD)( string NameCtr, int hdBD, string mode );

    /**
      * It's gate to function of modul.   Get controller's (self) status.
      */
//    int (*MGetCtrInfo)( string NameContr, string & info );



    /**
     * Print desription of command line options.
     */
    void pr_opt_descr( FILE * stream );

    /**
     * Test
     */
    int test();
/** Private atributes: */
private:
    string gener_bd;
    vector< SContr *>  Contr;   //Controllers list from BD
    vector< STContr *> TContr;  //Tip controllers list from TGRPModule
/** Private methods: */
private:
    virtual int AddM( TModule *modul );
    virtual int DelM( int hd );    
};

#endif // TTIPCONTROLLER_H
