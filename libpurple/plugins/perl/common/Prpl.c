/*
 * This file was generated automatically by ExtUtils::ParseXS version 3.40 from the
 * contents of Prpl.xs. Do not edit this file, edit Prpl.xs instead.
 *
 *    ANY CHANGES MADE HERE WILL BE LOST!
 *
 */

#line 1 "Prpl.xs"
#include "module.h"

#line 13 "Prpl.c"
#ifndef PERL_UNUSED_VAR
#  define PERL_UNUSED_VAR(var) if (0) var = var
#endif

#ifndef dVAR
#  define dVAR		dNOOP
#endif


/* This stuff is not part of the API! You have been warned. */
#ifndef PERL_VERSION_DECIMAL
#  define PERL_VERSION_DECIMAL(r,v,s) (r*1000000 + v*1000 + s)
#endif
#ifndef PERL_DECIMAL_VERSION
#  define PERL_DECIMAL_VERSION \
	  PERL_VERSION_DECIMAL(PERL_REVISION,PERL_VERSION,PERL_SUBVERSION)
#endif
#ifndef PERL_VERSION_GE
#  define PERL_VERSION_GE(r,v,s) \
	  (PERL_DECIMAL_VERSION >= PERL_VERSION_DECIMAL(r,v,s))
#endif
#ifndef PERL_VERSION_LE
#  define PERL_VERSION_LE(r,v,s) \
	  (PERL_DECIMAL_VERSION <= PERL_VERSION_DECIMAL(r,v,s))
#endif

/* XS_INTERNAL is the explicit static-linkage variant of the default
 * XS macro.
 *
 * XS_EXTERNAL is the same as XS_INTERNAL except it does not include
 * "STATIC", ie. it exports XSUB symbols. You probably don't want that
 * for anything but the BOOT XSUB.
 *
 * See XSUB.h in core!
 */


/* TODO: This might be compatible further back than 5.10.0. */
#if PERL_VERSION_GE(5, 10, 0) && PERL_VERSION_LE(5, 15, 1)
#  undef XS_EXTERNAL
#  undef XS_INTERNAL
#  if defined(__CYGWIN__) && defined(USE_DYNAMIC_LOADING)
#    define XS_EXTERNAL(name) __declspec(dllexport) XSPROTO(name)
#    define XS_INTERNAL(name) STATIC XSPROTO(name)
#  endif
#  if defined(__SYMBIAN32__)
#    define XS_EXTERNAL(name) EXPORT_C XSPROTO(name)
#    define XS_INTERNAL(name) EXPORT_C STATIC XSPROTO(name)
#  endif
#  ifndef XS_EXTERNAL
#    if defined(HASATTRIBUTE_UNUSED) && !defined(__cplusplus)
#      define XS_EXTERNAL(name) void name(pTHX_ CV* cv __attribute__unused__)
#      define XS_INTERNAL(name) STATIC void name(pTHX_ CV* cv __attribute__unused__)
#    else
#      ifdef __cplusplus
#        define XS_EXTERNAL(name) extern "C" XSPROTO(name)
#        define XS_INTERNAL(name) static XSPROTO(name)
#      else
#        define XS_EXTERNAL(name) XSPROTO(name)
#        define XS_INTERNAL(name) STATIC XSPROTO(name)
#      endif
#    endif
#  endif
#endif

/* perl >= 5.10.0 && perl <= 5.15.1 */


/* The XS_EXTERNAL macro is used for functions that must not be static
 * like the boot XSUB of a module. If perl didn't have an XS_EXTERNAL
 * macro defined, the best we can do is assume XS is the same.
 * Dito for XS_INTERNAL.
 */
#ifndef XS_EXTERNAL
#  define XS_EXTERNAL(name) XS(name)
#endif
#ifndef XS_INTERNAL
#  define XS_INTERNAL(name) XS(name)
#endif

/* Now, finally, after all this mess, we want an ExtUtils::ParseXS
 * internal macro that we're free to redefine for varying linkage due
 * to the EXPORT_XSUB_SYMBOLS XS keyword. This is internal, use
 * XS_EXTERNAL(name) or XS_INTERNAL(name) in your code if you need to!
 */

#undef XS_EUPXS
#if defined(PERL_EUPXS_ALWAYS_EXPORT)
#  define XS_EUPXS(name) XS_EXTERNAL(name)
#else
   /* default to internal */
#  define XS_EUPXS(name) XS_INTERNAL(name)
#endif

#ifndef PERL_ARGS_ASSERT_CROAK_XS_USAGE
#define PERL_ARGS_ASSERT_CROAK_XS_USAGE assert(cv); assert(params)

/* prototype to pass -Wmissing-prototypes */
STATIC void
S_croak_xs_usage(const CV *const cv, const char *const params);

STATIC void
S_croak_xs_usage(const CV *const cv, const char *const params)
{
    const GV *const gv = CvGV(cv);

    PERL_ARGS_ASSERT_CROAK_XS_USAGE;

    if (gv) {
        const char *const gvname = GvNAME(gv);
        const HV *const stash = GvSTASH(gv);
        const char *const hvname = stash ? HvNAME(stash) : NULL;

        if (hvname)
	    Perl_croak_nocontext("Usage: %s::%s(%s)", hvname, gvname, params);
        else
	    Perl_croak_nocontext("Usage: %s(%s)", gvname, params);
    } else {
        /* Pants. I don't think that it should be possible to get here. */
	Perl_croak_nocontext("Usage: CODE(0x%" UVxf ")(%s)", PTR2UV(cv), params);
    }
}
#undef  PERL_ARGS_ASSERT_CROAK_XS_USAGE

#define croak_xs_usage        S_croak_xs_usage

#endif

/* NOTE: the prototype of newXSproto() is different in versions of perls,
 * so we define a portable version of newXSproto()
 */
#ifdef newXS_flags
#define newXSproto_portable(name, c_impl, file, proto) newXS_flags(name, c_impl, file, proto, 0)
#else
#define newXSproto_portable(name, c_impl, file, proto) (PL_Sv=(SV*)newXS(name, c_impl, file), sv_setpv(PL_Sv, proto), (CV*)PL_Sv)
#endif /* !defined(newXS_flags) */

#if PERL_VERSION_LE(5, 21, 5)
#  define newXS_deffile(a,b) Perl_newXS(aTHX_ a,b,file)
#else
#  define newXS_deffile(a,b) Perl_newXS_deffile(aTHX_ a,b)
#endif

#line 157 "Prpl.c"

XS_EUPXS(XS_Purple__Find_prpl); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_Purple__Find_prpl)
{
    dVAR; dXSARGS;
    if (items != 1)
       croak_xs_usage(cv,  "id");
    {
	const char *	id = (const char *)SvPV_nolen(ST(0))
;
	Purple__Plugin	RETVAL;

	RETVAL = purple_find_prpl(id);
	{
	    SV * RETVALSV;
	    RETVALSV = purple_perl_bless_object(RETVAL, "Purple::Plugin");
	    RETVALSV = sv_2mortal(RETVALSV);
	    ST(0) = RETVALSV;
	}
    }
    XSRETURN(1);
}


XS_EUPXS(XS_Purple__Prpl_change_account_status); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_Purple__Prpl_change_account_status)
{
    dVAR; dXSARGS;
    if (items != 3)
       croak_xs_usage(cv,  "account, old_status, new_status");
    {
	Purple__Account	account = purple_perl_ref_object(ST(0))
;
	Purple__Status	old_status = purple_perl_ref_object(ST(1))
;
	Purple__Status	new_status = purple_perl_ref_object(ST(2))
;

	purple_prpl_change_account_status(account, old_status, new_status);
    }
    XSRETURN_EMPTY;
}


XS_EUPXS(XS_Purple__Prpl_get_statuses); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_Purple__Prpl_get_statuses)
{
    dVAR; dXSARGS;
    if (items != 2)
       croak_xs_usage(cv,  "account, presence");
    PERL_UNUSED_VAR(ax); /* -Wall */
    SP -= items;
    {
	Purple__Account	account = purple_perl_ref_object(ST(0))
;
	Purple__Presence	presence = purple_perl_ref_object(ST(1))
;
#line 24 "Prpl.xs"
	GList *l, *ll;
#line 217 "Prpl.c"
#line 26 "Prpl.xs"
	ll = purple_prpl_get_statuses(account,presence);
	for (l = ll; l != NULL; l = l->next) {
		XPUSHs(sv_2mortal(purple_perl_bless_object(l->data, "Purple::Status")));
	}
	/* We can free the list here but the script needs to free the
	 * Purple::Status 'objects' itself. */
	g_list_free(ll);
#line 226 "Prpl.c"
	PUTBACK;
	return;
    }
}


XS_EUPXS(XS_Purple__Prpl_got_account_idle); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_Purple__Prpl_got_account_idle)
{
    dVAR; dXSARGS;
    if (items != 3)
       croak_xs_usage(cv,  "account, idle, idle_time");
    {
	Purple__Account	account = purple_perl_ref_object(ST(0))
;
	gboolean	idle = (bool)SvTRUE(ST(1))
;
	time_t	idle_time = (time_t)SvIV(ST(2))
;

	purple_prpl_got_account_idle(account, idle, idle_time);
    }
    XSRETURN_EMPTY;
}


XS_EUPXS(XS_Purple__Prpl_got_account_login_time); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_Purple__Prpl_got_account_login_time)
{
    dVAR; dXSARGS;
    if (items != 2)
       croak_xs_usage(cv,  "account, login_time");
    {
	Purple__Account	account = purple_perl_ref_object(ST(0))
;
	time_t	login_time = (time_t)SvIV(ST(1))
;

	purple_prpl_got_account_login_time(account, login_time);
    }
    XSRETURN_EMPTY;
}


XS_EUPXS(XS_Purple__Prpl_got_user_idle); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_Purple__Prpl_got_user_idle)
{
    dVAR; dXSARGS;
    if (items != 4)
       croak_xs_usage(cv,  "account, name, idle, idle_time");
    {
	Purple__Account	account = purple_perl_ref_object(ST(0))
;
	const char *	name = (const char *)SvPV_nolen(ST(1))
;
	gboolean	idle = (bool)SvTRUE(ST(2))
;
	time_t	idle_time = (time_t)SvIV(ST(3))
;

	purple_prpl_got_user_idle(account, name, idle, idle_time);
    }
    XSRETURN_EMPTY;
}


XS_EUPXS(XS_Purple__Prpl_got_user_login_time); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_Purple__Prpl_got_user_login_time)
{
    dVAR; dXSARGS;
    if (items != 3)
       croak_xs_usage(cv,  "account, name, login_time");
    {
	Purple__Account	account = purple_perl_ref_object(ST(0))
;
	const char *	name = (const char *)SvPV_nolen(ST(1))
;
	time_t	login_time = (time_t)SvIV(ST(2))
;

	purple_prpl_got_user_login_time(account, name, login_time);
    }
    XSRETURN_EMPTY;
}


XS_EUPXS(XS_Purple__Prpl_send_raw); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_Purple__Prpl_send_raw)
{
    dVAR; dXSARGS;
    if (items != 2)
       croak_xs_usage(cv,  "gc, str");
    {
	Purple__Connection	gc = purple_perl_ref_object(ST(0))
;
	const char *	str = (const char *)SvPV_nolen(ST(1))
;
#line 63 "Prpl.xs"
	PurplePluginProtocolInfo *prpl_info;
#line 326 "Prpl.c"
	int	RETVAL;
	dXSTARG;
#line 65 "Prpl.xs"
	if (!gc)
		RETVAL = 0;
	else {
		prpl_info = PURPLE_PLUGIN_PROTOCOL_INFO(gc->prpl);
		if (prpl_info && prpl_info->send_raw != NULL) {
			RETVAL = prpl_info->send_raw(gc, str, strlen(str));
		} else {
			RETVAL = 0;
		}
	}
#line 340 "Prpl.c"
	XSprePUSH; PUSHi((IV)RETVAL);
    }
    XSRETURN(1);
}

#ifdef __cplusplus
extern "C"
#endif
XS_EXTERNAL(boot_Purple__Prpl); /* prototype to pass -Wmissing-prototypes */
XS_EXTERNAL(boot_Purple__Prpl)
{
#if PERL_VERSION_LE(5, 21, 5)
    dVAR; dXSARGS;
#else
    dVAR; dXSBOOTARGSXSAPIVERCHK;
#endif
#if (PERL_REVISION == 5 && PERL_VERSION < 9)
    char* file = __FILE__;
#else
    const char* file = __FILE__;
#endif

    PERL_UNUSED_VAR(file);

    PERL_UNUSED_VAR(cv); /* -W */
    PERL_UNUSED_VAR(items); /* -W */
#if PERL_VERSION_LE(5, 21, 5)
    XS_VERSION_BOOTCHECK;
#  ifdef XS_APIVERSION_BOOTCHECK
    XS_APIVERSION_BOOTCHECK;
#  endif
#endif

        (void)newXSproto_portable("Purple::Find::prpl", XS_Purple__Find_prpl, file, "$");
        (void)newXSproto_portable("Purple::Prpl::change_account_status", XS_Purple__Prpl_change_account_status, file, "$$$");
        (void)newXSproto_portable("Purple::Prpl::get_statuses", XS_Purple__Prpl_get_statuses, file, "$$");
        (void)newXSproto_portable("Purple::Prpl::got_account_idle", XS_Purple__Prpl_got_account_idle, file, "$$$");
        (void)newXSproto_portable("Purple::Prpl::got_account_login_time", XS_Purple__Prpl_got_account_login_time, file, "$$");
        (void)newXSproto_portable("Purple::Prpl::got_user_idle", XS_Purple__Prpl_got_user_idle, file, "$$$$");
        (void)newXSproto_portable("Purple::Prpl::got_user_login_time", XS_Purple__Prpl_got_user_login_time, file, "$$$");
        (void)newXSproto_portable("Purple::Prpl::send_raw", XS_Purple__Prpl_send_raw, file, "$$");
#if PERL_VERSION_LE(5, 21, 5)
#  if PERL_VERSION_GE(5, 9, 0)
    if (PL_unitcheckav)
        call_list(PL_scopestack_ix, PL_unitcheckav);
#  endif
    XSRETURN_YES;
#else
    Perl_xs_boot_epilog(aTHX_ ax);
#endif
}

