
#ifndef TUIS_H
#define TUIS_H

#include <string>
using std::string;

#include "tmodule.h"
#include "tgrpmodule.h"

//================================================================
//================== TUI =========================================
//================================================================

class TUI : public TModule
{
/** Public methods: */
    public:
     	TUI( ){ }

	virtual void Start( ){ }
	virtual void Stop( ){ }

	/** Private atributes: */
    private:
	static const char *o_name;
};

//================================================================
//================== TUIS ========================================
//================================================================

class TUIS : public TGRPModule
{
    /** Public methods: */
    public:
	TUIS( TKernel *app );

	TUI &gmd_at(unsigned hd)     { return( (TUI &)TGRPModule::gmd_at(hd) ); }
	TUI &operator[](unsigned hd) { return( gmd_at(hd) ); }

	void gmd_Start( );
	void gmd_Stop( );

	void gmd_CheckCommandLine( );
	void gmd_UpdateOpt();

    /** Private methods: */
    private:
	void pr_opt_descr( FILE * stream );	

    /** Private atributes: */
    private:
	static const char *o_name;
};

#endif // TUIS_H
