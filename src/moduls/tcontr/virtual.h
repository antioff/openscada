#ifndef VIRTUAL_H
#define VIRTUAL_H

#include "../../tmodule.h"
#include "../../tvalue.h"
#include "../../tcontroller.h"
#include "../../ttipcontroller.h"
#include "../../tparamcontr.h"

#include <string>
using std::string;
#include <vector>
using std::vector;

typedef unsigned char  byte;
typedef unsigned short word;
typedef short          word_s;
typedef unsigned long  dword;

//------- Discription formuls and algobloks -----------------------
struct SFrm
{
    string         name;
    char           tip;
    byte           n_inp;
    byte           n_koef;
    vector<string> name_inp;
    vector<string> name_kf;
    string         formul;
    unsigned       l_frm_e;
    char           *form_e;
};

struct SAlgb
{
    string           name;           //Name of a algoblok (parameter name)
    string           descr;          //Description of a algoblok 
    word             tp_alg;         //Type of a algoblok 
    vector< string > io;             //IO parameters
    vector< float >  kf;             //koefficients
};

class TVirtAlgb
{
    public:
    	TVirtAlgb(string cfg_file);
	~TVirtAlgb();

	void Load(string f_alg = "");
	void Save(string f_alg = "");
	void Free();
        //Formuls
	SFrm *GetFrm(unsigned id);
	unsigned SizeFrm() { return(frm_s.size()); }
	//Algobloks
	SAlgb *GetAlg(string name);	
    private:
	string          file;
	vector< SFrm >  frm_s;
	vector< SAlgb > algb_s;	
};

class TConfig;
class TConfigElem;
class TVirtual;

class TContrVirt: public TController
{
public:
    TContrVirt( TTipController *tcntr, string name_c,string _t_bd, string _n_bd, string _n_tb, TConfigElem *cfgelem);
    virtual ~TContrVirt();   

    virtual void Load(  );
    virtual void Save(  );
    virtual void Free(  );
    virtual void Start(  );
    virtual void Stop(  );    
    //virtual void Enable(  );
    //virtual void Disable(  );

    virtual TParamContr *ParamAttach(int type);
    int Period()  {return(period); }
    int Iterate() {return(iterate); }
public:
    
private:
    static void *Task(void *);
    static void wakeup(int n_sig) {}

    void Sync();
private:
    bool      run_st;      // Stat of task
    bool      endrun;      // Command for stop task
    int       period;      // ms
    int       d_sync;      // counter for syncing virtual controller    
    int       iterate;
    pthread_t pthr_tsk;
};

struct SIO
{
    bool  internal;    // internal locate parameter direct access
    //int  hd_cntr;     // maybe make direct access to virtualV1 type controller
    int   hd_prm;      // hd parameter local (TParamS) or external (TController)
    float min,max;    // scale
    bool  sync;
};

#define R_MAN  0
#define R_AUTO 1
#define R_CAS  2

struct SPID
{
    int   hd_out;
    int   hd_sp;
    int   hd_stat;
};

class TPrmVirt : public TParamContr
{
 public:
    TPrmVirt(TController *contr,  TTipParam *tp_prm );	    
    ~TPrmVirt( );
    
    void UpdateVAL();
    
    void Load( );
    
    float Calc();
    void  Sync();
    
 private:
    int           form;
    vector<SIO>   x_id;    
    vector<float> x;
    vector<float> k;
    int           hd_y;
    float         y_min,y_max;    
    SPID          *pid;           //for pid
 private:
    void SetVal( int id_elem );
    void GetVal( int id_elem );
    
    void  Y(float val);
    float Y();
    
    void  X(unsigned id ,float val);
    float X(unsigned id);
    
    float blok_dig( );
    float ymn();
    float sym();
    float free_formul();
    float calk_form(char *form, int len, int *off, float rez,byte h_prior);
    void  calk_oper(byte symb,float *rez,float parm);
    float pid_n( );
};

class TVirtual: public TTipController
{
public:
    TVirtual(char *name);
    ~TVirtual();
	
    void info( const string & name, string & info );
    //void connect( void *obj );
    void init( void *param );
    
    void CheckCommandLine( char **argv, int argc );
    void UpdateOpt(  );

    TController *ContrAttach(string name, string t_bd, string n_bd, string n_tb);

    string NameCfg()   { return(NameCfgF); }
    TVirtAlgb *AlgbS() { return(algbs); }
public:

private:
    void pr_opt_descr( FILE * stream );
private:
    static SCfgFld elem[];
    static SCfgFld ElemAN[];
    static SCfgFld ElemDG[];
    static SCfgFld ElemBL[];

    static SVAL  ValAN[];
    static SVAL  ValDG[];
    static SVAL  ValPID[];
	
    static SExpFunc ExpFuncLc[];
    TVirtAlgb    *algbs;
    //TTipController *TContr;
    //Name of config file for virtual controllers
    string       NameCfgF;
};




#endif //VIRTUAL_H

