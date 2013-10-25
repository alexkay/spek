# Created by: Alexander Kojevnikov <alexander@kojevnikov.com>
# $FreeBSD$

PORTNAME=	spek
PORTVERSION=	0.8.2
PORTREVISION=	2
CATEGORIES=	audio
MASTER_SITES=	${MASTER_SITE_GOOGLE_CODE}

MAINTAINER=	alexander@kojevnikov.com
COMMENT=	Acoustic spectrum analyser

LICENSE=	GPLv3

LIB_DEPENDS=	libavcodec.so:${PORTSDIR}/multimedia/ffmpeg

MAN1=		spek.1

USE_XZ=		yes
NO_STAGE=	yes
USE_GCC=	4.7+
USES=		gettext gmake pkgconfig
USE_GNOME=	desktopfileutils intltool
USE_WX=	2.8+
WX_UNICODE=	yes
WX_CONF_ARGS=	absolute
GNU_CONFIGURE=	yes
INSTALLS_ICONS=	yes
OPTIONS_DEFINE=	NLS

post-install:
	-@update-desktop-database

post-patch:
	@${REINPLACE_CMD} -e "s,^itlocaledir =.*$$,itlocaledir = ${PREFIX}/share/locale,g" ${WRKSRC}/po/Makefile.in.in

.include <bsd.port.options.mk>

.if ${PORT_OPTIONS:MNLS}
USES+=		gettext
PLIST_SUB+=	NLS=""
.else
CONFIGURE_ARGS+=--disable-nls
PLIST_SUB+=	NLS="@comment "
.endif

.include <bsd.port.mk>
