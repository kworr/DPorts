# $FreeBSD$

PORTNAME=	sie-nmsg
PORTVERSION=	1.2.0
CATEGORIES=	net dns
MASTER_SITES=	FARSIGHT LOCAL/truckman/farsight

MAINTAINER=	truckman@FreeBSD.org
COMMENT=	SIE message module plugin for libnmsg

LICENSE=	APACHE20
LICENSE_FILE=	${WRKSRC}/COPYRIGHT

LIB_DEPENDS=	libprotobuf-c.so:${PORTSDIR}/devel/protobuf-c \
		libnmsg.so:${PORTSDIR}/net/nmsg \
		libwdns.so:${PORTSDIR}/dns/wdns

GNU_CONFIGURE=	yes
USES=	gmake libtool pathfix pkgconfig

INSTALL_TARGET=	install-strip

.include <bsd.port.mk>
