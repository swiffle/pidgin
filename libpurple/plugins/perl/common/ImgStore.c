/*
 * This file was generated automatically by ExtUtils::ParseXS version 3.40 from the
 * contents of ImgStore.xs. Do not edit this file, edit ImgStore.xs instead.
 *
 *    ANY CHANGES MADE HERE WILL BE LOST!
 *
 */

#line 1 "ImgStore.xs"
#include "module.h"

#line 13 "ImgStore.c"
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

#line 157 "ImgStore.c"

XS_EUPXS(XS_Purple__ImgStore_add); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_Purple__ImgStore_add)
{
    dVAR; dXSARGS;
    if (items != 3)
       croak_xs_usage(cv,  "data, size, filename");
    {
	void *	data = INT2PTR(void *,SvIV(ST(0)))
;
	size_t	size = (size_t)SvUV(ST(1))
;
	const char *	filename = (const char *)SvPV_nolen(ST(2))
;
	Purple__StoredImage	RETVAL;

	RETVAL = purple_imgstore_add(data, size, filename);
	{
	    SV * RETVALSV;
	    RETVALSV = purple_perl_bless_object(RETVAL, "Purple::StoredImage");
	    RETVALSV = sv_2mortal(RETVALSV);
	    ST(0) = RETVALSV;
	}
    }
    XSRETURN(1);
}


XS_EUPXS(XS_Purple__ImgStore_add_with_id); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_Purple__ImgStore_add_with_id)
{
    dVAR; dXSARGS;
    if (items != 3)
       croak_xs_usage(cv,  "data, size, filename");
    {
	void *	data = INT2PTR(void *,SvIV(ST(0)))
;
	size_t	size = (size_t)SvUV(ST(1))
;
	const char *	filename = (const char *)SvPV_nolen(ST(2))
;
	int	RETVAL;
	dXSTARG;

	RETVAL = purple_imgstore_add_with_id(data, size, filename);
	XSprePUSH; PUSHi((IV)RETVAL);
    }
    XSRETURN(1);
}


XS_EUPXS(XS_Purple__ImgStore_find_by_id); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_Purple__ImgStore_find_by_id)
{
    dVAR; dXSARGS;
    if (items != 1)
       croak_xs_usage(cv,  "id");
    {
	int	id = (int)SvIV(ST(0))
;
	Purple__StoredImage	RETVAL;

	RETVAL = purple_imgstore_find_by_id(id);
	{
	    SV * RETVALSV;
	    RETVALSV = purple_perl_bless_object(RETVAL, "Purple::StoredImage");
	    RETVALSV = sv_2mortal(RETVALSV);
	    ST(0) = RETVALSV;
	}
    }
    XSRETURN(1);
}


XS_EUPXS(XS_Purple__ImgStore_get_data); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_Purple__ImgStore_get_data)
{
    dVAR; dXSARGS;
    if (items != 1)
       croak_xs_usage(cv,  "i");
    {
	Purple__StoredImage	i = purple_perl_ref_object(ST(0))
;
	gconstpointer	RETVAL;
	dXSTARG;

	RETVAL = purple_imgstore_get_data(i);
	XSprePUSH; PUSHi(PTR2IV(RETVAL));
    }
    XSRETURN(1);
}


XS_EUPXS(XS_Purple__ImgStore_get_filename); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_Purple__ImgStore_get_filename)
{
    dVAR; dXSARGS;
    if (items != 1)
       croak_xs_usage(cv,  "i");
    {
	Purple__StoredImage	i = purple_perl_ref_object(ST(0))
;
	const char *	RETVAL;
	dXSTARG;

	RETVAL = purple_imgstore_get_filename(i);
	sv_setpv(TARG, RETVAL); XSprePUSH; PUSHTARG;
    }
    XSRETURN(1);
}


XS_EUPXS(XS_Purple__ImgStore_get_size); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_Purple__ImgStore_get_size)
{
    dVAR; dXSARGS;
    if (items != 1)
       croak_xs_usage(cv,  "i");
    {
	Purple__StoredImage	i = purple_perl_ref_object(ST(0))
;
	size_t	RETVAL;
	dXSTARG;

	RETVAL = purple_imgstore_get_size(i);
	XSprePUSH; PUSHu((UV)RETVAL);
    }
    XSRETURN(1);
}


XS_EUPXS(XS_Purple__ImgStore_get_extension); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_Purple__ImgStore_get_extension)
{
    dVAR; dXSARGS;
    if (items != 1)
       croak_xs_usage(cv,  "i");
    {
	Purple__StoredImage	i = purple_perl_ref_object(ST(0))
;
	const char *	RETVAL;
	dXSTARG;

	RETVAL = purple_imgstore_get_extension(i);
	sv_setpv(TARG, RETVAL); XSprePUSH; PUSHTARG;
    }
    XSRETURN(1);
}


XS_EUPXS(XS_Purple__ImgStore_ref); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_Purple__ImgStore_ref)
{
    dVAR; dXSARGS;
    if (items != 1)
       croak_xs_usage(cv,  "id");
    {
	Purple__StoredImage	id = purple_perl_ref_object(ST(0))
;
	Purple__StoredImage	RETVAL;

	RETVAL = purple_imgstore_ref(id);
	{
	    SV * RETVALSV;
	    RETVALSV = purple_perl_bless_object(RETVAL, "Purple::StoredImage");
	    RETVALSV = sv_2mortal(RETVALSV);
	    ST(0) = RETVALSV;
	}
    }
    XSRETURN(1);
}


XS_EUPXS(XS_Purple__ImgStore_unref); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_Purple__ImgStore_unref)
{
    dVAR; dXSARGS;
    if (items != 1)
       croak_xs_usage(cv,  "id");
    {
	Purple__StoredImage	id = purple_perl_ref_object(ST(0))
;
	Purple__StoredImage	RETVAL;

	RETVAL = purple_imgstore_unref(id);
	{
	    SV * RETVALSV;
	    RETVALSV = purple_perl_bless_object(RETVAL, "Purple::StoredImage");
	    RETVALSV = sv_2mortal(RETVALSV);
	    ST(0) = RETVALSV;
	}
    }
    XSRETURN(1);
}


XS_EUPXS(XS_Purple__ImgStore_ref_by_id); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_Purple__ImgStore_ref_by_id)
{
    dVAR; dXSARGS;
    if (items != 1)
       croak_xs_usage(cv,  "id");
    {
	int	id = (int)SvIV(ST(0))
;

	purple_imgstore_ref_by_id(id);
    }
    XSRETURN_EMPTY;
}


XS_EUPXS(XS_Purple__ImgStore_unref_by_id); /* prototype to pass -Wmissing-prototypes */
XS_EUPXS(XS_Purple__ImgStore_unref_by_id)
{
    dVAR; dXSARGS;
    if (items != 1)
       croak_xs_usage(cv,  "id");
    {
	int	id = (int)SvIV(ST(0))
;

	purple_imgstore_unref_by_id(id);
    }
    XSRETURN_EMPTY;
}

#ifdef __cplusplus
extern "C"
#endif
XS_EXTERNAL(boot_Purple__ImgStore); /* prototype to pass -Wmissing-prototypes */
XS_EXTERNAL(boot_Purple__ImgStore)
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

        (void)newXSproto_portable("Purple::ImgStore::add", XS_Purple__ImgStore_add, file, "$$$");
        (void)newXSproto_portable("Purple::ImgStore::add_with_id", XS_Purple__ImgStore_add_with_id, file, "$$$");
        (void)newXSproto_portable("Purple::ImgStore::find_by_id", XS_Purple__ImgStore_find_by_id, file, "$");
        (void)newXSproto_portable("Purple::ImgStore::get_data", XS_Purple__ImgStore_get_data, file, "$");
        (void)newXSproto_portable("Purple::ImgStore::get_filename", XS_Purple__ImgStore_get_filename, file, "$");
        (void)newXSproto_portable("Purple::ImgStore::get_size", XS_Purple__ImgStore_get_size, file, "$");
        (void)newXSproto_portable("Purple::ImgStore::get_extension", XS_Purple__ImgStore_get_extension, file, "$");
        (void)newXSproto_portable("Purple::ImgStore::ref", XS_Purple__ImgStore_ref, file, "$");
        (void)newXSproto_portable("Purple::ImgStore::unref", XS_Purple__ImgStore_unref, file, "$");
        (void)newXSproto_portable("Purple::ImgStore::ref_by_id", XS_Purple__ImgStore_ref_by_id, file, "$");
        (void)newXSproto_portable("Purple::ImgStore::unref_by_id", XS_Purple__ImgStore_unref_by_id, file, "$");
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

