# New ports collection makefile for:	spek
# Date created:				2010-09-07
# Whom:					Alexander Kojevnikov <alexander@kojevnikov.com>
#
# $FreeBSD$
#

PORTNAME=	spek
PORTVERSION=	0.8.0
CATEGORIES=	audio
MASTER_SITES=	http://cloud.github.com/downloads/alexkay/${PORTNAME}/

MAINTAINER=	alexander@kojevnikov.com
COMMENT=	Acoustic spectrum analyser

LICENSE=	GPLv3

LIB_DEPENDS=	avcodec:${PORTSDIR}/multimedia/ffmpeg

MAN1=		spek.1

USE_XZ=	yes
USE_GMAKE=	yes
USE_GETTEXT=	yes
USE_GNOME=	desktopfileutils intltool
USE_WX=	2.8+
WX_UNICODE=	yes
WX_CONF_ARGS=	absolute
GNU_CONFIGURE=	yes
INSTALLS_ICONS=	yes
MAKE_JOBS_SAFE=	yes

post-install:
	-@update-desktop-database

post-patch:
	@${REINPLACE_CMD} -e "s,^itlocaledir =.*$$,itlocaledir = ${PREFIX}/share/locale,g" ${WRKSRC}/po/Makefile.in.in

.include <bsd.port.mk>
