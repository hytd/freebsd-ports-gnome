# Created by: Olivier Duchateau
# $FreeBSD$

PORTNAME=	xfce4-whiskermenu-plugin
PORTVERSION=	1.5.3
CATEGORIES=	x11 xfce
MASTER_SITES=	XFCE/src/panel-plugins/${PORTNAME}/${PORTVERSION:R}
DIST_SUBDIR=	xfce4

MAINTAINER=	xfce@FreeBSD.org
COMMENT=	Alternate menu for the Xfce Desktop

LICENSE=	GPLv2
LICENSE_FILE=	${WRKSRC}/COPYING

USES=	cmake:outsource gettext localbase pkgconfig tar:bzip2 xfce
USE_GNOME=	gtk20
USE_XFCE=	panel
INSTALLS_ICONS=	yes

CMAKE_ARGS+=	-DCMAKE_INSTALL_MANDIR=${MANDIRS}

.include <bsd.port.mk>
