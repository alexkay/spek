# Created by: Alexander Kojevnikov <alexander@kojevnikov.com>
# $FreeBSD$

PORTNAME=	spek
PORTVERSION=	0.8.2
CATEGORIES=	audio
MASTER_SITES=	${MASTER_SITE_GOOGLE_CODE}

MAINTAINER=	alexander@kojevnikov.com
COMMENT=	Acoustic spectrum analyser

LICENSE=	GPLv3

LIB_DEPENDS=	avcodec1:${PORTSDIR}/multimedia/ffmpeg1

MAN1=		spek.1

USE_XZ=	yes
USE_GCC=	4.7+
USE_GMAKE=	yes
USE_GETTEXT=	yes
USE_GNOME=	desktopfileutils intltool
USE_WX=	2.8+
WX_UNICODE=	yes
WX_CONF_ARGS=	absolute
GNU_CONFIGURE=	yes
CONFIGURE_ENV=	FFMPEG_CFLAGS="-I${LOCALBASE}/include/ffmpeg1" \
		FFMPEG_LIBS="-lavcodec1 -lavformat1 -lavutil -L${LOCALBASE}/lib/ffmpeg1"
INSTALLS_ICONS=	yes
MAKE_JOBS_SAFE=	yes

post-install:
	-@update-desktop-database

post-patch:
	@${REINPLACE_CMD} -e "s,^itlocaledir =.*$$,itlocaledir = ${PREFIX}/share/locale,g" ${WRKSRC}/po/Makefile.in.in

.include <bsd.port.mk>
