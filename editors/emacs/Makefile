# Created by: MANTANI Nobutaka <nobutaka@nobutaka.com>
# $FreeBSD$

PORTNAME=	emacs
PORTVERSION=	${EMACS_VER}
PORTREVISION=	2
PORTEPOCH=	3
CATEGORIES=	editors ipv6
MASTER_SITES=	GNU
PKGNAMESUFFIX?= 24

PATCH_SITE_SUBDIR+=	ashish
PATCH_DIST_STRIP=	-p1

MAINTAINER=	ashish@FreeBSD.org
COMMENT=	GNU editing macros

LICENSE=	GPLv3
LICENSE_FILE=	${WRKSRC}/COPYING

CONFLICTS=	emacs-19.* emacs21-[0-9]* emacs22-[0-9]* emacs23-[0-9]* \
		xemacs-[0-9]* xemacs-devel-[0-9]* xemacs-mule-[0-9]* \
		xemacs-devel-mule-[0-9]* \
		emacs-devel-[0-9]*

EMACS_VER=	24.5
GNU_CONFIGURE=	yes
USES=		cpe gmake makeinfo ncurses pkgconfig shebangfix tar:xz
CPPFLAGS+=	-I${LOCALBASE}/include
LDFLAGS+=	-L${LOCALBASE}/lib
INFO_PATH=	${DATADIR_REL}/info
WRKSRC=		${WRKDIR}/${PORTNAME}-${PORTVERSION}
CPE_VENDOR=	gnu

SUB_FILES=	pkg-install
SUB_LIST+=	ARCHLIBDIR=${PREFIX}/libexec/emacs/${EMACS_VER}/${CONFIGURE_TARGET}
SHEBANG_FILES=	lib-src/grep-changelog

CONFIGURE_ARGS=	--localstatedir=/var

PLIST_SUB=	EMACS_VER=${EMACS_VER} GNU_HOST=${CONFIGURE_TARGET}

MAKE_ENV=	LC_ALL=C

INFO=		ada-mode auth autotype bovine calc ccmode cl dbus dired-x \
		ebrowse ede ediff edt efaq eieio eintr elisp emacs-gnutls \
		emacs-mime emacs epa erc ert eshell eudc eww flymake forms \
		gnus htmlfontify idlwave ido info mairix-el message mh-e \
		newsticker nxml-mode octave-mode org pcl-cvs pgg rcirc reftex \
		remember sasl sc semantic ses sieve smtpmail speedbar srecode \
		todo-mode tramp url vip viper widget wisent woman

OPTIONS_DEFINE=	CANNA X11 ACL DBUS GCONF GIF JPEG M17N OTF PNG SOUND SOURCES SVG TIFF SCROLLBARS \
		XFT XIM XML XPM MAGICK GNUTLS GSETTINGS LTO FILENOTIFY

OPTIONS_SINGLE=	X11 SOUND

CANNA_DESC=		Canna support
ACL_DESC=		ACL support
GSETTINGS_DESC=		GSettings support
SCROLLBARS_DESC=	Toolkit scroll-bars
SOUND_DESC=		Sound support
SOURCES_DESC=		Install sources
XAW_DESC=		Athena widgets
XAW3D_DESC=		Athena3D widgets
XIM_DESC=		X Input Method Support
M17N_DESC=		M17N support for text-shaping
OTF_DESC=		Opentype fonts suport
FILENOTIFY_DESC=	File notification support
LTO_DESC=		Enable link-time optimization (requires GCC 4.6+)

OPTIONS_SINGLE_X11=	GTK2 GTK3 XAW XAW3D MOTIF
OPTIONS_SINGLE_SOUND=	ALSA OSS

.if !defined(EMACS_NO_X11_SLAVE)
OPTIONS_DEFAULT=DBUS GCONF GIF GTK2 JPEG M17N OTF PNG SOUND SOURCES SVG TIFF \
		SCROLLBARS XFT XIM XML XPM MAGICK GNUTLS GSETTINGS OSS X11
CONFLICTS+=	emacs-nox11-[0-9]*
.else
OPTIONS_DEFAULT=SOURCES GNUTLS XML
CONFLICTS+=	emacs24-[0-9]*
.endif

OPTIONS_SUB=		SOURCES

CANNA_CONFIGURE_ON=	--with-canna \
			--with-canna-includes=${LOCALBASE}/include \
			--with-canna-libraries=${LOCALBASE}/lib
CANNA_PATCH_SITES=	http://distfiles.pirateparty.in/%SUBDIR%/ LOCAL
CANNA_PATCHFILES=	emacs24.4canna-20141030.diff.gz
CANNA_LIB_DEPENDS=	libcanna.so:${PORTSDIR}/japanese/canna-lib

SOUND_CONFIGURE_OFF=	--with-sound=no
ALSA_CONFIGURE_ON=	--with-sound=alsa
ALSA_LIB_DEPENDS=	libasound.so:${PORTSDIR}/audio/alsa-lib
OSS_CONFIGURE_ON=	--with-sound=oss

DBUS_LIB_DEPENDS=	libdbus-1.so:${PORTSDIR}/devel/dbus
DBUS_CONFIGURE_WITH=	dbus

GNUTLS_LIB_DEPENDS=	libgnutls.so:${PORTSDIR}/security/gnutls
GNUTLS_CONFIGURE_WITH=	gnutls

XML_USE=		GNOME=libxml2
XML_CONFIGURE_WITH=	xml2

SVG_USE=		GNOME=librsvg2
SVG_CONFIGURE_WITH=	rsvg

X11_CONFIGURE_WITH=	x
X11_USE=		XORG=x11
X11_USES=		desktop-file-utils

GTK2_USE=		GNOME=gtk20
GTK2_CONFIGURE_ON=	--with-x-toolkit=gtk2

GTK3_USE=		GNOME=gtk30
GTK3_CONFIGURE_ON=	--with-x-toolkit=gtk3

XAW3D_LIB_DEPENDS=	libXaw3d.so:${PORTSDIR}/x11-toolkits/Xaw3d
XAW3D_CONFIGURE_ON=	--with-x-toolkit=athena

XAW_USE=		XORG=xaw
XAW_CONFIGURE_ON+=	--with-x-toolkit=athena --without-xaw3d

MOTIF_USES=		motif
MOTIF_CONFIGURE_ON+=	--with-x-toolkit=motif

SCROLLBARS_CONFIGURE_WITH=	toolkit-scroll-bars

JPEG_USES=		jpeg
JPEG_CONFIGURE_WITH=	jpeg

TIFF_LIB_DEPENDS=	libtiff.so:${PORTSDIR}/graphics/tiff
TIFF_CONFIGURE_WITH=	tiff

GIF_LIB_DEPENDS=	libgif.so:${PORTSDIR}/graphics/giflib
GIF_CONFIGURE_WITH=	gif

PNG_LIB_DEPENDS+=	libpng.so:${PORTSDIR}/graphics/png
PNG_CONFIGURE_WITH=	png

XPM_USE=		XORG=xpm
XPM_CONFIGURE_WITH=	xpm

XFT_USE=		XORG=xft
XFT_LIB_DEPENDS=	libfreetype.so:${PORTSDIR}/print/freetype2
XFT_CONFIGURE_WITH=	xft

M17N_LIB_DEPENDS=	libm17n.so:${PORTSDIR}/devel/m17n-lib
M17N_CONFIGURE_WITH=	m17n-flt

OTF_LIB_DEPENDS=	libotf.so:${PORTSDIR}/print/libotf
OTF_CONFIGURE_WITH=	libotf

MAGICK_LIB_DEPENDS=	libMagickCore-6.so:${PORTSDIR}/graphics/ImageMagick
MAGICK_CONFIGURE_WITH=	imagemagick

GSETTINGS_USE=		gnome=glib20
GSETTINGS_CONFIGURE_WITH=	gsettings

GCONF_USE=		gnome=gconf2
GCONF_CONFIGURE_WITH=	gconf

XIM_CONFIGURE_WITH=	xim

LTO_CONFIGURE_ON=	--enable-link-time-optimization
LTO_USE=		GCC=4.6+

ACL_CONFIGURE_OFF=	--disable-acl

FILENOTIFY_CONFIGURE_ON=	--with-file-notification=gfile
FILENOTIFY_CONFIGURE_OFF=	--without-file-notification
FILENOTIFY_USE=		GNOME=glib20

.include <bsd.port.options.mk>

# clang is broken on i386 platform in -O2, due to use of alloca, and stack realignment
# and %esi as frame pointer register
# http://llvm.org/bugs/show_bug.cgi?id=18171
# fixed in r263312 (11-CURRENT), and fixed in r263765 (10-STABLE)
.if ${ARCH} == "i386"
.if (${OSVERSION} < 1000706) || (${OSVERSION} >= 1100000 && ${OSVERSION} < 1100015)
USE_GCC=		any
.endif
.endif


.if ${PORT_OPTIONS:MX11}

INSTALLS_ICONS=	yes

.if !${PORT_OPTIONS:MXFT}
.if ${PORT_OPTIONS:MM17N}
IGNORE=	m17n support for text-shaping requires Xft. Please run 'make config'
.elif ${PORT_OPTIONS:MOTF}
IGNORE=	opentype fonts support requires Xft. Please run 'make config'
.endif
.endif

.else

.for OPT in SVG GTK2 GTK3 XAW3D XAW MOTIF SCROLLBARS JPEG TIFF GIF PNG XPM XFT M17N MAGICK GCONF GSETTINGS XIM
.if ${PORT_OPTIONS:M${OPT}}
IGNORE=	option ${OPT} requires requires X11 option
.endif
.endfor

.endif

.if !${PORT_OPTIONS:MDBUS}
.if ${PORT_OPTIONS:MGSETTINGS}
IGNORE=	support for GSettings requires DBUS. Please run 'make config'
.endif
.endif

.include <bsd.port.pre.mk>

# Building with GCC 4.6+ requires it
.if ${ncurses_ARGS} == "port"
LDFLAGS+=	-ltinfo
.endif

.if ${ARCH} == "ia64"
BROKEN=	Emacs 24.X does not currently build on ia64
.endif

post-patch:
	@${REINPLACE_CMD} -e "s/%%EMACS_VER%%/${EMACS_VER}/g" -e "s/%%DATADIR%%/${DATADIR:C/\//\\\//g}/g" ${WRKSRC}/sources.el
.if empty(PORT_OPTIONS:MX11)
	@${REINPLACE_CMD} -e 's/^Terminal=.*$$/Terminal=true/' ${WRKSRC}/etc/emacs.desktop
.endif

post-install:
	@${RM} -rf ${STAGEDIR}/var/games/emacs
.if ${PORT_OPTIONS:MSOURCES}
	@${MKDIR} ${STAGEDIR}${DATADIR}/${EMACS_VER}/src
	${INSTALL_DATA} ${WRKSRC}/src/*.[ch] ${STAGEDIR}${DATADIR}/${EMACS_VER}/src
	${INSTALL_DATA} ${WRKSRC}/sources.el ${STAGEDIR}${DATADIR}/${EMACS_VER}/site-lisp/site-start.el
.endif
	@${FIND} ${STAGEDIR}${PREFIX}/${INFO_PATH} -type f -name '*.gz' -exec ${GUNZIP_CMD} '{}' \;

.if !defined(WITHOUT_X11)
pre-everything::
	@${ECHO_MSG}
	@${ECHO_MSG} "====> To disable X11 support, define: WITHOUT_X11."
	@${ECHO_MSG}
.endif

.include <bsd.port.post.mk>
