# Created by: gahr
# $FreeBSD$

PORTNAME=	itcl4
PORTVERSION=	4.0.1
CATEGORIES=	lang
MASTER_SITES=	SF/incrtcl/%5Bincr%20Tcl_Tk%5D-4-source/itcl%20${PORTVERSION}/
DISTNAME=	itcl${PORTVERSION}

MAINTAINER=	tcltk@FreeBSD.org
COMMENT=	Object-oriented extension to Tcl [incr Tcl]

USES+=		tcl:86
GNU_CONFIGURE=	yes
CONFIGURE_ARGS+=--with-tcl=${TCL_LIBDIR} \
		--with-tclinclude=${TCL_INCLUDEDIR} \
		--includedir=${PREFIX}/include/itcl${PORTVERSION:R}
PLIST_SUB+=	PKGVER=${PORTVERSION}

.include <bsd.port.mk>
