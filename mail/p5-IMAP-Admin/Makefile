# Created by: Martti Kuparinen <martti.kuparinen@ericsson.com>
# $FreeBSD$

PORTNAME=	IMAP-Admin
PORTVERSION=	1.6.7
PORTREVISION=	1
CATEGORIES=	mail perl5
MASTER_SITES=	CPAN
PKGNAMEPREFIX=	p5-

MAINTAINER=	perl@FreeBSD.org
COMMENT=	Perl module for basic IMAP server administration

USES=		perl5
USE_PERL5=	configure

EXAMPLESDIR=	${PREFIX}/share/examples/${PKGBASE}

OPTIONS_DEFINE=	EXAMPLES

.include <bsd.port.options.mk>

PORTEXAMPLES=	create.pl delete.pl

post-install:
	${MKDIR} ${STAGEDIR}${EXAMPLESDIR}
	@cd ${WRKSRC}/examples && ${INSTALL_DATA} ${PORTEXAMPLES} \
		${STAGEDIR}${EXAMPLESDIR}

.include <bsd.port.mk>
