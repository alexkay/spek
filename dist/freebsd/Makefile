# New ports collection makefile for:	spek
# Date created:				2010-09-07
# Whom:					Alexander Kojevnikov <alexander@kojevnikov.com>
#
# $FreeBSD: ports/audio/spek/Makefile,v 1.1 2010/09/10 20:16:08 makc Exp $
#

PORTNAME=	spek
PORTVERSION=	0.6
CATEGORIES=	audio
MASTER_SITES=	${MASTER_SITE_GOOGLE_CODE}

MAINTAINER=	alexander@kojevnikov.com
COMMENT=	Acoustic spectrum analyser

LIB_DEPENDS=	avcodec.1:${PORTSDIR}/multimedia/ffmpeg

USE_BZIP2=	yes
USE_GMAKE=	yes
USE_GNOME=	gtk20 desktopfileutils
GNU_CONFIGURE=	yes
INSTALLS_ICONS=	yes
MAKE_JOBS_SAFE=	yes

post-install:
	-@update-desktop-database

.include <bsd.port.mk>
