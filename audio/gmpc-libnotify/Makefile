# Created by: Martin Wilke <miwi@FreeBSD.org>
# $FreeBSD$

PORTNAME=	gmpc-libnotify
PORTVERSION=	11.8.16
PORTREVISION=	4
CATEGORIES=	audio
MASTER_SITES=	http://download.sarine.nl/Programs/gmpc/${PORTVERSION}/

MAINTAINER=	ports@FreeBSD.org
COMMENT=	Libnotify for gmpc

LICENSE=	GPLv2 # or later
LICENSE_FILE=	${WRKSRC}/COPYING

LIB_DEPENDS=	libmpd.so:${PORTSDIR}/audio/libmpd \
		libnotify.so:${PORTSDIR}/devel/libnotify
BUILD_DEPENDS=	${LOCALBASE}/libdata/pkgconfig/gmpc.pc:${PORTSDIR}/audio/gmpc
RUN_DEPENDS=	gmpc:${PORTSDIR}/audio/gmpc

GNU_CONFIGURE=	yes
USE_GNOME=	glib20 gtk20 intltool
USES=		gettext gmake libtool pkgconfig
INSTALL_TARGET=	install-strip

PLIST_FILES=	lib/gmpc/plugins/libnotifyplugin.so

.include <bsd.port.mk>
